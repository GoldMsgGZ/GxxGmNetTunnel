
// NetTunnelSimulateServer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CNetTunnelSimulateServerApp:
// �йش����ʵ�֣������ NetTunnelSimulateServer.cpp
//

class CNetTunnelSimulateServerApp : public CWinAppEx
{
public:
	CNetTunnelSimulateServerApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CNetTunnelSimulateServerApp theApp;