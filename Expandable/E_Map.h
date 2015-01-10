
#include "E_EnvironmentManager.h"

class E_Map : public CWnd
{
private:
	static E_Map* singleton;
	int visibleTime=3000;
	CWnd* hwnd_cwnd_emap;
	E_Window* draggingWindow;
	clock_t sTime, nTime;
	CPoint iconClick;
	HWND selectIconHwnd;
	RECT foreRect;
	int iconMoveMode;
	bool clicked;
	bool ison;
	bool redraw;
	int transparent;
public:
	E_Map();
	~E_Map();
	std::list<RECT*> iconRectList;
	std::list<HWND> iconHwndList;
	const static COLORREF backgroundColor;
	static E_Map* getSingleton();
	static volatile bool isThreadRunning ;
	void drawMap();
	void terminateMap();
	void onMouseLButtonDown(int x, int y);
	void onMouseMove();
	void OnMouseLButtonUp(int x, int y);
	void startTimer();
	void stopTimer();
	void setTransparent(int value);
	UINT checkTime(LPVOID param);
	static const wchar_t* caption;
	
	
	DECLARE_MESSAGE_MAP()
//	afx_msg void OnPaint();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};