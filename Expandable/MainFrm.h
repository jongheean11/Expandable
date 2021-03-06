
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#include "E_WindowSwitcher.h"
#define WM_TRAY_EVENT (WM_USER + 3)
#pragma once
class CMainFrame : public CFrameWndEx
{

protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

	// 특성입니다.
public:
	list<thread*> thread_list;
	int keydown = 0;
	int isCheckRunning = false;
	thread* t = NULL;
	//int startAnimate = false;
	// 작업입니다.
	HWND hwnd;
	//윈도우 리스트
public:

	// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void DestroyTrayIcon();
	// 구현입니다.
public:

	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;

#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;

	// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg LRESULT OnTrayNotification(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

public:
	bool autoalready;
	bool alreadyrun;
	HWND autohwnd;
	afx_msg void On32775();
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg HRESULT OnTrayEvent(WPARAM wParam, LPARAM lParam);
	afx_msg HRESULT OnUserNotify(WPARAM wParam, LPARAM lParam);
	afx_msg HRESULT OnMapRight(WPARAM wParam, LPARAM lParam);
	afx_msg HRESULT OnChange(WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK EnumAuto(HWND hwnd, LPARAM lParam);
	DWORD GetProcessByFileName(char* name);
	bool icondisable;
	// alt up 체킹
	int startChecking();
	int stopChecking();
	//void restartTimer();
	int onChecking();
	void terminateAHK();

	afx_msg void On32777();
	void changetray(int num);
	afx_msg void On32778();
	afx_msg void On32779();
	afx_msg void On32781();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


