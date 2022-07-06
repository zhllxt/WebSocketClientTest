
// WebSocketClientTestDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "WebSocketClientTest.h"
#include "WebSocketClientTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CWebSocketClientTestDlg 对话框

CWebSocketClientTestDlg::CWebSocketClientTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_WEBSOCKETCLIENTTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWebSocketClientTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LOG, m_EditLog);
}

BEGIN_MESSAGE_MAP(CWebSocketClientTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CWebSocketClientTestDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CWebSocketClientTestDlg::OnBnClickedButtonStop)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CWebSocketClientTestDlg 消息处理程序

BOOL CWebSocketClientTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	this->InitUIMsgQueue(UM_UIMESSAGEQUEUE);

	m_Resizer.Add(this, IDC_EDIT_URL, RESIZE_LOCKLEFT | RESIZE_LOCKRIGHT);
	m_Resizer.Add(this, IDC_BUTTON_START, RESIZE_LOCKRIGHT);
	m_Resizer.Add(this, IDC_BUTTON_STOP, RESIZE_LOCKRIGHT);
	m_Resizer.Add(this, IDC_EDIT_LOG, RESIZE_LOCKALL);

	SetDlgItemText(IDC_EDIT_URL, _T("ws://127.0.0.1:9580/index"));

	SetWindowText(_T("WebSocket 客户端测试工具(2022-07-04) - 37792738@qq.com"));

	SetTimer(1, 10, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWebSocketClientTestDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWebSocketClientTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CWebSocketClientTestDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == UM_UIMESSAGEQUEUE)
	{
		this->ProcessUIEvent();
	}

	return __super::PreTranslateMessage(pMsg);
}

void CWebSocketClientTestDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	m_Resizer.Resize(this);
}

void CWebSocketClientTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// 定时执行自定义消息队列，防止程序运行时在标题栏点击右键弹出系统菜单后阻塞整个程序的消息循环导致界面得不到刷新
	if (nIDEvent == 1)
	{
		this->ProcessUIEvent();
	}
	__super::OnTimer(nIDEvent);
}

void CWebSocketClientTestDlg::OnDestroy()
{
	__super::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	this->KillTimer(1);

	if (this->m_ws_client)
		this->m_ws_client->stop();
}

void CWebSocketClientTestDlg::OnBnClickedButtonStart()
{
	if (!m_ws_client)
		m_ws_client = std::make_shared<asio2::ws_client>();

	CString strURL;
	GetDlgItemText(IDC_EDIT_URL, strURL);

	// ws://192.168.0.146:9580/index
	strURL.Trim();

	if (strURL.Left(5) != _T("ws://"))
	{
		MessageBox(_T("URL必须以 ws:// 开头"), _T("错误"), MB_OK | MB_ICONERROR);
		return;
	}

	strURL = strURL.Right(strURL.GetLength() - 5);

	if (strURL.Find(':') == -1)
	{
		MessageBox(_T("URL必须包含IP和端口，且IP和端口要用冒号分隔"), _T("错误"), MB_OK | MB_ICONERROR);
		return;
	}

	CString strHost, strPort, strTarget;

	strHost = strURL.Left(strURL.Find(':'));
	strURL  = strURL.Right(strURL.GetLength() - strURL.Find(':') - 1);

	if (strURL.Find('/') == -1)
	{
		strPort = strURL;
		strTarget = _T("/");
	}
	else
	{
		strPort = strURL.Left(strURL.Find('/'));
		strTarget = strURL.Right(strURL.GetLength() - strURL.Find('/'));
	}

	m_ws_client->bind_recv([this](std::string_view data) mutable
	{
		this->PostUIEvent([this, str = CString(CStringA(data.data(), int(data.size())))]() mutable
		{
			this->WriteLog(std::move(str));
		});
	}).bind_disconnect([this]() mutable
	{
		this->PostUIEvent([this]() mutable
		{
			GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
		});
	}).bind_connect([this]() mutable
	{
		this->PostUIEvent([this, ec = asio2::get_last_error()]() mutable
		{
			if (!ec)
			{
				GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
			}
			else
			{
				GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);

				this->WriteLog(CString(ec.message().data()));
			}
		});
	});

	m_ws_client->set_auto_reconnect(false);

	if (!m_ws_client->start(
		std::string((LPCSTR)CStringA(strHost)),
		std::string((LPCSTR)CStringA(strPort)),
		std::string((LPCSTR)CStringA(strTarget))))
	{
		MessageBox(_T("连接服务端失败，请重试"), _T("错误"), MB_OK | MB_ICONERROR);
		return;
	}
}

void CWebSocketClientTestDlg::OnBnClickedButtonStop()
{
	if (!m_ws_client)
		return;

	m_ws_client->stop();

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
}

void CWebSocketClientTestDlg::WriteLog(CString str)
{
	if (!this->m_EditLog.GetSafeHwnd() || !::IsWindow(this->m_EditLog.GetSafeHwnd()))
		return;

	str += "\r\n";

	int len = 0, nLineIndex = 0, nLineCount = 0, nLineLen = 0;

	len = this->m_EditLog.GetWindowTextLength();
	UINT dwLimit = this->m_EditLog.GetLimitText();
	while ((UINT)(len + (str.GetLength() * sizeof(TCHAR))) >= dwLimit)
	{
		nLineCount = this->m_EditLog.GetLineCount();

		nLineCount /= 3; // 删除前3分之一行的文字 

		len = 0;
		int line = 0;
		// 如果日志信息显示已经超过编辑框最多能显示的文字，就将删除前3分之一行的文字 
		while (line < nLineCount)
		{
			nLineIndex = this->m_EditLog.LineIndex(int(line));
			// The return value is the character index of the line specified in the wParam parameter, or it 
			// is –1 if the specified line number is greater than the number of lines in the edit control. 
			if (nLineIndex == -1)
				break;
			nLineLen = this->m_EditLog.LineLength(nLineIndex);
			len += nLineLen;
			len += int(std::strlen("\r\n"));

			line++;
		}
		if (len > 0)
		{
			this->m_EditLog.SetSel(0, len);
			this->m_EditLog.ReplaceSel(_T(""));
		}
		len = this->m_EditLog.GetWindowTextLength();
	}
	len = this->m_EditLog.GetWindowTextLength();
	this->m_EditLog.SetSel(len, len);
	this->m_EditLog.ReplaceSel(str);
}
