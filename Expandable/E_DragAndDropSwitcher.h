#include "E_ISwitcherUpdator.h"
#pragma once
class E_DragAndDropSwitcher : CWnd, E_ISwitcherUpdator
{
private:
	static E_DragAndDropSwitcher* singleton;
	int nextDesktopIndex;
	E_DragAndDropSwitcher();
	~E_DragAndDropSwitcher();
public:
	virtual void updateSelectedDesktop();
	static E_DragAndDropSwitcher* getSingleton();
	void startSwitcher();
	void terminateSwitcher();
	void OnResize();
	void OnRMouseDown(int nx, int ny);
	void OnRMouseMove(int nx, int ny);
	void RmouseUp(int nx, int ny);
	int getNextDesktop();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
};

