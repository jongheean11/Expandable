#pragma once
class E_DragAndDropSwitcher : CWnd
{
private:
	static E_DragAndDropSwitcher* singleton;
	int nextDesktopIndex;
public:
	E_DragAndDropSwitcher();
	~E_DragAndDropSwitcher();
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

