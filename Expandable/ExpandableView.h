#include "E_Global.h"
#include "E_DesktopSwitcher.h"

// ExpandableView.h : CExpandableView Ŭ������ �������̽�
//

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
	E_Global e_global;
	E_DesktopSwitcher desktopSwitcher;

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
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // ExpandableView.cpp�� ����� ����
inline CExpandableDoc* CExpandableView::GetDocument() const
   { return reinterpret_cast<CExpandableDoc*>(m_pDocument); }
#endif

