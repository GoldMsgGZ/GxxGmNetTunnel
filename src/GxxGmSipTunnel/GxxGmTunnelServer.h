#ifndef _GxxGmTunnelServer_H_
#define _GxxGmTunnelServer_H_

#include "GxxGmSipTunnelCli.h"
#include "GxxGmTunnelData.h"
#include <winsock.h>
#include <map>

typedef struct _TUNNEL_MSG_
{
	SOCKET client_socket_;	// 客户端接入的SOCKET
	char socket_index_[40];	// 客户端SOCKET-ID，一个GUID

} TUNNEL_MSG, *PTUNNEL_MSG;

class GxxGmTunnelServer : public GxxGmSipTunnelCliNotifer
{
public:
	GxxGmTunnelServer();
	~GxxGmTunnelServer();

public:
	/**
	 * 初始化网络连接配置
	 * 参数：
	 *	@tcp_srv_ip		用于绑定监听的IP，如果监听所有IP则填“0.0.0.0”
	 *	@tcp_srv_port	TCP监听端口
	 *	@sip_cli_ip		SIP隧道客户端IP（本端IP）
	 *	@sip_cli_port	SIP隧道客户端端口号（本端端口）
	 *	@sip_cli_id		SIP隧道客户端编码（本端编码）
	 *	@sip_srv_ip		SIP隧道服务端IP（对端IP）
	 *	@sip_srv_port	SIP隧道服务端端口号（对端端口）
	 *	@sip_srv_id		SIP隧道服务端编码（对端编码）
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