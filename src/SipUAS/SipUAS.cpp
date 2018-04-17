#include "eXosip2/eXosip.h"
#include <iostream>
#include <WinSock.h>
using namespace std;

#define SERVER_IP		"192.168.182.1"
#define SERVER_PORT		5069

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
	errCode = eXosip_listen_addr(sip_context, IPPROTO_UDP, SERVER_IP, SERVER_PORT, AF_INET, 0);
	if (errCode != 0)
	{
		cout<<"监听失败！"<<endl;
		return 0;
	}
	else
		cout<<"监听成功！"<<endl;

	// 进入无限循环，等待事件
	while (true)
	{
		// 等待事件，50毫秒检查一次
		eXosip_event_t *je = eXosip_event_wait(sip_context, 0, 50);

		//协议栈带有此语句,具体作用未知
		eXosip_lock(sip_context);
		eXosip_default_action(sip_context, je);
		eXosip_automatic_refresh(sip_context);
		eXosip_unlock(sip_context);
		
		//没有接收到消息
		if (je == NULL)
			continue;

		// 收到消息了
		switch (je->type)
		{
		case EXOSIP_MESSAGE_NEW:
			// 新消息到来
			if (MSG_IS_MESSAGE(je->request))
			{
				// 显示消息
				osip_body_t *body = NULL;
				osip_message_get_body(je->request, 0, &body);
				cout<<"Recv msg : "<<body->body<<endl;

				// 应答，表示已经收到
				osip_message_t *answer = NULL;
				eXosip_message_build_answer(sip_context, je->tid, 200, &answer);
				eXosip_message_send_answer(sip_context, je->tid, 200, answer);
			}
			break;
		default:
			break;
		}
	}

	eXosip_quit(sip_context);
	return 0;
}