#include "E_Desktop.h"
#include "E_EnvironmentManager.h"

class E_DesktopSwitcher : public CWnd
{
private:
	//singleton variable
	static E_DesktopSwitcher* singleton;
	//private constructor
	E_DesktopSwitcher();
	~E_DesktopSwitcher();	//강제 해제를 막음

	E_Desktop* focusedDesktop;
	E_Window* clickedWindow;
	bool ison;
public:
	//get singleton function
	static E_DesktopSwitcher* E_DesktopSwitcher::getSingleton()
	{
		if (E_DesktopSwitcher::singleton == NULL)
			E_DesktopSwitcher::singleton = new E_DesktopSwitcher();
		return E_DesktopSwitcher::singleton;
	}
	void drawWindowSwitcher();
	void destroyWindowSwitcher();
	void drawPreview();
	void onMouseClick();
	void switchDesktop(int direction);
	void drawOverview(E_Desktop* targetDesktop, E_Window* targetWindow);
	void updateComponent(E_Desktop* targetDesktop, E_Window* targetWindow);

private:
	std::list<HTHUMBNAIL> handle_list;
};