#include "eXosip2/eXosip.h"
#include <iostream>
#include <WinSock.h>
using namespace std;

#define SERVER_IP		"192.168.182.1"
#define SERVER_PORT		5069

int main(int argc, const char *argv[])
{
	eXosip_t *sip_context = eXosip_malloc();

	// ��ʼ��
	int errCode = eXosip_init(sip_context);
	if (errCode != 0)
	{
		cout<<"��ʼ��ʧ�ܣ�"<<endl;
		return 0;
	}

	// ����
	errCode = eXosip_listen_addr(sip_context, IPPROTO_UDP, SERVER_IP, SERVER_PORT, AF_INET, 0);
	if (errCode != 0)
	{
		cout<<"����ʧ�ܣ�"<<endl;
		return 0;
	}
	else
		cout<<"�����ɹ���"<<endl;

	// ��������ѭ�����ȴ��¼�
	while (true)
	{
		// �ȴ��¼���50������һ��
		eXosip_event_t *je = eXosip_event_wait(sip_context, 0, 50);

		//Э��ջ���д����,��������δ֪
		eXosip_lock(sip_context);
		eXosip_default_action(sip_context, je);
		eXosip_automatic_refresh(sip_context);
		eXosip_unlock(sip_context);
		
		//û�н��յ���Ϣ
		if (je == NULL)
			continue;

		// �յ���Ϣ��
		switch (je->type)
		{
		case EXOSIP_MESSAGE_NEW:
			// ����Ϣ����
			if (MSG_IS_MESSAGE(je->request))
			{
				// ��ʾ��Ϣ
				osip_body_t *body = NULL;
				osip_message_get_body(je->request, 0, &body);
				cout<<"Recv msg : "<<body->body<<endl;

				// Ӧ�𣬱�ʾ�Ѿ��յ�
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