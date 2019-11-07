// ClientSocket.cpp : 实现文件
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
// CClientSocket 成员函数

//当客户端接收到服务器端发的数据时会响应接收函数OnReceive，这里只是简单的将获取的
//信息显示在编辑框中。SendMSG函数用于向服务器发送消息，函数会在主对话框类中调用。
void CClientSocket::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
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
		((CClientDlg*)theApp.GetMainWnd())->SetDlgItemTextW(IDC_CUR_STATE, L"停止");
	}else if(a[0] == L"1"){
		((CClientDlg*)theApp.GetMainWnd())->SetDlgItemTextW(IDC_CUR_STATE, L"正转");
	}else if(a[0] == L"2"){
		((CClientDlg*)theApp.GetMainWnd())->SetDlgItemTextW(IDC_CUR_STATE, L"反转");
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
	//生成协议头
	if (Send(lpBuff, nlen) == SOCKET_ERROR)
	{
		AfxMessageBox(_T("Send error!!"));
		return FALSE;
	}
	return  TRUE;
}

