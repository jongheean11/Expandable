
// MainFrm.h : CMainFrame Ŭ������ �������̽�
//

#include "E_WindowSwitcher.h"
#define WM_TRAY_EVENT (WM_USER + 3)
#pragma once
class CMainFrame : public CFrameWndEx
{
	
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Ư���Դϴ�.
public:
	list<thread*> thread_list;
	int keydown = 0;
	int isCheckRunning = false;
	thread* t = NULL;
// �۾��Դϴ�.
public:
	 
// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void DestroyTrayIcon();
// �����Դϴ�.
public:
	
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;

// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI); 
	afx_msg LRESULT OnTrayNotification(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void On32775();
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg HRESULT OnTrayEvent(WPARAM wParam, LPARAM lParam);
	// alt up üŷ
	int startChecking();
	int stopChecking();
	int onChecking();
	
	afx_msg void On32777();
	void changetray(int num);
};


