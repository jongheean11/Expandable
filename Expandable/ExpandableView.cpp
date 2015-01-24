
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
#include "InjDll.h"
#include "E_WindowTest.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define WM_USER_NOTIFY (WM_USER + 4)

// CExpandableView

IMPLEMENT_DYNCREATE(CExpandableView, CView)

BEGIN_MESSAGE_MAP(CExpandableView, CView)
	ON_WM_CONTEXTMENU()
//	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_TIMER()
	ON_WM_CREATE()
//	ON_WM_SYSKEYDOWN()
//	ON_WM_SYSKEYUP()
ON_WM_SYSCOMMAND()
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
	if (nChar == 'Z')
	{
		//E_Notify noti2;
		//E_Notify* e_noti = E_Notify::getSingleton();
		E_Notify* e_noti = new E_Notify();
		HWND hwnd = this->m_hWnd;
		char *name = "expandable.exe";
		e_noti->showNotify(1,name,hwnd);
	}
	if (nChar == 'I')
	{
		Injector(L"explorer.exe", INJECTION_MODE, L"ExpandableDLL_x86.dll");
	}
	if (nChar == 'E')
	{
		Injector(L"explorer.exe", EJECTION_MODE, L"ExpandableDLL_x86.dll");
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
	if (nIDEvent == 3)
	{
		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		POINT point;
		GetCursorPos(&point);
		
		if (!(GetAsyncKeyState(VK_RBUTTON) && 0x8000) && !(GetAsyncKeyState(VK_LBUTTON) && 0x8000))
		{
			if (!(E_DesktopSwitcher::getSingleton()->ison || E_WindowSwitcher::getSingleton()->isRunning()))
			{
				if ((point.x < 5) && (((enManager->getHeight() * 0.3) < point.y) && (point.y < (enManager->getHeight() * 0.7))))
				{
					if (E_Global::getSingleton()->getDesktopWidth() != 1)
					{
						KillTimer(3);
						E_DesktopSwitcher::getSingleton()->restore = false;
						//SetTimer(4, 30, NULL);
						E_DragAndDropSwitcher::getSingleton()->cursor_left = true;
						E_DragAndDropSwitcher::getSingleton()->initSwitcher();
						SetTimer(30, 32, NULL);
					}
				}

				else if ((point.x >(enManager->getWidth() - 5)) && (((enManager->getHeight() * 0.3) < point.y) && (point.y < (enManager->getHeight() * 0.7))))
				{
					if (E_Global::getSingleton()->getDesktopWidth() != 1)
					{
						KillTimer(3);
						//SetTimer(5, 30, NULL);
						E_DesktopSwitcher::getSingleton()->restore = false;
						E_DragAndDropSwitcher::getSingleton()->cursor_right = true;
						E_DragAndDropSwitcher::getSingleton()->initSwitcher();
						SetTimer(30, 32, NULL);
					}
				}
				else if ((((enManager->getWidth() * 0.3) < point.x) && (point.x < (enManager->getWidth() * 0.7))) && (point.y < 5))
				{
					if (E_Global::getSingleton()->getDesktopHeight() != 1)
					{
						KillTimer(3);
						//SetTimer(6, 30, NULL);
						E_DesktopSwitcher::getSingleton()->restore = false;
						E_DragAndDropSwitcher::getSingleton()->cursor_top = true;
						E_DragAndDropSwitcher::getSingleton()->initSwitcher();
						SetTimer(30, 32, NULL);
					}
				}
				else if ((((enManager->getWidth() * 0.3) < point.x) && (point.x < (enManager->getWidth() * 0.7))) && (point.y >(enManager->getHeight() - 5)))
				{
					if (E_Global::getSingleton()->getDesktopHeight() != 1)
					{
						KillTimer(3);
						//SetTimer(7, 30, NULL);
						E_DesktopSwitcher::getSingleton()->restore = false;
						E_DragAndDropSwitcher::getSingleton()->cursor_bottom = true;
						E_DragAndDropSwitcher::getSingleton()->initSwitcher();
						SetTimer(30, 32, NULL);
					}
				}
			}
			CRect mapCRect(enManager->getWidth() - 30, enManager->getHeight() - 30, enManager->getWidth(), enManager->getHeight());
			if (mapCRect.PtInRect(point))
			{
				E_Map* e_map = E_Map::getSingleton();
				if(!e_map->ison)
					e_map->drawMap();
			} 
		}
	}else if (nIDEvent == 30)
	{
		E_DragAndDropSwitcher* drSwitcher = E_DragAndDropSwitcher::getSingleton();
		if (!drSwitcher->ison)
		{
			KillTimer(30);
			SetTimer(3, 32, NULL);
			E_DesktopSwitcher::getSingleton()->restore = true;
		
		}
	}
	else if (nIDEvent == E_Global::DLLINJECTIONTIMER)
	{
		E_EnvironmentManager* envm = E_EnvironmentManager::getSingleton();
		TRACE_WIN32A("TRY INJECT");
		if (!envm->is64bitsWindows()){
			if (!E_Util::isContainDLL(L"explorer.exe", L"ExpandableDLL_x86.dll"))
				Injector(L"explorer.exe", INJECTION_MODE, L"ExpandableDLL_x86.dll");
			if (!E_Util::isContainDLL(L"chrome.exe", L"ExpandableDLL_x86.dll"))
				Injector(L"chrome.exe", INJECTION_MODE, L"ExpandableDLL_x86.dll");
			if (!E_Util::isContainDLL(L"iexplorer.exe", L"ExpandableDLL_x86.dll"))
				Injector(L"iexplorer.exe", INJECTION_MODE, L"ExpandableDLL_x86.dll");
		}
		else{
			if (!E_Util::isContainDLL(L"chrome.exe", L"ExpandableDLL_x86.dll"))
				Injector(L"chrome.exe", INJECTION_MODE, L"ExpandableDLL_x86.dll");
			if (!E_Util::isContainDLL(L"iexplorer.exe", L"ExpandableDLL_x86.dll"))
				Injector(L"iexplorer.exe", INJECTION_MODE, L"ExpandableDLL_x86.dll");
		}
	}

	CView::OnTimer(nIDEvent);
}

int CExpandableView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	//SetCapture();
	SetTimer(3, 32, NULL);
	
	SetTimer(E_Global::DLLINJECTIONTIMER, 60000, NULL);
	//SetTimer(2, 5000, NULL);

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.

	return 0;
}


//void CExpandableView::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
//
//	TRACE_WIN32A("OnSysKeyDown");
//	CView::OnSysKeyDown(nChar, nRepCnt, nFlags);
//}


//void CExpandableView::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
//	TRACE_WIN32A("OnSysKeyUp");
//	CView::OnSysKeyUp(nChar, nRepCnt, nFlags);
//}


void CExpandableView::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	TRACE_WIN32A("CExpandableView::OnSysCommand");
	CView::OnSysCommand(nID, lParam);
}
