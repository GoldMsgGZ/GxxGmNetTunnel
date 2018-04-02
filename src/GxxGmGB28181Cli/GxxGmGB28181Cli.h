#ifndef _GxxGmGB28181Cli_H_
#define _GxxGmGB28181Cli_H_

#include <string>
#include <windows.h>
#include "..\GxxPublicLib\GB28181\GB28181AgentSDK.h"

class GxxGmGB28181Cli
{
public:
	GxxGmGB28181Cli(const char *ip, int port, const char *id);
	~GxxGmGB28181Cli();

public:
	int Initialize();
	int Register(const char *srv_ip, int srv_port, const char *srv_id, const char *authen_user, const char *authen_pass);
	int SendData(const char *data, int data_len);

public:
	static DWORD WINAPI HeartBeatThread(LPVOID lpParam);

public:
	std::string ip_;
	int port_;
	std::string id_;

	std::string srv_ip_;
	int srv_port_;
	std::string srv_id_;

	AGENT_HANDLE handle_;
};

#endif//_GxxGmGB28181Cli_H_