

// ExpandableView.h : CExpandableView Ŭ������ �������̽�
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
protected: // serialization������ ��������ϴ�.
	CExpandableView();
	DECLARE_DYNCREATE(CExpandableView)

// Ư���Դϴ�.
public:
	CExpandableDoc* GetDocument() const;
	
	bool desktopSwitcher_bool;


// �۾��Դϴ�.
public:
//	E_Global e_global;
	
	CTime initTime;
// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// �����Դϴ�.
public:
	virtual ~CExpandableView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
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

#ifndef _DEBUG  // ExpandableView.cpp�� ����� ����
inline CExpandableDoc* CExpandableView::GetDocument() const
   { return reinterpret_cast<CExpandableDoc*>(m_pDocument); }
#endif

