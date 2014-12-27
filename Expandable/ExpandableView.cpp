
// ExpandableView.cpp : CExpandableView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Expandable.h"
#endif

#include "ExpandableDoc.h"
#include "ExpandableView.h"

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
END_MESSAGE_MAP()

// CExpandableView 생성/소멸

double E_Global::resolutionWidth;
double E_Global::resolutionHeight;

CExpandableView::CExpandableView()
{
	//// Initialize
	// boolean
	desktopSwitcher_bool = false;

	// Screen 해상도값을 Global의 resolutionWidth, resolutionHeight에 Set
	E_Global::resolutionWidth = GetSystemMetrics(SM_CXSCREEN);
	E_Global::resolutionHeight = GetSystemMetrics(SM_CYSCREEN);
}

CExpandableView::~CExpandableView()
{
}

BOOL CExpandableView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CExpandableView 그리기

void CExpandableView::OnDraw(CDC* /*pDC*/)
{
	CExpandableDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
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


// CExpandableView 진단

#ifdef _DEBUG
void CExpandableView::AssertValid() const
{
	CView::AssertValid();
}

void CExpandableView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CExpandableDoc* CExpandableView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CExpandableDoc)));
	return (CExpandableDoc*)m_pDocument;
}
#endif //_DEBUG


// CExpandableView 메시지 처리기


void CExpandableView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 'D')
	{
		desktopSwitcher_bool = true;
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CExpandableView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 'D')
	{
		desktopSwitcher_bool = false;
		desktopSwitcher.drawWindowSwitcher();
	}

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}
