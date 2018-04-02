#include "GxxGmGB28181Cli.h"
#include <windows.h>
#include <string>

GxxGmGB28181Cli::GxxGmGB28181Cli(const char *ip, int port, const char *id)
: ip_(ip)
, port_(port)
, id_(id)
, handle_(NULL)
{
	handle_ = GB28181Agent_Init(1, 4, Enum28181Version::eVERSION_2016, 4000);
}

GxxGmGB28181Cli::~GxxGmGB28181Cli()
{

}

int GxxGmGB28181Cli::Initialize()
{
	GS28181_ERR err = GB28181Agent_Start(handle_, ip_.c_str(), port_, id_.c_str(), EnumTransType::eTYPE_UDP);
	return err;
}

int GxxGmGB28181Cli::Register(const char *srv_ip, int srv_port, const char *srv_id, const char *authen_user, const char *authen_pass)
{
	StruRegistMsg reg_msg;
	reg_msg.iExpires = 4000;
	strcpy(reg_msg.stuCnnParam.szIP, srv_ip);
	reg_msg.stuCnnParam.iPort = port_;
	strcpy(reg_msg.stuCnnParam.szGBCode, srv_id);
	strcpy(reg_msg.szUserName, authen_user);
	strcpy(reg_msg.szPassword, authen_pass);

	srv_ip_ = srv_ip;
	srv_id_ = srv_id;
	srv_port_ = srv_port;

	//char tm[4096] = {0};
	GS28181_ERR err = GS28181_ERR_SUCCESS;
	do 
	{
		err = GB28181Agent_Register(handle_, &reg_msg, NULL);
		if (err == GS28181_ERR_SUCCESS)
		{
			CreateThread(NULL, 0, HeartBeatThread, this, 0, NULL);
			break;
		}

		printf("×¢²áÊ§°Ü£¡´íÎóÂë£º%d\n", err);
		Sleep(1000);
	} while (err != GS28181_ERR_SUCCESS);

	printf("×¢²áµ½·þÎñÆ÷³É¹¦£¡\n");
	
	return 0;
}

int GxxGmGB28181Cli::SendData(const char *data, int data_len)
{
	return 0;
}

DWORD WINAPI GxxGmGB28181Cli::HeartBeatThread(LPVOID lpParam)
{
	GxxGmGB28181Cli *cli = (GxxGmGB28181Cli *)lpParam;

	while (true)
	{
		StruConnectParam conn_param;
		strcpy(conn_param.szIP, cli->srv_ip_.c_str());
		strcpy(conn_param.szGBCode, cli->srv_id_.c_str());
		conn_param.iPort = cli->srv_port_;
		GS28181_ERR err = GB28181Agent_HeartBeat(cli->handle_, &conn_param, 1, NULL);

		Sleep(1000);
	}
}