


class E_Map
{
private:
	int visibleTime;
	E_Window* draggingWindow;
	clock_t sTime, nTime;
	
public:
	static volatile bool isThreadRunning;
	void drawMap();
	void onMouseLButtonDown();
	void onMouseMove();
	void OnMouseLButtonUp();
	void startTimer();
	void stopTimer();
	UINT checkTime(LPVOID param);
};