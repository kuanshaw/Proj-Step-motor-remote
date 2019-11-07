
// ClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"
#include <stdlib.h> 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CClientDlg �Ի���

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
	DDX_Text(pDX, IDC_PORT, m_port);		//�˿�
	//  DDX_Text(pDX, IDC_IP, m_IP);			//IP��ַ
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


// CClientDlg ��Ϣ�������

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_fore_t.SetCheck(true);
	m_back_t.SetCheck(false);

	GetDlgItem(IDC_DATARECEIVE)->ShowWindow(false);
	GetDlgItem(IDC_DATASEND)->ShowWindow(false);
	GetDlgItem(IDC_SEND)->ShowWindow(false);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�
void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CClientDlg::UpdataMotorData()
{
	if (!m_connect)return;     //δ���ӷ�������ִ��
	UpdateData(true);          //��ȡ��������

	sock_send(L"AB000000");		// Get Data

	UpdateData(false);
}

void CClientDlg::BeConnect()
{
	if (!m_connect)return;     //δ���ӷ�������ִ��
	UpdateData(true);          //��ȡ��������

	sock_send(L"AA000000");		// Get Data

	SetTimer(2,500,NULL);

	UpdateData(false);
}


//˵����������ͨ��Create��Connect��������������ӡ������ڱ����ϲ��ԣ�����
//IPΪ127.0.0.1��ʵ��Ӧ��ʱ�������һ���ؼ����������������IP��
void CClientDlg::OnBnClickedConnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_connect)                             // ����Ѿ����ӣ���Ͽ�������
	{
		m_connect = false;
		pSock = NULL;
		//pSock->Close();
		delete pSock;
		m_ConPC.SetWindowTextW(_T("Connect"));
		m_bt_start.SetWindowTextW(_T("����"));
		KillTimer(1);
		KillTimer(2);
		UpdateData(false);
		return;
	}
	else                                        // δ���ӣ������ӷ�����
	{
		pSock = new CClientSocket();
		if (!pSock->Create())         //�����׽���
		{
			AfxMessageBox(_T("Fail to Create Socket"));
			return;
		}
		
	}

	if (!pSock->Connect( m_IP, m_port))    //���ӷ�����
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

//�����SendMSG������������˵Ĳ�һ�£�����ʵ����CClientSocket������ʵ�֡�
//WChar2MByte�ַ�ת��������������˵�һ�£����ⲻ��׸����
void CClientDlg::OnBnClickedSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_connect)return;     //δ���ӷ�������ִ��
	UpdateData(true);          //��ȡ��������

	if (m_DataSend != ""){
		sock_send(m_DataSend);
	}
	UpdateData(false);   
}


//�������ڷ��ͺ���SendMSG�е��ã������ַ�����ת���������ַ�ת��Ϊ���ַ�����
//����ת���Ļ������շ�ֻ�ܽ���һ���ֽڡ�
BOOL CClientDlg::WChar2MByte(LPCWSTR srcBuff, LPSTR destBuff, int nlen)
{
	int n = 0;
	n = WideCharToMultiByte(CP_OEMCP, 0, srcBuff, -1, destBuff, 0, 0, FALSE);
	if (n<nlen)return FALSE;
	WideCharToMultiByte(CP_OEMCP, 0, srcBuff, -1, destBuff, nlen, 0, FALSE);
	return TRUE;
}


//�ú������ڷ�ֹ����enter����escʱ�˳�����
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);           //��ȡ��������
	
	if (!m_connect)
		return;					//δ���ӷ�������ִ��
	if (m_speed_set == "")
		return;

	UpdateData(true);

	bt_flag = 1;
	m_bt_start.SetWindowTextW(_T("�����ٶ�"));

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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	m_fore_t.SetCheck (TRUE);
	m_back_t.SetCheck (FALSE);
	UpdateData(false);
}


void CClientDlg::OnBnClickedRunBack()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	m_fore_t.SetCheck (FALSE);
	m_back_t.SetCheck (TRUE);
	UpdateData(false);
}

void CClientDlg::OnBnClickedResetStep()
{
	if (!m_connect)return;     //δ���ӷ�������ִ��
	UpdateData(true);          //��ȡ��������

	sock_send(L"B10000000");
	//m_step_count = _T("0");
	UpdateData(false); 
}


void CClientDlg::OnBnClickedMotorStop()
{
	if (!m_connect)return;     //δ���ӷ�������ִ��
	UpdateData(true);          //��ȡ��������
	m_bt_start.SetWindowTextW(_T("����"));

	sock_send(L"B20000000");
	UpdateData(false);   
}


void CClientDlg::OnBnClickedMotorSpeedUp()
{
	if (!m_connect)return;     //δ���ӷ�������ִ��
	UpdateData(true);          //��ȡ��������

	sock_send(L"B30000000");
	UpdateData(false); 
}


void CClientDlg::OnBnClickedMotorSpeedDown()
{
	if (!m_connect)return;     //δ���ӷ�������ִ��
	UpdateData(true);          //��ȡ��������

	sock_send(L"B40000000");
	UpdateData(false); 
}


void CClientDlg::OnBnClickedChangeDir()
{
	if (!m_connect)return;     //δ���ӷ�������ִ��
	UpdateData(true);          //��ȡ��������

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
	if (!m_connect)return;     //δ���ӷ�������ִ��
	UpdateData(true);          //��ȡ��������

	sock_send(L"A00000000");
	UpdateData(false); 
}


void CClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(nIDEvent == 1){
		UpdataMotorData();
	}else if(nIDEvent == 2){
		BeConnect();
		SetTimer(1,250,NULL);
	}
	CDialogEx::OnTimer(nIDEvent);
}
