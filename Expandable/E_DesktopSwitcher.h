#include "E_Desktop.h"
#include "E_EnvironmentManager.h"
//#include <hash_map>
//using namespace stdext;

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
	// property for moving window
	bool window_selected;
	CPoint window_leftdown_point;
	RECT *window_RECT, *window_RECT_copy;
	int desktop_i; // target window (desktop을 window로 바꿔야됨)
	HTHUMBNAIL window_hthumbnail;
	DWM_THUMBNAIL_PROPERTIES winThumbProps;
	double main_desktop_size;
public:
	//get singleton function
	static E_DesktopSwitcher* E_DesktopSwitcher::getSingleton()
	{
		if (E_DesktopSwitcher::singleton == NULL)
			E_DesktopSwitcher::singleton = new E_DesktopSwitcher();
		return E_DesktopSwitcher::singleton;
	}

//	void drawWindowSwitcher();
//	void destroyWindowSwitcher();
//	void drawPreview();
//	void onMouseClick();
//	void switchDesktop(int direction);
//	void drawOverview(E_Desktop* targetDesktop, E_Window* targetWindow);
//	void updateComponent(E_Desktop* targetDesktop, E_Window* targetWindow);
	
	
	void startSwitcher();
	void terminateSwitcher();
	void setZOrderTop();
	void queryPositionAndProcess(int nx, int ny);
	void switchDesktop(E_Desktop* selection);
	double getMainDesktopSize();
	void setMainDesktopSize(double size);
	//
//	bool moveWindow(E_Window* target, E_Desktop* dest);
//	bool moveWindow(E_Window* target, E_Desktop* dest);
//private:
	double swappoint_h;
	std::list<HTHUMBNAIL> desktop_hthumbnail_list, window_hthumbnail_list;
	std::list<RECT*> desktop_area_list_rect;
	std::list<HWND> desktop_area_list_hwnd;
	std::list<RECT*> window_area_list_rect;
	std::list<HWND> window_area_list_hwnd;
	//hash_map<CRect, HWND> desktop_area_list;
	//hash_map<CRect, HWND> window_area_list;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	
};