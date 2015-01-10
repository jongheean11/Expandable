
#include "E_EnvironmentManager.h"

class E_Map : public CWnd
{
private:
	static E_Map* singleton;
	int visibleTime=3000;
	CWnd* hwnd_cwnd_emap;
	E_Window* draggingWindow;
	clock_t sTime, nTime;
	bool ison;
	int transparent;
public:
	E_Map();
	~E_Map();
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
};