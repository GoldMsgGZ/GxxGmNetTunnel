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

	// ����Ҫ���ӵķ�������Ϣ
	tcp_srv_ip_ = tcp_srv_ip;
	tcp_srv_port_ = tcp_port;

	// ��Ȼ�ˣ����ﻹ����Ҫ�Ȳ���һ��
	// ��������

	// ����SIP���
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
	// ������SIP����������������������Ȳ��
	GxxGmTunnelData tunnel_data_factory;
	int errCode = tunnel_data_factory.ParseData(request, request_len);
	if (errCode != 0)
		return errCode;

	// ȡ��ͨ��ID���ҵ���Ӧ��socket
	std::string tunnel_id = tunnel_data_factory.GetTunnelId();
	SOCKET client_socket = client_connections_.GetSocketByConnectionId(tunnel_id);
	if (client_socket == INVALID_SOCKET)
	{
		// �����ڶ�Ӧ��socket�����Ǵ���һ��
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

		// ������Ҫ���̳߳�������һ���̣߳������������ʵ���������ص�Ӧ������
		RECV_RESPONSE_THREAD_PARAM *thread_param = new RECV_RESPONSE_THREAD_PARAM;
		thread_param->tunnel_client_ = this;
		thread_param->connect_id_ = tunnel_id;
		CreateThread(NULL, 0, RecvRealServerResponseThread, thread_param, 0, NULL);
	}

	// ȡ��ʵ������
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

	// ���͸���ʵ�ķ�����
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

	// ���ȸ������ID��ȡ���׽��֣�Ȼ��ʼѭ������
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
			// ���������
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
			// һ������²�����ֽ���0�Լ����߸����ģ��������һ���ǳ�����ʲô�쳣
			errCode = WSAGetLastError();

			if (errCode == WSAESHUTDOWN)
			{
				// ���ӶϿ��ˣ���ʱ���Ҫ�����Ӧ��ӳ���ϵ
				tunnel_client->client_connections_.RemoveTunnelConnection(tunnel_id);
				break;
			}

			// �����������ӵ������Ϣ
			// ���ˣ����ǲ�������Ϊ�п���sip���ӻ�������
			// �������ͨ�����滹��Ҫ��������ͨ�����ݣ�����ͬ�����ߵ�������������д���ȷ�ϵ�����

			break;
		}
	}

	return 0;
}