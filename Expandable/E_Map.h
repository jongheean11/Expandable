
#include "E_EnvironmentManager.h"
#include "E_ISwitcherUpdator.h"
#pragma once
#define WM_USER_EVENT (WM_USER + 2)
class E_Map : public CWnd, virtual E_ISwitcherUpdator
{
private:
	static E_Map* singleton;
	int visibleTime = 3000;

	E_Window* draggingWindow;
	clock_t sTime, nTime;
	CPoint iconClick;
	CPoint mouse;
	HWND selectIconHwnd;
	RECT foreRect;
	int iconMoveMode;
	bool forSelectMap;
	bool clicked;
	bool redraw;
	bool select;
	bool up;
	BOOL m_bTrack;
	bool leave;
	bool alreadyin;
	CRect getSize;
	POINT pt;
	bool checkdelete;
	DWORD parnetpid;

public:
	DWORD pidforhide;
	static BOOL CALLBACK EnumShow(HWND hwnd, LPARAM lParam);
	static BOOL CALLBACK EnumCallBackMap(HWND hwnd, LPARAM lParam);
	static BOOL CALLBACK EnumCallHide(HWND hwnd, LPARAM lParam);
	static BOOL CALLBACK special(HWND hwnd, LPARAM lParam);
	DWORD parentprocessId;
	long childmovx, childmovy;
	RECT rectForChildMove;
	CWnd* hwnd_cwnd_emap;
	bool ison2;
	bool ison;
	HWND hwnd;
	CWnd* getmapwindow();
	int time;
	int getdesktop(int indexx, int indexy);
	int clickindexx, clickindexy;
	int upindexx, upindexy;
	int movindexx, movindexy;
	virtual void updateSelectedDesktop();
	E_Map();
	~E_Map();
	std::list<RECT*> iconRectList;
	std::list<HWND> iconHwndList;
	const static COLORREF backgroundColor;
	static E_Map* getSingleton();
	static volatile bool isThreadRunning;
	void drawMap();
	void terminateMap();
	HWND maphwnd;
	UINT checkTime(LPVOID param);
	static const wchar_t* caption;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HRESULT OnUserEvent(WPARAM wParam, LPARAM lParam);
	afx_msg HRESULT OnInvali(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseLeave();
	//afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};