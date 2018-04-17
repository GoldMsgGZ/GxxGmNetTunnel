#ifndef _GxxGmTunnelServer_H_
#define _GxxGmTunnelServer_H_

#include "GxxGmSipTunnelCli.h"
#include "GxxGmTunnelData.h"
#include <winsock.h>
#include <map>

typedef struct _TUNNEL_MSG_
{
	SOCKET client_socket_;	// �ͻ��˽����SOCKET
	char socket_index_[40];	// �ͻ���SOCKET-ID��һ��GUID

} TUNNEL_MSG, *PTUNNEL_MSG;

class GxxGmTunnelServer : public GxxGmSipTunnelCliNotifer
{
public:
	GxxGmTunnelServer();
	~GxxGmTunnelServer();

public:
	/**
	 * ��ʼ��������������
	 * ������
	 *	@tcp_srv_ip		���ڰ󶨼�����IP�������������IP���0.0.0.0��
	 *	@tcp_srv_port	TCP�����˿�
	 *	@sip_cli_ip		SIP����ͻ���IP������IP��
	 *	@sip_cli_port	SIP����ͻ��˶˿ںţ����˶˿ڣ�
	 *	@sip_cli_id		SIP����ͻ��˱��루���˱��룩
	 *	@sip_srv_ip		SIP��������IP���Զ�IP��
	 *	@sip_srv_port	SIP�������˶˿ںţ��Զ˶˿ڣ�
	 *	@sip_srv_id		SIP�������˱��루�Զ˱��룩
	 */
	int Initialize(const char *tcp_srv_ip, int tcp_srv_port, const char *sip_cli_ip, int sip_cli_port, const char *sip_cli_id, const char *sip_srv_ip, int sip_srv_port, const char *sip_srv_id);
	void Close();

public:
	int Start();
	void Stop();

public:
	static DWORD WINAPI ListenThread(LPVOID lpParam);
	HANDLE listen_thread_handle_;
	bool is_need_stop_;

public:
	static DWORD WINAPI RecvClientThread(LPVOID lpParam);
	//static DWORD WINAPI SendClientThread(LPVOID lpParam);

public:
	virtual int RecvResponse(const char *response, int response_len);

public:
	SOCKET srv_sock_;

public:
	GxxGmSipTunnelCli *sip_tunnel_cli_;

public:
	GxxGmTunnelConnections client_connections_;
};

#endif//_GxxGmTunnelServer_H_