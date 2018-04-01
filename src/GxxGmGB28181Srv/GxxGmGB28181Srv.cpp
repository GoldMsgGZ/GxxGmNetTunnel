#include "GxxGmGB28181Srv.h"


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


SIP_REPSOND_CODE GxxGmGB28181Srv::_DevRegist_CallBackFunc(const StruRegistMsg * stuMsg, void * pUserData)
{
	return 0;
}

void GxxGmGB28181Srv::_QueryResult_CallBackFunc(EnumQueryType eType, const char * czAgentGBCode, void * pMsg, void * pUserData)
{
	// 
}

SIP_REPSOND_CODE GxxGmGB28181Srv::_NotifyInfo_CallBackFunc(EnumNotifyType eType, const char * czAgentGBCode, void * pMsg, void * pUserData)
{
	return 0;
}

SIP_REPSOND_CODE GxxGmGB28181Srv::_StreamRequest_CallBackFunc(STREAM_HANDLE hStream, const char * czAgentGode, EnumStreamRequest eRequest, 
													const StruMediaInfo * pInMedia, const StruStreamDescription * pDescri, void * pUserData)
{
	return 0;
}

void GxxGmGB28181Srv::_ServerLogCallBack(EnumLogLevel eLevel, const char * szTemp, int iLen, void * pUserData)
{
	//
}