
// WebSocketClientTestDlg.h: 头文件
//

#pragma once

#include <mutex>
#include <asio2/http/ws_client.hpp>
#include "DlgItemResizer.h"

class CUIMsgQueue
{
public:
	CUIMsgQueue()
	{
	}
	~CUIMsgQueue()
	{
	}

public:
	CUIMsgQueue& InitUIMsgQueue(UINT nUIThreadMsgID)
	{
		m_dwUIThreadID = ::GetCurrentThreadId();
		m_nUIThreadMsgID = nUIThreadMsgID;
		return (*this);
	}

	inline bool IsInUIThread()
	{
		return (::GetCurrentThreadId() == m_dwUIThreadID);
	}

	template<typename F>
	void PostUIEvent(F&& f)
	{
		ASSERT(m_dwUIThreadID != 0);
		ASSERT(m_nUIThreadMsgID != 0);
		// 将所有对界面的操作封装成一个个闭包(这里是lambda表达式)，将这些闭包存储到一个vector中，
		// 然后向UI线程投递一个自定义消息UI线程在接收到该自定义消息时，从vector中取出所有的元素
		// (这里即是一个个的操作界面的闭包)，然后调用该闭包函数
		std::lock_guard guard(m_UIMsgQueueLock);
		m_UIMsgQueue.emplace_back(std::forward<F>(f));
		// 向UI线程投递一个自定义消息
		::PostThreadMessage(m_dwUIThreadID, m_nUIThreadMsgID, 0, 0);
	}

	void ProcessUIEvent()
	{
		// 先构造一个临时的vector
		std::vector<std::function<void()>> UIMsgQueue;
		{
			// 进锁后，将成员变量vector中的所有元素转换到这个临时变量中，因为如果我们不转换，而是直接处理的话，
			// 因为是操作界面，所以可能导致阻塞，而假如此时有别的地方要调用QueueUIEvent函数来投递闭包时，
			// 该QueueUIEvent函数也会阻塞，这是不合理的
			std::lock_guard g(m_UIMsgQueueLock);
			UIMsgQueue = std::move(m_UIMsgQueue);
		}
		for (auto& f : UIMsgQueue)
		{
			f(); // 调用闭包函数
		}
	}

protected:
	std::mutex                         m_UIMsgQueueLock;
	std::vector<std::function<void()>> m_UIMsgQueue;
	DWORD                              m_dwUIThreadID = 0;
	UINT                               m_nUIThreadMsgID = 0;
};

#define UM_UIMESSAGEQUEUE (WM_USER + 101)

// CWebSocketClientTestDlg 对话框
class CWebSocketClientTestDlg : public CDialog, public CUIMsgQueue
{
// 构造
public:
	CWebSocketClientTestDlg(CWnd* pParent = nullptr);	// 标准构造函数

	std::shared_ptr<asio2::ws_client> m_ws_client;

	void WriteLog(CString strText);

// 对话框数据
	enum { IDD = IDD_WEBSOCKETCLIENTTEST_DIALOG };
	CEdit m_EditLog;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;
	CDlgItemResizer m_Resizer;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
};
