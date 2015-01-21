

// ExpandableView.h : CExpandableView 클래스의 인터페이스
//

#include "E_Global.h"
#include "E_Map.h"

#include "E_DesktopSwitcher.h"
#include "E_AeroPeekController.h"
#include "E_WindowSwitcher.h"
#include "E_DragAndDropSwitcher.h"
#include "E_Notify.h"
#pragma once


class CExpandableView : public CView
{
protected: // serialization에서만 만들어집니다.
	CExpandableView();
	DECLARE_DYNCREATE(CExpandableView)

// 특성입니다.
public:
	CExpandableDoc* GetDocument() const;
	
	bool desktopSwitcher_bool;


// 작업입니다.
public:
//	E_Global e_global;
	
	CTime initTime;
// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현입니다.
public:
	virtual ~CExpandableView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
//	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
//	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // ExpandableView.cpp의 디버그 버전
inline CExpandableDoc* CExpandableView::GetDocument() const
   { return reinterpret_cast<CExpandableDoc*>(m_pDocument); }
#endif

