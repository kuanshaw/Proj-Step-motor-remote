
// ClientDlg.h : ͷ�ļ�
//

#pragma once

#include "afxwin.h"
#include "ClientSocket.h"


// CClientDlg �Ի���
class CClientDlg : public CDialogEx
{
// ����
public:
	CClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CLIENT_DIALOG };

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
	DECLARE_MESSAGE_MAP();

public:
	bool m_connect;
	CClientSocket* pSock;  // �ͻ����׽���ָ�����
	BOOL WChar2MByte(LPCWSTR lpSrc, LPSTR lpDest, int nlen); //�ַ�ת������
	virtual BOOL PreTranslateMessage(MSG* pMsg);		//��ֹ����enter��escʱ�˳�����
	void sock_send(CString str);
public:
	CButton m_ConPC;
	CString m_DataSend;
	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedConnect();
	UINT m_port;
	CString m_IP;
	CString m_speed_set;
	afx_msg void OnBnClickedMotorStart();
	CButton m_fore_t;
	CButton m_back_t;
	afx_msg void OnBnClickedRunFore();
	afx_msg void OnBnClickedRunBack();
	afx_msg void OnBnClickedMotorStop();
	afx_msg void OnBnClickedResetStep();
	afx_msg void OnBnClickedMotorSpeedUp();
	afx_msg void OnBnClickedMotorSpeedDown();
	afx_msg void OnBnClickedChangeDir();
	CString m_step_count;
	CString m_sur_speed;
	CString m_cur_state;
	CEdit m_send_edit;

	afx_msg void OnBnClickedUpdateBt();

	void BeConnect();
	void UpdataMotorData();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CButton m_bt_start;
	CButton m_bt_stop;
};
