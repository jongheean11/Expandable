#pragma once
#include "afxwin.h"
#include "E_EnvironmentManager.h"
class E_WindowSwitcher :
	public CWnd
{
private:
	static E_WindowSwitcher* singleton;
	E_WindowSwitcher();
	~E_WindowSwitcher();
	E_EnvironmentManager* envManager;
public:
	static E_WindowSwitcher* getSingleton();
	// UI를 보여주고 입력을 받는 창을 활성화 시킴
	void startSwitcher();
	// UI를 없에고 창을 가리는 함수
	void terminateSwitcher();
	// ZOrderTop
	void setZOrderTop();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
};

