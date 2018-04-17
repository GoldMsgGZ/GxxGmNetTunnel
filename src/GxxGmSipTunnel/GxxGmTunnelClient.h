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
	 * 初始化网络连接配置
	 * 参数：
	 *	@tcp_srv_ip		目标服务的ip地址
	 *	@tcp_srv_port	目标服务开放的端口号
	 *	@sip_cli_ip		SIP隧道客户端IP（对端IP）
	 *	@sip_cli_port	SIP隧道客户端端口号（对端端口）
	 *	@sip_cli_id		SIP隧道客户端编码（对端编码）
	 *	@sip_srv_ip		SIP隧道服务端IP（本端IP）
	 *	@sip_srv_port	SIP隧道服务端端口号（本端端口）
	 *	@sip_srv_id		SIP隧道服务端编码（本端编码）
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