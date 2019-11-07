
// ClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"
#include <stdlib.h> 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CClientDlg 对话框

CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDlg::IDD, pParent)
	, m_connect(false)
	, m_port(8123)
	, m_IP(_T("192.168.4.1"))
	, m_speed_set(_T("05.00"))
	, m_step_count(_T("0"))
	, m_sur_speed(_T("00.00"))
	, m_cur_state(_T("stop"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONNECT, m_ConPC);
	DDX_Text(pDX, IDC_DATASEND, m_DataSend);
	DDX_Text(pDX, IDC_PORT, m_port);		//端口
	//  DDX_Text(pDX, IDC_IP, m_IP);			//IP地址
	DDX_Text(pDX, IDC_IP, m_IP);
	DDX_Text(pDX, IDC_MOTOR_SET_SPEED, m_speed_set);
	DDX_Control(pDX, IDC_RUN_FORE, m_fore_t);
	DDX_Control(pDX, IDC_RUN_BACK, m_back_t);
	DDX_Text(pDX, IDC_STEP_COUNTER2, m_step_count);
	DDX_Text(pDX, IDC_CUR_SPEED2, m_sur_speed);
	DDX_Text(pDX, IDC_CUR_STATE, m_cur_state);
	DDX_Control(pDX, IDC_DATASEND, m_send_edit);
	DDX_Control(pDX, IDC_MOTOR_START, m_bt_start);
	DDX_Control(pDX, IDC_MOTOR_STOP, m_bt_stop);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND, &CClientDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_CONNECT, &CClientDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_MOTOR_START, &CClientDlg::OnBnClickedMotorStart)
	ON_BN_CLICKED(IDC_RUN_FORE, &CClientDlg::OnBnClickedRunFore)
	ON_BN_CLICKED(IDC_RUN_BACK, &CClientDlg::OnBnClickedRunBack)
	ON_BN_CLICKED(IDC_MOTOR_STOP, &CClientDlg::OnBnClickedMotorStop)
	ON_BN_CLICKED(IDC_RESET_STEP, &CClientDlg::OnBnClickedResetStep)
	ON_BN_CLICKED(IDC_MOTOR_SPEED_UP, &CClientDlg::OnBnClickedMotorSpeedUp)
	ON_BN_CLICKED(IDC_MOTOR_SPEED_DOWN, &CClientDlg::OnBnClickedMotorSpeedDown)
	ON_BN_CLICKED(IDC_CHANGE_DIR, &CClientDlg::OnBnClickedChangeDir)
	ON_BN_CLICKED(IDC_UPDATE_BT, &CClientDlg::OnBnClickedUpdateBt)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CClientDlg 消息处理程序

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_fore_t.SetCheck(true);
	m_back_t.SetCheck(false);

	GetDlgItem(IDC_DATARECEIVE)->ShowWindow(false);
	GetDlgItem(IDC_DATASEND)->ShowWindow(false);
	GetDlgItem(IDC_SEND)->ShowWindow(false);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CClientDlg::UpdataMotorData()
{
	if (!m_connect)return;     //未连接服务器则不执行
	UpdateData(true);          //获取界面数据

	sock_send(L"AB000000");		// Get Data

	UpdateData(false);
}

void CClientDlg::BeConnect()
{
	if (!m_connect)return;     //未连接服务器则不执行
	UpdateData(true);          //获取界面数据

	sock_send(L"AA000000");		// Get Data

	SetTimer(2,500,NULL);

	UpdateData(false);
}


//说明：本函数通过Create和Connect与服务器建立连接。由于在本机上测试，所以
//IP为127.0.0.1，实际应用时可以添加一个控件用于输入服务器的IP。
void CClientDlg::OnBnClickedConnect()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_connect)                             // 如果已经连接，则断开服务器
	{
		m_connect = false;
		pSock = NULL;
		//pSock->Close();
		delete pSock;
		m_ConPC.SetWindowTextW(_T("Connect"));
		m_bt_start.SetWindowTextW(_T("启动"));
		KillTimer(1);
		KillTimer(2);
		UpdateData(false);
		return;
	}
	else                                        // 未连接，则连接服务器
	{
		pSock = new CClientSocket();
		if (!pSock->Create())         //创建套接字
		{
			AfxMessageBox(_T("Fail to Create Socket"));
			return;
		}
		
	}

	if (!pSock->Connect( m_IP, m_port))    //连接服务器
	{
		AfxMessageBox(_T("Failt to Connect Server"));
		return;
	}
	else
	{
		m_connect = true;
		m_ConPC.SetWindowTextW(_T("Disconnect"));
		UpdateData(false);
		BeConnect();
	}
}

