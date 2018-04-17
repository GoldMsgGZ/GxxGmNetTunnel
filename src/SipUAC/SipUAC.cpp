#include "eXosip2/eXosip.h"
#include <iostream>
#include <WinSock.h>
using namespace std;

#define SERVER_IP		"192.168.182.1"
#define SERVER_PORT		5069
#define SERVER_ID		"00000000000000000000"

#define CLI_IP			"192.168.182.1"
#define CLI_PORT		5070
#define CLI_ID			"00000000000000000001"

int main(int argc, const char *argv[])
{
	eXosip_t *sip_context = eXosip_malloc();

	// 初始化
	int errCode = eXosip_init(sip_context);
	if (errCode != 0)
	{
		cout<<"初始化失败！"<<endl;
		return 0;
	}

	// 监听
	errCode = eXosip_listen_addr(sip_context, IPPROTO_UDP, CLI_IP, CLI_PORT, AF_INET, 0);
	if (errCode != 0)
	{
		cout<<"监听失败！"<<endl;
		return 0;
	}
	else
		cout<<"监听成功！"<<endl;

	// 构建源请求
	char dest_call[4096] = {0};
	sprintf_s(dest_call, 4096, "sip:%s@%s:%d", SERVER_ID, SERVER_IP, SERVER_PORT);

	char src_call[4096] = {0};
	sprintf_s(src_call, 4096, "sip:%s@%s:%d", CLI_ID, CLI_IP, CLI_PORT);

	// 发送请求
	osip_message_t *message = NULL;
	eXosip_message_build_request(sip_context, &message, "MESSAGE", dest_call, src_call, NULL);

	char msg[4096] = {0};
	sprintf_s(msg, 4096, "This is a sip message(Method:MESSAGE)");
	osip_message_set_body(message, msg, strlen(msg));

	osip_message_set_content_type(message, "text/xml");
	errCode = eXosip_message_send_request(sip_context, message);

	return 0;
}