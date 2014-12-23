#include <afxwin.h>
#include "E_Desktop.h"

class E_DesktopSwitcher
{
private:
	
public:
	E_DesktopSwitcher();
	~E_DesktopSwitcher();
	void drawWindowSwitcher();
	void drawPreview();
	void onMouseClick();
	void switchDesktop(int direction);
	void drawOverview(E_Desktop* targetDesktop, E_Window* targetWindow);
	void updateComponent(E_Desktop* targetDesktop, E_Window* targetWindow);
};