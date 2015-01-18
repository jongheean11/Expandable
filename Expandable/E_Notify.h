#pragma once
#include "E_EnvironmentManager.h"
class E_Notify : public CWnd
{
private:
	//static E_Notify* singleton;
	int notifywidth;
	int notifyheight;
	int myPos;
	int pId;
	char pName[255];
	HWND pHwnd;
	bool click;
	bool m_bTrack;
	int colornum;
public:
	bool deleteAble();
	E_Notify();
	~E_Notify();
	//static E_Notify* getSingleton();
	void terminateNotify();
	int time;
	CWnd* hwnd_cwnd_notify;
	void showNotify(int processId, char* processName, HWND hwnd);
	static BOOL CALLBACK EnumCallHide(HWND hwnd, LPARAM lParam);
	void setPname(char* title);
	

	static const wchar_t* caption;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
};

