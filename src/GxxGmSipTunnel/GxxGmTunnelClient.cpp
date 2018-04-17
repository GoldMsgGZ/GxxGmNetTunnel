#include "eXosip2/eXosip.h"
#include "GxxGmTunnelClient.h"


typedef struct _RECV_RESPONSE_THREAD_PARAM_
{
	GxxGmTunnelClient *tunnel_client_;
	std::string connect_id_;
} RECV_RESPONSE_THREAD_PARAM, *PRECV_RESPONSE_THREAD_PARAM;


GxxGmTunnelClient::GxxGmTunnelClient()
: sip_tunnel_srv_(new GxxGmSipTunnelSrv(this))
{

}

GxxGmTunnelClient::~GxxGmTunnelClient()
{

}

int GxxGmTunnelClient::Initialize(const char *tcp_srv_ip, int tcp_port, const char *sip_cli_ip, int sip_cli_port, const char *sip_cli_id, const char *sip_srv_ip, int sip_srv_port, const char *sip_srv_id)
{
	int errCode = 0;

	// 保存要连接的服务器信息
	tcp_srv_ip_ = tcp_srv_ip;
	tcp_srv_port_ = tcp_port;

	// 当然了，这里还是需要先测试一下
	// 不测试了

	// 开启SIP隧道
	errCode = sip_tunnel_srv_->Initialize(sip_srv_ip, sip_srv_port, sip_srv_id, sip_cli_ip, sip_cli_port, sip_cli_id);
	if (errCode != 0)
		return errCode;

	return errCode;
}

void GxxGmTunnelClient::Close()
{

}

int GxxGmTunnelClient::Start()
{
	return 0;
}

void GxxGmTunnelClient::Stop()
{

}

int GxxGmTunnelClient::RecvRequest(const char *request, int request_len)
{
	// 这里是SIP隧道传过来的请求，我们首先拆包
	GxxGmTunnelData tunnel_data_factory;
	int errCode = tunnel_data_factory.ParseData(request, request_len);
	if (errCode != 0)
		return errCode;

	// 取出通道ID，找到对应的socket
	std::string tunnel_id = tunnel_data_factory.GetTunnelId();
	SOCKET client_socket = client_connections_.GetSocketByConnectionId(tunnel_id);
	if (client_socket == INVALID_SOCKET)
	{
		// 不存在对应的socket，咱们创建一个
		client_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (client_socket == INVALID_SOCKET)
		{
			errCode = WSAGetLastError();
			return errCode;
		}
		
		SOCKADDR_IN srv_addr;
		srv_addr.sin_family = AF_INET;
		srv_addr.sin_addr.S_un.S_addr = inet_addr(tcp_srv_ip_.c_str());
		srv_addr.sin_port = htons(tcp_srv_port_);

		errCode = connect(client_socket, (SOCKADDR*)&srv_addr, sizeof(SOCKADDR));
		if (errCode == SOCKET_ERROR)
		{
			errCode = WSAGetLastError();
			return errCode;
		}

		SOCKET ret = client_connections_.HandleClientSocket(client_socket, tunnel_id);
		if (ret == INVALID_SOCKET)
			return -6001;

		// 这里需要从线程池中启动一个线程，来处理接收真实服务器返回的应答数据
		RECV_RESPONSE_THREAD_PARAM *thread_param = new RECV_RESPONSE_THREAD_PARAM;
		thread_param->tunnel_client_ = this;
		thread_param->connect_id_ = tunnel_id;
		CreateThread(NULL, 0, RecvRealServerResponseThread, thread_param, 0, NULL);
	}

	// 取出实际数据
	char *real_request_data = NULL;
	int real_request_data_len = 0;
	while (true)
	{
		errCode = tunnel_data_factory.GetTunnelData(real_request_data, &real_request_data_len);
		if (errCode == 0)
			break;
		else if (errCode == -1)
		{
			if (real_request_data)
				delete [] real_request_data;

			real_request_data = new char[real_request_data_len];
		}
		else if (errCode == -2)
		{
			if (real_request_data)
				delete [] real_request_data;

			real_request_data = new char[real_request_data_len];
		}
	}

	// 发送给真实的服务器
	errCode = send(client_socket, real_request_data, real_request_data_len, 0);
	if (errCode != real_request_data_len)
		errCode = WSAGetLastError();
	else
		errCode = 0;

	delete [] real_request_data;
	real_request_data_len = NULL;

	return errCode;
}

DWORD GxxGmTunnelClient::RecvRealServerResponseThread(LPVOID lpParam)
{
	RECV_RESPONSE_THREAD_PARAM *thread_param = (RECV_RESPONSE_THREAD_PARAM *)lpParam;
	int errCode = 0;

	// 首先根据隧道ID，取出套接字，然后开始循环接收
	std::string tunnel_id = thread_param->connect_id_;
	GxxGmTunnelClient *tunnel_client = thread_param->tunnel_client_;

	SOCKET client_socket = tunnel_client->client_connections_.GetSocketByConnectionId(tunnel_id);
	if (client_socket == INVALID_SOCKET)
	{
		return -6002;
	}

	while (true)
	{
		char real_response_data[512] = {0};
		int recv_len = recv(client_socket, real_response_data, 512, 0);
		if (recv_len > 0)
		{
			// 在这里组包
			GxxGmTunnelData tunnel_data_factory;
			tunnel_data_factory.SetTunnelId(tunnel_id.c_str(), tunnel_id.size());
			tunnel_data_factory.SetTunnelData(real_response_data, recv_len);

			int tunnel_data_len = tunnel_data_factory.GetSerializedDataLength();
			char *tunnel_data = new char[tunnel_data_len];
			tunnel_data_factory.GetSerializedData(tunnel_data, tunnel_data_len);

			errCode = tunnel_client->sip_tunnel_srv_->SendResponse(tunnel_data, tunnel_data_len);
		}
		else
		{
			// 一般情况下不会出现接收0自己或者负数的，这种情况一定是出现了什么异常
			errCode = WSAGetLastError();

			if (errCode == WSAESHUTDOWN)
			{
				// 连接断开了，这时候就要清掉对应的映射关系
				tunnel_client->client_connections_.RemoveTunnelConnection(tunnel_id);
				break;
			}

			// 清理掉这个连接的相关信息
			// 算了，还是不清理，因为有可能sip连接还建立着
			// 后面隧道通信里面还需要增加数据通信内容，用于同步两边的连接情况，还有传输确认的事情

			break;
		}
	}

	return 0;
}