
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
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_TIMER()
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

void CExpandableView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CExpandableView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
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
	if (nChar == 'D')
	{
		desktopSwitcher_bool = true;
	}
	if (nChar == 'A')
	{
		
		
	}
	if (nChar == 'C'){
	/*	char dbgmsg[999];
		sprintf(dbgmsg,"mode? : %d\n",
			E_AeroPeekController::getSingleton()->isAeroPeekMode());
		OutputDebugStringA(dbgmsg);*/
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
	if (nChar == 'T'){
		E_WindowSwitcher::getSingleton()->drawIcon();
	}
	
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
		
		e_map.drawMap();
		e_map.startTimer();
	}
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CExpandableView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	e_map.stopTimer();
	CView::OnTimer(nIDEvent);
}
