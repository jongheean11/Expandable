
// ExpandableView.cpp : CExpandableView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "Expandable.h"
#endif

#include "ExpandableDoc.h"
#include "ExpandableView.h"

#include "E_WindowTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CExpandableView

IMPLEMENT_DYNCREATE(CExpandableView, CView)

BEGIN_MESSAGE_MAP(CExpandableView, CView)
	ON_WM_CONTEXTMENU()
//	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
END_MESSAGE_MAP()

// CExpandableView ����/�Ҹ�


CExpandableView::CExpandableView()
{
	//// Initialize
	// boolean
	desktopSwitcher_bool = false;

	// Screen �ػ󵵰��� Global�� resolutionWidth, resolutionHeight�� Set

}

CExpandableView::~CExpandableView()
{
}

BOOL CExpandableView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.
	
	return CView::PreCreateWindow(cs);
}

// CExpandableView �׸���

void CExpandableView::OnDraw(CDC* /*pDC*/)
{
	CExpandableDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}

//void CExpandableView::OnRButtonUp(UINT /* nFlags */, CPoint point)
//{
//	ClientToScreen(&point);
//	OnContextMenu(this, point);
//}

void CExpandableView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif

}


// CExpandableView ����

#ifdef _DEBUG
void CExpandableView::AssertValid() const
{
	CView::AssertValid();
}

void CExpandableView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CExpandableDoc* CExpandableView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CExpandableDoc)));
	return (CExpandableDoc*)m_pDocument;
}
#endif //_DEBUG


// CExpandableView �޽��� ó����


void CExpandableView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	TRACE_WIN32A("OnKeyDown");
	if (nChar == 'D')
	{
		desktopSwitcher_bool = true;
	}
	if (nChar == 'A')
	{
		
		
	}
	if (nChar == 'C'){
//		char dbgmsg[999];
//		sprintf(dbgmsg,"mode? : %d\n",
//			E_AeroPeekController::getSingleton()->isAeroPeekMode());
//		OutputDebugStringA(dbgmsg);
		if (E_WindowSwitcher::getSingleton()->isRunning() == false)
			E_WindowSwitcher::getSingleton()->startSwitcher();
		else
			E_WindowSwitcher::getSingleton()->terminateSwitcher();
	}
	if (nChar == 'L'){
		E_Global::getSingleton()->getSelectedDesktop()->setAllHide();
		E_Global* global = E_Global::getSingleton();
		int index = global->getSelectedIndex();
		global->setSelectedIndex(index ++);
	}
	if (nChar == 'J'){
		E_Global::getSingleton()->getSelectedDesktop()->setAllShow();
	}
	if (nChar == 'H'){
		E_WindowTest::testTakeScreenshot();
	}
	if (nChar == 'K'){
		E_DragAndDropSwitcher::getSingleton()->startSwitcher();
	}
	//if (nChar == 'Q'){
	//	//������ �׽�Ʈ
	//	E_Global::getSingleton()->startUpdate();
	//}
	//if (nChar == 'W'){

	//	E_Global::getSingleton()->stopUpdate();
	//}
	//if (nChar == 'E'){
	//	E_Desktop* d = E_Global::getSingleton()->getSelectedDesktop();
	//	d->getWindowList();
	//}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CExpandableView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 'D')
	{
		desktopSwitcher_bool = false;
		E_DesktopSwitcher::getSingleton()->startSwitcher();
	}
	if (nChar == 'A')
	{
		E_Map* e_map = E_Map::getSingleton();
		e_map->drawMap();
	}
	TRACE_WIN32A("OnKeyUp");

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


