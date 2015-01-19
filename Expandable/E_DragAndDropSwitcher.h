#include "E_ISwitcherUpdator.h"
#include "E_Desktop.h"
#include <hash_map>
using namespace stdext;
using namespace std;

#pragma once
class E_DragAndDropSwitcher : public CWnd, virtual E_ISwitcherUpdator
{
private:
	static E_DragAndDropSwitcher* singleton;
	E_DragAndDropSwitcher();
	~E_DragAndDropSwitcher();

public:
	//E_Window *currentTaskbar, *switchTaskbar;
	//CWnd currentCWnd, switchCWnd;
	//CBitmap *currentBitmap, *switchBitmap;
	bool ison, started, switchable;

	bool cursor_left, cursor_right, cursor_top, cursor_bottom;
	LONG main_left, main_right, main_top, main_bottom;
	POINT prev_point;
	CRect movingCRect;
	HWND hShellWnd, hTaskbarWnd;
	CRect sizeRect_background, sizeRect_taskbar;
	
	RECT *currentDesktopRECT, *currentTaskbarRECT, *switchDesktopRECT, *switchTaskbarRECT;
	HTHUMBNAIL currentDesktopThumbnail, currentTaskbarThumbnail, switchDesktopThumbnail, switchTaskbarThumbnail;
	DWM_THUMBNAIL_PROPERTIES winThumbProps;

	hash_map<RECT*, HTHUMBNAIL> current_RECT_HTHUMBNAIL_map, switch_RECT_HTHUMBNAIL_map;
	
	E_Desktop* switchDesktop;
	int switchIndex;

	const static COLORREF backgroundColor;

	virtual void updateSelectedDesktop();
	static E_DragAndDropSwitcher* getSingleton();
	void initSwitcher();
	void startSwitcher();
	void terminateSwitcher();
	void OnResize();
	void turnUpdateOn();
	void turnUpdateOff();
	void animateSwitcher();
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
};