void CClientDlg::sock_send(CString str){

	str = str + L"E";
	str = str + L"F";

	char* pBuff = new char[str.GetLength()];
    memset(pBuff, 0, str.GetLength());

    WChar2MByte(str.GetBuffer(0), pBuff, str.GetLength());

    pSock->SendMSG(pBuff, str.GetLength());
	free(pBuff);
}

//这里的SendMSG函数与服务器端的不一致，函数实体在CClientSocket类中以实现。
//WChar2MByte字符转换函数与服务器端的一致，在这不再赘述。
void CClientDlg::OnBnClickedSend()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_connect)return;     //未连接服务器则不执行
	UpdateData(true);          //获取界面数据

	if (m_DataSend != ""){
		sock_send(m_DataSend);
	}
	UpdateData(false);   
}


//本函数在发送函数SendMSG中调用，用于字符集的转换，将宽字符转换为多字符集，
//不经转换的话，接收方只能接收一个字节。
BOOL CClientDlg::WChar2MByte(LPCWSTR srcBuff, LPSTR destBuff, int nlen)
{
	int n = 0;
	n = WideCharToMultiByte(CP_OEMCP, 0, srcBuff, -1, destBuff, 0, 0, FALSE);
	if (n<nlen)return FALSE;
	WideCharToMultiByte(CP_OEMCP, 0, srcBuff, -1, destBuff, nlen, 0, FALSE);
	return TRUE;
}


//该函数用于防止按下enter或者esc时退出程序。
BOOL CClientDlg::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->wParam)
	{
	//case VK_RETURN:
	case VK_ESCAPE:
		return true; 
		break;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

int bt_flag=0;
void CClientDlg::OnBnClickedMotorStart()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);           //获取界面数据
	
	if (!m_connect)
		return;					//未连接服务器则不执行
	if (m_speed_set == "")
		return;

	UpdateData(true);

	bt_flag = 1;
	m_bt_start.SetWindowTextW(_T("设置速度"));

	CString radio_flag;
	if(m_fore_t.GetCheck())
		radio_flag = L"1";
	else if(m_back_t.GetCheck())
		radio_flag = L"2";

	CString farme_str = L"B0";
	
	farme_str = farme_str + radio_flag + m_speed_set;
	sock_send(farme_str);

	UpdateData(false);   
}


void CClientDlg::OnBnClickedRunFore()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	m_fore_t.SetCheck (TRUE);
	m_back_t.SetCheck (FALSE);
	UpdateData(false);
}


void CClientDlg::OnBnClickedRunBack()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	m_fore_t.SetCheck (FALSE);
	m_back_t.SetCheck (TRUE);
	UpdateData(false);
}

void CClientDlg::OnBnClickedResetStep()
{
	if (!m_connect)return;     //未连接服务器则不执行
	UpdateData(true);          //获取界面数据

	sock_send(L"B10000000");
	//m_step_count = _T("0");
	UpdateData(false); 
}


void CClientDlg::OnBnClickedMotorStop()
{
	if (!m_connect)return;     //未连接服务器则不执行
	UpdateData(true);          //获取界面数据
	m_bt_start.SetWindowTextW(_T("启动"));

	sock_send(L"B20000000");
	UpdateData(false);   
}


void CClientDlg::OnBnClickedMotorSpeedUp()
{
	if (!m_connect)return;     //未连接服务器则不执行
	UpdateData(true);          //获取界面数据

	sock_send(L"B30000000");
	UpdateData(false); 
}


void CClientDlg::OnBnClickedMotorSpeedDown()
{
	if (!m_connect)return;     //未连接服务器则不执行
	UpdateData(true);          //获取界面数据

	sock_send(L"B40000000");
	UpdateData(false); 
}


void CClientDlg::OnBnClickedChangeDir()
{
	if (!m_connect)return;     //未连接服务器则不执行
	UpdateData(true);          //获取界面数据

	/*
	if(m_fore_t.GetCheck()){
		m_fore_t.SetCheck(false);
		m_back_t.SetCheck(true);
	}
	else if(m_back_t.GetCheck()){
		m_fore_t.SetCheck(true);
		m_back_t.SetCheck(false);
	}
	*/
	sock_send(L"B50000000");
	UpdateData(false); 
}


void CClientDlg::OnBnClickedUpdateBt()
{
	if (!m_connect)return;     //未连接服务器则不执行
	UpdateData(true);          //获取界面数据

	sock_send(L"A00000000");
	UpdateData(false); 
}


void CClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nIDEvent == 1){
		UpdataMotorData();
	}else if(nIDEvent == 2){
		BeConnect();
		SetTimer(1,250,NULL);
	}
	CDialogEx::OnTimer(nIDEvent);
}
