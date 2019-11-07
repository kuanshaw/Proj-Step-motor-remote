// ClientSocket.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "ClientSocket.h"
#include "ClientDlg.h"
#include <stdlib.h> 

// CClientSocket

CClientSocket::CClientSocket()
{
}

CClientSocket::~CClientSocket()
{
}


int SplitString(const CString str, char split, CStringArray &strArray)
{
	strArray.RemoveAll();
	CString strTemp = str;
	int iIndex = 0;
	while (1)
	{
		iIndex = strTemp.Find(split);
		if(iIndex >= 0)
		{
			strArray.Add(strTemp.Left(iIndex));
			strTemp = strTemp.Right(strTemp.GetLength()-iIndex-1);
		}
		else
		{
			break;
		}
	}
	strArray.Add(strTemp);
 
	return strArray.GetSize();
}
// CClientSocket ��Ա����

//���ͻ��˽��յ��������˷�������ʱ����Ӧ���պ���OnReceive������ֻ�Ǽ򵥵Ľ���ȡ��
//��Ϣ��ʾ�ڱ༭���С�SendMSG���������������������Ϣ�������������Ի������е��á�
void CClientSocket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	char* pData = NULL;
	pData = new char[1024];
	memset(pData, 0, sizeof(char)* 1024);
	UCHAR leng = 0;
	CString str;
	leng = Receive(pData, 1024, 0);
	str = pData;

	CStringArray a;
	SplitString(str, ',', a);

	if(a[0] == L"0"){
		((CClientDlg*)theApp.GetMainWnd())->SetDlgItemTextW(IDC_CUR_STATE, L"ֹͣ");
	}else if(a[0] == L"1"){
		((CClientDlg*)theApp.GetMainWnd())->SetDlgItemTextW(IDC_CUR_STATE, L"��ת");
	}else if(a[0] == L"2"){
		((CClientDlg*)theApp.GetMainWnd())->SetDlgItemTextW(IDC_CUR_STATE, L"��ת");
	}
	((CClientDlg*)theApp.GetMainWnd())->SetDlgItemTextW(IDC_CUR_SPEED2, a[1]);
	((CClientDlg*)theApp.GetMainWnd())->SetDlgItemTextW(IDC_STEP_COUNTER2, a[2]);

	((CClientDlg*)theApp.GetMainWnd())->SetDlgItemTextW(IDC_DATARECEIVE, str);
	delete pData;
	pData = NULL;
	CSocket::OnReceive(nErrorCode);
}

BOOL CClientSocket::SendMSG(LPSTR lpBuff, int nlen)
{
	//����Э��ͷ
	if (Send(lpBuff, nlen) == SOCKET_ERROR)
	{
		AfxMessageBox(_T("Send error!!"));
		return FALSE;
	}
	return  TRUE;
}

