#ifndef _GxxGmSipTunnelCli_H_
#define _GxxGmSipTunnelCli_H_

#include <string>
#include <Windows.h>


class GxxGmSipTunnelCliNotifer
{
public:
	virtual int RecvResponse(const char *response, int response_len) = 0;
};

class GxxGmSipTunnelCli
{
public:
	GxxGmSipTunnelCli(GxxGmSipTunnelCliNotifer *notifer);
	~GxxGmSipTunnelCli();

public:
	int Initialize(const char *srv_ip, int srv_port, const char *srv_id, const char *cli_ip, int cli_port, const char *cli_id);
	void Close();

public:
	int SendRequest(const char *request, int request_len);

public:
	static DWORD WINAPI MessageThread(LPVOID lpParam);
	HANDLE message_thread_handle_;
	bool is_need_stop_;

public:
	void* sip_context_;
	GxxGmSipTunnelCliNotifer *notifer_;

	std::string srv_ip_;
	int srv_port_;
	std::string srv_id_;

	std::string cli_ip_;
	int cli_port_;
	std::string cli_id_;
};

#endif//_GxxGmSipTunnelCli_H_
