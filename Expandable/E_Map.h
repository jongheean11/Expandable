


class E_Map
{
private:
	int visibleTime=3000;
	CWnd* hwnd_cwnd_emap;
	E_Window* draggingWindow;
	clock_t sTime, nTime;
	bool ison = false;
public:
	E_Map();
	~E_Map();
	static volatile bool isThreadRunning ;
	void drawMap();
	void onMouseLButtonDown();
	void onMouseMove();
	void OnMouseLButtonUp();
	void startTimer();
	void stopTimer();
	UINT checkTime(LPVOID param);
};