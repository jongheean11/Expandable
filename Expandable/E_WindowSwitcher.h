#pragma once
#include "afxwin.h"
#include "E_EnvironmentManager.h"
#include "E_AeroPeekController.h"
class E_WindowSwitcher :
	public CWnd
{
private:
	static E_WindowSwitcher* singleton;
	E_WindowSwitcher();
	~E_WindowSwitcher();
	E_EnvironmentManager* envManager;
	bool running;
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
	// 현재 Switcher가 동작중인가?
public:
	bool isRunning();
	// 해상도에 대비한 아이콘의 크기를 반환
	int getIconSize(int res_width);
	// 해상도에 비례한 에어로픽 박스 크기
	int getAeroSize(int res_width);
	// 박스 개수와 박스 크기를 이용한 전체 Window너비 반환
	int getAeroWindowWidth(int maxBoxCount, int boxwidth);
	// 박스 개수와 너비를 이용해 높이 산출
	int getAeroWindowHeight(int maxBoxCount, int boxwidth);
};

