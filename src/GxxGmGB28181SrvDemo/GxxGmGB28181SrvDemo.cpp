// GxxGmGB28181SrvDemo.cpp : �������̨Ӧ�ó������ڵ㡣
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
	GxxGmGB28181Srv srv("127.0.0.1", 5060, "12345678901234567890");
	int errCode = srv.Initialize();

	getchar();

	return 0;
}