bool checking = false;
void CExpandableView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	//e_map.stopTimer();
	
	/*
	if (nIDEvent == 1)
	{
		return;
	}
	else if (nIDEvent == 2)
	{
		return;
	}*/
	if (nIDEvent == 3)
	{
		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		POINT point;
		GetCursorPos(&point);

		if (!(GetAsyncKeyState(VK_RBUTTON) && 0x8000) && !(GetAsyncKeyState(VK_LBUTTON) && 0x8000))
		{
			if ((point.x < 5) && (((enManager->getHeight() * 0.3) < point.y) && (point.y < (enManager->getHeight() * 0.7))))
			{
				KillTimer(3);
				SetTimer(4, 50, NULL);
				ReleaseCapture();
			}
			
			else if ((point.x >(enManager->getWidth() - 5)) && (((enManager->getHeight() * 0.3) < point.y) && (point.y < (enManager->getHeight() * 0.7))))
			{
				KillTimer(3);
				SetTimer(5, 50, NULL);
			}
			else if ((((enManager->getWidth() * 0.3) < point.x) && (point.x < (enManager->getWidth() * 0.7))) && (point.y < 5))
			{
				KillTimer(3);
				SetTimer(6, 50, NULL);
			}
			else if ((((enManager->getWidth() * 0.3) < point.x) && (point.x < (enManager->getWidth() * 0.7))) && (point.y >(enManager->getHeight() - 5)))
			{
				KillTimer(3);
				SetTimer(7, 50, NULL);
			}
			else
			{
				CRect mapCRect = new CRect(enManager->getWidth() - 50, enManager->getHeight() - 50, enManager->getWidth(), enManager->getHeight());
				if (mapCRect.PtInRect(point))
				{
					E_Map* e_map = E_Map::getSingleton();
					if(!e_map->ison)
						e_map->drawMap();
				}
			}
		}
	}

	else if (nIDEvent == 4) // left
	{
		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		POINT point;
		GetCursorPos(&point);

		if ((point.x < 5) && (((enManager->getHeight() * 0.3) < point.y) && (point.y < (enManager->getHeight() * 0.7))))
		{
			if ((GetAsyncKeyState(VK_RBUTTON) && 0x8000) && !(GetAsyncKeyState(VK_LBUTTON) && 0x8000))
			{
				E_DragAndDropSwitcher* drSwitcher = E_DragAndDropSwitcher::getSingleton();
				if (!((drSwitcher->cursor_left) || (drSwitcher->cursor_right) || (drSwitcher->cursor_top) || (drSwitcher->cursor_bottom)))
				{
					KillTimer(4);
					drSwitcher->cursor_left = true;
					drSwitcher->startSwitcher();
					SetTimer(3, 80, NULL);
				}
			}
		}
		else
		{
			KillTimer(4);
			SetTimer(3, 80, NULL);
		}
	}

	else if (nIDEvent == 5) // right
	{
		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		POINT point;
		GetCursorPos(&point);

		if ((point.x >(enManager->getWidth() - 5)) && (((enManager->getHeight() * 0.3) < point.y) && (point.y < (enManager->getHeight() * 0.7))))
		{
			if ((GetAsyncKeyState(VK_RBUTTON) && 0x8000) && !(GetAsyncKeyState(VK_LBUTTON) && 0x8000))
			{
				E_DragAndDropSwitcher* drSwitcher = E_DragAndDropSwitcher::getSingleton();
				if (!((drSwitcher->cursor_left) || (drSwitcher->cursor_right) || (drSwitcher->cursor_top) || (drSwitcher->cursor_bottom)))
				{
					KillTimer(5);
					drSwitcher->cursor_right = true;
					drSwitcher->startSwitcher();
					SetTimer(3, 80, NULL);
				}
			}
		}
		else
		{
			KillTimer(5);
			SetTimer(3, 80, NULL);
		}
	}

	else if (nIDEvent == 6) // top
	{
		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		POINT point;
		GetCursorPos(&point);

		if ((((enManager->getWidth() * 0.3) < point.x) && (point.x < (enManager->getWidth() * 0.7))) && (point.y < 5))
		{
			if ((GetAsyncKeyState(VK_RBUTTON) && 0x8000) && !(GetAsyncKeyState(VK_LBUTTON) && 0x8000))
			{
				E_DragAndDropSwitcher* drSwitcher = E_DragAndDropSwitcher::getSingleton();
				if (!((drSwitcher->cursor_left) || (drSwitcher->cursor_right) || (drSwitcher->cursor_top) || (drSwitcher->cursor_bottom)))
				{
					KillTimer(6);
					drSwitcher->cursor_top = true;
					drSwitcher->startSwitcher();
					SetTimer(3, 80, NULL);
				}
			}
		}
		else
		{
			KillTimer(6);
			SetTimer(3, 80, NULL);
		}
	}

	else if (nIDEvent == 7) // bottom
	{
		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		POINT point;
		GetCursorPos(&point);

		if ((((enManager->getWidth() * 0.3) < point.x) && (point.x < (enManager->getWidth() * 0.7))) && (point.y >(enManager->getHeight() - 5)))
		{
			if ((GetAsyncKeyState(VK_RBUTTON) && 0x8000) && !(GetAsyncKeyState(VK_LBUTTON) && 0x8000))
			{
				E_DragAndDropSwitcher* drSwitcher = E_DragAndDropSwitcher::getSingleton();
				if (!((drSwitcher->cursor_left) || (drSwitcher->cursor_right) || (drSwitcher->cursor_top) || (drSwitcher->cursor_bottom)))
				{
					KillTimer(7);
					drSwitcher->cursor_bottom = true;
					drSwitcher->startSwitcher();
					SetTimer(3, 80, NULL);
				}
			}
		}
		else
		{
			KillTimer(7);
			SetTimer(3, 80, NULL);
		}
	}

	CView::OnTimer(nIDEvent);
}

int CExpandableView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	//SetCapture();
	SetTimer(3, 80, NULL);
	//SetTimer(2, 5000, NULL);

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.

	return 0;
}


void CExpandableView::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	TRACE_WIN32A("OnSysKeyDown");
	CView::OnSysKeyDown(nChar, nRepCnt, nFlags);
}


void CExpandableView::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	TRACE_WIN32A("OnSysKeyUp");
	CView::OnSysKeyUp(nChar, nRepCnt, nFlags);
}
