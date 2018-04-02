#include "GxxGmGB28181Srv.h"
#include <windows.h>
#include <iostream>


GxxGmGB28181Srv::GxxGmGB28181Srv(const char *ip, int port, const char *id)
: ip_(ip)
, port_(port)
, id_(id)
{
	
}

GxxGmGB28181Srv::~GxxGmGB28181Srv()
{

}

int GxxGmGB28181Srv::Initialize()
{
	// 初始化
	handle_ = GB28181Server_Init(1, 4, Enum28181Version::eVERSION_2016, 4000);
	if (handle_ == NULL)
	{
		return -1;
	}

	// 注册回调
	GB28181Server_SetLogCallBack(handle_, _ServerLogCallBack, this);
	GB28181Server_SetRegistCallBack(handle_, _DevRegist_CallBackFunc, this);
	GB28181Server_SetNotifyCallBack(handle_, _NotifyInfo_CallBackFunc, this);
	GB28181Server_SetStreamReqCallBack(handle_, _StreamRequest_CallBackFunc, this);

	// 开始28181服务端
	GS28181_ERR err = GB28181Server_Start(handle_, ip_.c_str(), port_, id_.c_str(), "12345678", EnumTransType::eTYPE_UDP);

	return err;
}

// 
// DWORD WINAPI GxxGmGB28181Srv::HeartBeatThread(LPVOID lpParam)
// {
// 	GxxGmGB28181Srv *srv = (GxxGmGB28181Srv*)lpParam;
// 
// 	while (true)
// 	{
// 
// 	}
// 
// 	return 0;
// }


SIP_REPSOND_CODE GxxGmGB28181Srv::_DevRegist_CallBackFunc(const StruRegistMsg * stuMsg, void * pUserData)
{
	char msg[4096] = {0};
	sprintf_s(msg, 4096, "来自 %s:%d 的平台(设备) %s 发起了注册信息", stuMsg->stuCnnParam.szIP, stuMsg->stuCnnParam.iPort, stuMsg->stuCnnParam.szGBCode);
	std::cout<<msg<<std::endl;
	return SIP_RESPONSE_CODE_SUCCESS;
}

void GxxGmGB28181Srv::_QueryResult_CallBackFunc(EnumQueryType eType, const char * czAgentGBCode, void * pMsg, void * pUserData)
{
	// 
	char msg[4096] = {0};
	sprintf_s(msg, 4096, "");
	std::cout<<msg<<std::endl;
}

SIP_REPSOND_CODE GxxGmGB28181Srv::_NotifyInfo_CallBackFunc(EnumNotifyType eType, const char * czAgentGBCode, void * pMsg, void * pUserData)
{
	char msg[4096] = {0};
	sprintf_s(msg, 4096, "");
	std::cout<<msg<<std::endl;
	return SIP_RESPONSE_CODE_SUCCESS;
}

SIP_REPSOND_CODE GxxGmGB28181Srv::_StreamRequest_CallBackFunc(STREAM_HANDLE hStream, const char * czAgentGode, EnumStreamRequest eRequest, 
													const StruMediaInfo * pInMedia, const StruStreamDescription * pDescri, void * pUserData)
{
	char msg[4096] = {0};
	sprintf_s(msg, 4096, "");
	std::cout<<msg<<std::endl;
	return SIP_RESPONSE_CODE_SUCCESS;
}

void GxxGmGB28181Srv::_ServerLogCallBack(EnumLogLevel eLevel, const char * szTemp, int iLen, void * pUserData)
{
	//
	char msg[4096] = {0};
	sprintf_s(msg, 4096, "");
	std::cout<<msg<<std::endl;
}