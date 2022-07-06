////////////////////////////////////////////////////////////////////////
// DlgItemResizer.cpp : implementation file
//	
// Written by Magnus Egelberg (magnus.egelberg@lundalogik.se)
// Much of this code comes from Eli Vingot (elivingt@internet-zahav.net)
// 
//

#include "pch.h"
#include "DlgItemResizer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



CDlgItemResizer::CDlgItemResizer()
{
	m_szInitial = CSize(0,0);
	m_szMin = CSize(0,0);
}


CDlgItemResizer::~CDlgItemResizer()
{
	// Make sure to delete all allocated items
	for (INT i = 0; i < m_Controls.GetSize(); i++)
		delete ((DLGITEMINFO *)m_Controls[i]);
}


void CDlgItemResizer::Add(CWnd *pCtrl, UINT uFlags)
{
	// Make sure params are valid
	ASSERT(pCtrl != 0);
	ASSERT(pCtrl->GetParent() != 0);

	CWnd *pParent = pCtrl->GetParent();

	if (uFlags == 0)
		return; 

	CRect rc;
	pCtrl->GetWindowRect(rc);
	pParent->ScreenToClient(rc);

	DLGITEMINFO *item    = new DLGITEMINFO;
	item->m_uFlags       = uFlags;
	item->m_hWnd         = pCtrl->m_hWnd;
	item->m_rectPosition = rc;

	// Set inital size if not set
	if (m_szInitial.cx == 0 || m_szInitial.cy == 0) {
		CRect rc;
		pParent->GetClientRect(rc);
		m_szInitial = rc.Size();
	}

	// Add it to the array
	m_Controls.Add(item);
}

void CDlgItemResizer::Resize(CWnd *pWnd)
{
	// Just return if no initial size yet
	if (m_szInitial.cx == 0 || m_szInitial.cy == 0)
		return;

	// Don't bother to resize minimized windows
	if (GetWindowLong(pWnd->m_hWnd, GWL_STYLE) & WS_MINIMIZE)
		return;

	CRect client;
	pWnd->GetClientRect(client);

	for (INT i = 0; i < m_Controls.GetSize(); i++)
	{
		DLGITEMINFO *item = (DLGITEMINFO *)m_Controls[i];

		if( !IsWindow( item->m_hWnd ) ) continue;

		CWnd *ctrl = CWnd::FromHandle(item->m_hWnd);


		// Invalidate the old position
		CRect rect;
		ctrl->GetWindowRect(rect);  
		pWnd->ScreenToClient(rect);
		pWnd->InvalidateRect(rect);

		// Get the current size of the control
		CSize size = rect.Size();

		// Set the new position according to the flags specified
		if( m_szMin.cx < client.Width() )
		{
			if ((item->m_uFlags & (RESIZE_LOCKLEFT|RESIZE_LOCKRIGHT)) == (RESIZE_LOCKLEFT|RESIZE_LOCKRIGHT)) {
				rect.left = item->m_rectPosition.left;
				rect.right = client.right - (m_szInitial.cx - item->m_rectPosition.right);
			}
			else if (item->m_uFlags & RESIZE_LOCKRIGHT) {
				rect.right = client.right - (m_szInitial.cx - item->m_rectPosition.right);
				rect.left  = rect.right - size.cx;
			}

			else if (item->m_uFlags & RESIZE_LOCKLEFT) {
				rect.left  = item->m_rectPosition.left;
				rect.right = rect.left + size.cx;
			}
		}

		if( m_szMin.cy < client.Height() )
		{
			if ((item->m_uFlags & (RESIZE_LOCKTOP|RESIZE_LOCKBOTTOM)) == (RESIZE_LOCKTOP|RESIZE_LOCKBOTTOM)) {
				rect.top = item->m_rectPosition.top;
				rect.bottom = client.bottom - (m_szInitial.cy - item->m_rectPosition.bottom);
			}
			else if (item->m_uFlags & RESIZE_LOCKTOP) {
				rect.top = item->m_rectPosition.top;
				rect.bottom = rect.top + size.cy;
			}
			else if (item->m_uFlags & RESIZE_LOCKBOTTOM) {
				rect.bottom = client.bottom - (m_szInitial.cy - item->m_rectPosition.bottom);
				rect.top = rect.bottom - size.cy;
			}
		}


		// Check if control is completely inside client, hide if not
		// Do this only when the RESIZE_SHOWHIDE flag is set.
		if (item->m_uFlags & RESIZE_SHOWHIDE) {
			CRect unionrect;
			unionrect.UnionRect(rect, client);

			if (unionrect != client)
				ctrl->ShowWindow(SW_HIDE);
			else {

				// Make sure it is visible
				if (!(ctrl->GetStyle() & WS_VISIBLE))
					ctrl->ShowWindow(SW_SHOWNORMAL);

				ctrl->MoveWindow(rect);
			}
		}
		else
			ctrl->MoveWindow(rect);

	}
}

