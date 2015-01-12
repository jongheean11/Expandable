
#include "E_EnvironmentManager.h"
#include "E_ISwitcherUpdator.h"

class E_Map : public CWnd, virtual E_ISwitcherUpdator
{
private:
	static E_Map* singleton;
	int visibleTime=3000;
	CWnd* hwnd_cwnd_emap;
	E_Window* draggingWindow;
	clock_t sTime, nTime;
	CPoint iconClick;
	CPoint mouse;
	HWND selectIconHwnd;
	RECT foreRect;
	int iconMoveMode;
	bool forSelectMap;
	bool clicked;
	bool ison;
	bool redraw;
	int transparent;
	int time;
	int settingTimer;
	bool select;
	bool up;
public:
	int clickindexx,clickindexy;
	int upindexx,upindexy;
	virtual void updateSelectedDesktop();
	E_Map();
	~E_Map();
	std::list<RECT*> iconRectList;
	std::list<HWND> iconHwndList;
	const static COLORREF backgroundColor;
	static E_Map* getSingleton();
	static volatile bool isThreadRunning ;
	void drawMap();
	void terminateMap();
	void setTransparent(int value);
	UINT checkTime(LPVOID param);
	static const wchar_t* caption;
	void setTimer(int value);
	
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};