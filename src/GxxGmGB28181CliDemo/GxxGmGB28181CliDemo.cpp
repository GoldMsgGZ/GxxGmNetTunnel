// GxxGmGB28181CliDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "..\\GxxGmGB28181Cli\\GxxGmGB28181Cli.h"

#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\GxxGmGB28181Cli.lib")
#pragma comment(lib, "..\\GxxPublicLib\\GB28181\\lib28181AgentSDK.lib")
#else
#pragma comment(lib, "..\\Release\\GxxGmGB28181Cli.lib")
#pragma comment(lib, "..\\GxxPublicLib\\GB28181\\lib28181AgentSDK.lib")
#endif
#else
// Linux...
#endif

int _tmain(int argc, _TCHAR* argv[])
{
	GxxGmGB28181Cli cli("192.168.199.181", 5090, "00000000000000000001");
	cli.Initialize();
	cli.Register("192.168.199.181", 5060, "00000000000000000000", "00000000000000000000", "12345678");

	getchar();
	return 0;
}

