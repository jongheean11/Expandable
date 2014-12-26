#include "E_Desktop.h"
#include "E_Window.h"
class E_Map
{
private:
	int visibleTime;
	E_Window* draggingWindow;
public:
	void drawMap();
	void onMouseLButtonDown();
	void onMouseMove();
	void OnMouseLButtonUp();
	void startTimer();
	void stopTimer();
};