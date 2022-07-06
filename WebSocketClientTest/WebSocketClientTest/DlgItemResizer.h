#if !defined(AFX_DLGITEMRESIZER_H__C1E23966_0DD5_11D3_8745_00105A48D360__INCLUDED_)
#define AFX_DLGITEMRESIZER_H__C1E23966_0DD5_11D3_8745_00105A48D360__INCLUDED_
//DlgItemResizer.h
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const UINT RESIZE_LOCKLEFT        = 0x0001; // Distance to left is fixed
const UINT RESIZE_LOCKRIGHT       = 0x0002; // Distance to right is fixed
const UINT RESIZE_LOCKTOP         = 0x0004; // Distance to top is fixed
const UINT RESIZE_LOCKBOTTOM      = 0x0008; // Distance to bottom is fixed
const UINT RESIZE_SHOWHIDE        = 0x0010; // Show/hide if not fully visible

const UINT RESIZE_LOCKALL         = RESIZE_LOCKLEFT|RESIZE_LOCKRIGHT|RESIZE_LOCKTOP|RESIZE_LOCKBOTTOM;
const UINT RESIZE_LOCKTOPLEFT     = RESIZE_LOCKLEFT|RESIZE_LOCKTOP;
const UINT RESIZE_LOCKBOTTOMRIGHT = RESIZE_LOCKBOTTOM|RESIZE_LOCKRIGHT;

class CDlgItemResizer {

public:

	CDlgItemResizer();
	virtual ~CDlgItemResizer();

	// Adds a control to the resize list
	void Add(CWnd *pControl, UINT uFlags);

	void Add(CWnd *pWnd, INT nCtrlID, UINT uFlags)
	{ Add(pWnd->GetDlgItem(nCtrlID), uFlags); }

	// Resizes the controls in the form
	//void Resize(CFormView *pWnd);

	// Resizes the controls in the window
	void Resize(CWnd *pWnd);

	// Sets the original size of the parent
	void SetInitialSize(INT cx, INT cy)
	{ m_szInitial = CSize(cx, cy); }

	// Sets the original size of the parent
	void SetInitialSize(const CSize &size)
	{ m_szInitial = size; }

	// Sets the minimize size of the control
	void SetMinSize(INT cx, INT cy)
	{ m_szMin = CSize(cx, cy); }

	// Sets the minimize size of the control
	void SetMinSize(const CSize &size)
	{ m_szMin = size; }

	// Returns the initial size
	CSize GetInitialSize() const
	{ return m_szInitial; }

	// Returns the minimize size
	CSize GetMinSize() const
	{ return m_szMin; }

protected:
	CPtrArray m_Controls;
	CSize m_szInitial;
	CSize m_szMin;

	//
	// This struct is used to hold information
	// about the added controls
	//
	struct DLGITEMINFO {
		UINT m_uFlags;
		HWND m_hWnd;
		CRect m_rectPosition;
	};
};

#endif 
