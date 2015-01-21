#include "E_Desktop.h"
#include "E_EnvironmentManager.h"
#include "E_ISwitcherUpdator.h"
#include <hash_map>
using namespace stdext;

class E_DesktopSwitcher : public CWnd, virtual  E_ISwitcherUpdator
{
private:
	//singleton variable
	static E_DesktopSwitcher* singleton;
	//private constructor
	E_DesktopSwitcher();
	~E_DesktopSwitcher();	//강제 해제를 막음

	E_Desktop* focusedDesktop;
	E_Window* clickedWindow;
	
	// property for doubleclick
	SYSTEMTIME doubleClickTimer, __doubleClickTimer;
	bool doubleclick_first, doubleclick_second;

	// property for arrow button
	bool leftarrow_pressed, rightarrow_pressed;

	// property for moving desktop
	bool desktop_selected;

public:
	list<CWnd*> desktop_CWnd_list;
	CWnd* mainCWnd;
	bool window_squeezed_inlist;
	LONG window_squeezed_left, window_squeezed_top;

	CRect *leftarrow, *rightarrow;
	
	bool ison;

	int target_desktop_index, desktoplist_startindex;

private:
	// property for moving window
	bool window_selected, window_squeezed, desktop_inrange;
	E_Window* window_ptr;
	CPoint window_leftdown_point;
	RECT *window_RECT, *window_RECT_copy, *window_RECT_from_desktop;
	HTHUMBNAIL window_hthumbnail, window_hthumbnail_from_desktop;
	DWM_THUMBNAIL_PROPERTIES winThumbProps;
	double main_desktop_size;

public:
	double main_desktop_width, main_desktop_height;

public:
	virtual void updateSelectedDesktop();
	//get singleton function
	static E_DesktopSwitcher* E_DesktopSwitcher::getSingleton()
	{
		if (E_DesktopSwitcher::singleton == NULL)
			E_DesktopSwitcher::singleton = new E_DesktopSwitcher();
		return E_DesktopSwitcher::singleton;
	}

	void startSwitcher();
	void terminateSwitcher();
	void switchDesktop(E_Desktop* selection);
	double getMainDesktopSize();
	void setMainDesktopSize(double size);
	void keyArrowPress(int direction);

	bool leftkey_pressed, rightkey_pressed, enterkey_pressed, esckey_pressed;
	int initindex;

	//double swappoint_h;
	std::list<RECT*> desktop_area_list_rect; // Desktop 영역
	std::list<RECT*> window_area_list_rect; // 메인 Desktop 내의 window들

	std::hash_map<RECT*, HTHUMBNAIL> window_RECT_hthumbnail_map;
	std::hash_map<RECT*, E_Window*> window_area_map_RECT_EWindow;

	std::hash_map<RECT*, HTHUMBNAIL> desktop_RECT_hthumbnail_map;

	std::hash_map<int,hash_map<RECT*, HTHUMBNAIL>> window_desktop_RECT_hthumbnail_map;
	std::hash_map<int, list<RECT*>> window_desktop_rect_map;

public:
	double ratio_wh, ratio_hw,
		initial_padding_width, initial_padding_height,
		padding_width, padding_height,
		switch_width, switch_height,
		ratio_ww, ratio_hh,
		background_left, background_right, background_top, background_bottom,
		taskbar_top;
	RECT *main_backgroundRECT, *main_taskbarRECT;
	HTHUMBNAIL main_backgroundHTHUMBNAIL, main_taskbarHTHUMBNAIL;
	HWND hShellWnd, hTaskbarWnd;
	CRect sizeRect_background, sizeRect_taskbar;
	
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};