//////////////////////////////////////////////////////////////////////////
//
// 此工程是网络通道模拟服务端

#include <iostream>
#include "GxxGmTunnelClient.h"
#include <Windows.h>

void show_usage()
{
	std::cout<<"GxxGmNetTunnelServer.exe [Tcp-ip] [Tcp-port] [Tunnel-Cli-ip] [Tunnel-Cli-port] [Tunnel-Cli-id] [Tunnel-Srv-ip] [Tunnel-Srv-port] [Tunnel-Srv-id]"<<std::endl;
	system("pause");
}

int main(int argc, const char *argv[])
{
	//////////////////////////////////////////////////////////////////////////
	// 此程序需要的参数列表：
	// 
	// * TCP绑定IP
	// * TCP绑定端口
	// * 隧道客户端IP
	// * 隧道客户端端口
	// * 隧道客户端编码
	// * 隧道服务端IP
	// * 隧道服务端端口
	// * 隧道服务端编码
	//
	// 总共8个参数
	if (argc < 9)
	{
		show_usage();
		return 0;
	}

	char tcp_ip[4096] = {0};
	char tcp_port[4096] = {0};
	char tunnel_cli_ip[4096] = {0};
	char tunnel_cli_port[4096] = {0};
	char tunnel_cli_id[4096] = {0};
	char tunnel_srv_ip[4096] = {0};
	char tunnel_srv_port[4096] = {0};
	char tunnel_srv_id[4096] = {0};

	strcpy_s(tcp_ip, 4096, argv[1]);
	strcpy_s(tcp_port, 4096, argv[2]);
	strcpy_s(tunnel_cli_ip, 4096, argv[3]);
	strcpy_s(tunnel_cli_port, 4096, argv[4]);
	strcpy_s(tunnel_cli_id, 4096, argv[5]);
	strcpy_s(tunnel_srv_ip, 4096, argv[6]);
	strcpy_s(tunnel_srv_port, 4096, argv[7]);
	strcpy_s(tunnel_srv_id, 4096, argv[8]);

	int t_port = atoi(tcp_port);
	int t_cli_port = atoi(tunnel_cli_port);
	int t_srv_port = atoi(tunnel_srv_port);

	GxxGmTunnelClient *client = new GxxGmTunnelClient();
	int errCode = client->Initialize(tcp_ip, t_port, tunnel_cli_ip, t_cli_port, tunnel_cli_id, tunnel_srv_ip, t_srv_port, tunnel_srv_id);
	if (errCode != 0)
	{
		std::cout<<"初始化网络隧道组件失败！"<<std::endl;
		system("pause");
		return errCode;
	}

	errCode = client->Start();
	if (errCode != 0)
	{
		std::cout<<"启动网络隧道失败！"<<std::endl;
		system("pause");
		return errCode;
	}

	std::cout<<"网络隧道正在运行..."<<std::endl;

	while (true)
	{
		Sleep(5000);
	}

	return 0;
}