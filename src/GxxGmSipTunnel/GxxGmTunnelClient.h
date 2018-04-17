//////////////////////////////////////////////////////////////////////////
//
// 

#ifndef _GxxGmTunnelClient_H_
#define _GxxGmTunnelClient_H_

#include "GxxGmSipTunnelSrv.h"
#include "GxxGmTunnelData.h"
#include <string>

typedef struct _CLIENT_CONNECTION_INFO_
{
	SOCKET client_socket_;
	std::string connection_id_;

} CLIENT_CONNECTION_INFO, *PCLIENT_CONNECTION_INFO;

class GxxGmTunnelClient : public GxxGmSipTunnelSrvNotifer
{
public:
	GxxGmTunnelClient();
	~GxxGmTunnelClient();

public:
	/**
	 * ��ʼ��������������
	 * ������
	 *	@tcp_srv_ip		Ŀ������ip��ַ
	 *	@tcp_srv_port	Ŀ����񿪷ŵĶ˿ں�
	 *	@sip_cli_ip		SIP����ͻ���IP���Զ�IP��
	 *	@sip_cli_port	SIP����ͻ��˶˿ںţ��Զ˶˿ڣ�
	 *	@sip_cli_id		SIP����ͻ��˱��루�Զ˱��룩
	 *	@sip_srv_ip		SIP��������IP������IP��
	 *	@sip_srv_port	SIP�������˶˿ںţ����˶˿ڣ�
	 *	@sip_srv_id		SIP�������˱��루���˱��룩
	 */
	int Initialize(const char *tcp_srv_ip, int tcp_port, const char *sip_cli_ip, int sip_cli_port, const char *sip_cli_id, const char *sip_srv_ip, int sip_srv_port, const char *sip_srv_id);
	void Close();

public:
	int Start();
	void Stop();

public:
	virtual int RecvRequest(const char *request, int request_len);

public:
	static DWORD WINAPI RecvRealServerResponseThread(LPVOID lpParam);

public:
	GxxGmSipTunnelSrv *sip_tunnel_srv_;

public:
	GxxGmTunnelConnections client_connections_;

	std::string tcp_srv_ip_;
	int tcp_srv_port_;
};

#endif//_GxxGmTunnelClient_H_