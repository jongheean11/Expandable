#include "E_Desktop.h"
#include "E_EnvironmentManager.h"

class E_DesktopSwitcher
{
private:
	CWnd* hwnd_cwnd;
	//HWND hwnd;
	bool ison;
public:
	E_DesktopSwitcher();
	~E_DesktopSwitcher();
	void drawWindowSwitcher();
	VOID destroyWindowSwitcher();
	void drawPreview();
	void onMouseClick();
	void switchDesktop(int direction);
	void drawOverview(E_Desktop* targetDesktop, E_Window* targetWindow);
	void updateComponent(E_Desktop* targetDesktop, E_Window* targetWindow);
};