
// NetTunnelSimulateServerDlg.h : ͷ�ļ�
//

#pragma once


// CNetTunnelSimulateServerDlg �Ի���
class CNetTunnelSimulateServerDlg : public CDialog
{
// ����
public:
	CNetTunnelSimulateServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_NETTUNNELSIMULATESERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
