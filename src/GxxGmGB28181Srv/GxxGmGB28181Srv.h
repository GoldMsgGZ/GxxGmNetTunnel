#ifndef _GxxGmGB28181Srv_H_
#define _GxxGmGB28181Srv_H_

#include <string>
#include <windows.h>
#include "..\GxxPublicLib\GB28181\GB28181ServerSDK.h"

class GxxGmGB28181Srv
{
public:
	GxxGmGB28181Srv(const char *ip, int port, const char *id);
	~GxxGmGB28181Srv();

public:
	int Initialize();

public:
	static SIP_REPSOND_CODE _DevRegist_CallBackFunc(const StruRegistMsg * stuMsg, void * pUserData);
	static void _QueryResult_CallBackFunc(EnumQueryType eType, const char * czAgentGBCode, void * pMsg, void * pUserData);
	static SIP_REPSOND_CODE _NotifyInfo_CallBackFunc(EnumNotifyType eType, const char * czAgentGBCode, void * pMsg, void * pUserData);
	static SIP_REPSOND_CODE _StreamRequest_CallBackFunc(STREAM_HANDLE hStream, const char * czAgentGode, EnumStreamRequest eRequest, 
		const StruMediaInfo * pInMedia, const StruStreamDescription * pDescri, void * pUserData);
	static void _ServerLogCallBack(EnumLogLevel eLevel, const char * szTemp, int iLen, void * pUserData);

private:
	std::string ip_;
	int port_;
	std::string id_;
	SERVER_HANDLE handle_;
};

#endif//_GxxGmGB28181Srv_H_