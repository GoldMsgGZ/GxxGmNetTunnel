// GxxGmGB28181SrvDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "..\\GxxGmGB28181Srv\\GxxGmGB28181Srv.h"

#ifdef _WIN32
	#ifdef _DEBUG
		#pragma comment(lib, "..\\Debug\\GxxGmGB28181Srv.lib")
		#pragma comment(lib, "..\\GxxPublicLib\\GB28181\\lib28181ServerSDK.lib")
	#else
		#pragma comment(lib, "..\\Release\\GxxGmGB28181Srv.lib")
		#pragma comment(lib, "..\\GxxPublicLib\\GB28181\\lib28181ServerSDK.lib")
	#endif
#else
// Linux...
#endif


int _tmain(int argc, _TCHAR* argv[])
{
	GxxGmGB28181Srv srv("192.168.199.181", 5060, "00000000000000000000");
	int errCode = srv.Initialize();

	getchar();

	return 0;
}

