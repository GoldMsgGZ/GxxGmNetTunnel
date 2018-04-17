#include "GxxGmTunnelServer.h"


typedef struct _CLIENT_INFO_
{
	SOCKET cli_sock_;
	SOCKADDR_IN cli_addr_;
	GxxGmTunnelServer *srv_;
	std::string cli_sock_id_;
} CLIENT_INFO, *PCLIENT_INFO;


GxxGmTunnelServer::GxxGmTunnelServer()
: listen_thread_handle_(NULL)
, is_need_stop_(false)
, sip_tunnel_cli_(NULL)
{

}

GxxGmTunnelServer::~GxxGmTunnelServer()
{

}

int GxxGmTunnelServer::Initialize(const char *tcp_srv_ip, int tcp_srv_port, const char *sip_cli_ip, int sip_cli_port, const char *sip_cli_id, const char *sip_srv_ip, int sip_srv_port, const char *sip_srv_id)
{
	int errCode = 0;

	//// 首先开启TCP监听
	//WSAData wsaData;
	//errCode = WSAStartup(MAKEWORD(2,0), &wsaData);
	//if (errCode != 0)
	//	return errCode;

	//srv_sock_ = socket(AF_INET, SOCK_STREAM, 0);
	//if (srv_sock_ == INVALID_SOCKET)
	//{
	//	errCode = WSAGetLastError();
	//	return errCode;
	//}

	//SOCKADDR_IN srv_addr;
	//srv_addr.sin_family = AF_INET;
	//srv_addr.sin_port = htons(tcp_srv_port);
	//srv_addr.sin_addr.S_un.S_addr = inet_addr(tcp_srv_ip);

	//errCode = bind(srv_sock_, (SOCKADDR*)&srv_addr, sizeof(SOCKADDR));
	//if (errCode == SOCKET_ERROR)
	//{
	//	errCode = WSAGetLastError();
	//	return errCode;
	//}

	//errCode = listen(srv_sock_, 5);
	//if (errCode == SOCKET_ERROR)
	//{
	//	errCode = WSAGetLastError();
	//	return errCode;
	//}

	sip_tunnel_cli_ = new GxxGmSipTunnelCli(this);
	errCode = sip_tunnel_cli_->Initialize(sip_srv_ip, sip_srv_port, sip_srv_id, sip_cli_ip, sip_cli_port, sip_cli_id);

	return errCode;
}

void GxxGmTunnelServer::Close()
{
	this->Stop();
	WSACleanup();
}

int GxxGmTunnelServer::Start()
{
	int errCode = 0;

	// 启动接收线程
	DWORD exit_code = 0;
	GetExitCodeThread(listen_thread_handle_, &exit_code);
	if (exit_code == STILL_ACTIVE)
	{
		errCode = 0;
	}
	else
	{
		listen_thread_handle_ = CreateThread(NULL, 0, ListenThread, this, 0, NULL);
		if (listen_thread_handle_ == NULL)
			errCode = GetLastError();
	}

	is_need_stop_ = false;
	return errCode;
}

void GxxGmTunnelServer::Stop()
{
	is_need_stop_ = true;
}

int GxxGmTunnelServer::RecvResponse(const char *response, int response_len)
{
	// 首先解包
	GxxGmTunnelData tunnel_data_factory;
	int errCode = tunnel_data_factory.ParseData(response, response_len);
	if (errCode != 0)
		return errCode;

	// 取出通道ID，找到对应的socket
	std::string tunnel_id = tunnel_data_factory.GetTunnelId();
	SOCKET client_socket = client_connections_.GetSocketByConnectionId(tunnel_id);
	if (client_socket == INVALID_SOCKET)
	{
		// 不存在对应的socket
		return -6002;
	}

	// 取出实际数据
	char *real_response_data = NULL;
	int real_response_data_len = 0;
	while (true)
	{
		errCode = tunnel_data_factory.GetTunnelData(real_response_data, &real_response_data_len);
		if (errCode == 0)
			break;
		else if (errCode == -1)
		{
			if (real_response_data)
				delete [] real_response_data;

			real_response_data = new char[real_response_data_len];
		}
		else if (errCode == -2)
		{
			if (real_response_data)
				delete [] real_response_data;

			real_response_data = new char[real_response_data_len];
		}
	}

	// 发送给客户端
	errCode = send(client_socket, response, response_len, 0);
	if (errCode != real_response_data_len)
		errCode = WSAGetLastError();
	else
		errCode = 0;

	delete [] real_response_data;
	real_response_data = NULL;

	return errCode;
}

DWORD WINAPI GxxGmTunnelServer::ListenThread(LPVOID lpParam)
{
	GxxGmTunnelServer *server = (GxxGmTunnelServer *)lpParam;

	while (true)
	{
		SOCKADDR_IN client_addr;
		int addr_len = sizeof(SOCKADDR);
		SOCKET client_sock = accept(server->srv_sock_, (SOCKADDR *)&client_addr, &addr_len);

		// 如果接入的是无效的客户端，跳过
		if (client_sock == INVALID_SOCKET)
		{
			int errCode = WSAGetLastError();
			if (errCode == WSAEINTR)
			{
				// socket被关闭
				break;
			}
			else
			{
				continue;
			}
		}

		server->client_connections_.HandleClientSocket(client_sock);

		// 每接收到一个客户端的接入请求后，都在线程池里面新建一个任务进行处理
		// 很想用Chromium那套东西来实现...
		CLIENT_INFO *client_info = new CLIENT_INFO;
		client_info->cli_sock_ = client_sock;
		client_info->cli_addr_ = client_addr;
		client_info->srv_ = server;
		CreateThread(NULL, 0, GxxGmTunnelServer::RecvClientThread, client_info, 0, NULL);
	}

	return 0;
}

DWORD WINAPI GxxGmTunnelServer::RecvClientThread(LPVOID lpParam)
{
	CLIENT_INFO *client_info = (CLIENT_INFO *)lpParam;

	while (true)
	{
		// 收到客户端发来的请求后，重构消息，扔给SIP隧道
		char recv_buffer[512] = {0};
		int recv_len = recv(client_info->cli_sock_, recv_buffer, 4096, 0);
		if (recv_len > 0)
		{
			// 这里重新构建消息，消息算法为：Base64(client_sock_id + message)
			std::string tunnel_id = client_info->srv_->client_connections_.GetConnectionIdBySocket(client_info->cli_sock_);

			GxxGmTunnelData tunnel_data_factory;
			tunnel_data_factory.SetTunnelId(tunnel_id.c_str(), tunnel_id.size());
			tunnel_data_factory.SetTunnelData(recv_buffer, recv_len);

			int tunnel_data_len = tunnel_data_factory.GetSerializedDataLength();
			char *tunnel_data = new char[tunnel_data_len];
			tunnel_data_factory.GetSerializedData(tunnel_data, tunnel_data_len);

			int errCode = client_info->srv_->sip_tunnel_cli_->SendRequest(tunnel_data, tunnel_data_len);
		}
	}

	return 0;
}