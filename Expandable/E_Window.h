#include "E_Util.h"

using namespace Gdiplus;
using namespace std;

#pragma once
class E_Window
{
private:
	HWND window;
	CBitmap icon;
	CBitmap screenshot;
	
	int zIndex;
	int windowState;
	int dockState;
	int alertState;

public:

	bool takeScreenshot();
	E_Window(HWND);
	~E_Window();

	static bool setIconInvisible(HWND hwnd);
	static bool setIconVisible(HWND hwnd);
	void setTransparent();
	void setOpaque();
	void setHide();
	void setShow();
	// 윈도우 세팅
	void setWindow(HWND window);
	HWND getWindow();

	void operator=(const E_Window &tmp);
	bool operator==(const E_Window &tmp);
	//최소/최대화 애니메이션 제거 및 활성화
	void SetMinimizeMaximizeAnimation(bool status);
	CBitmap* getScreenshot();

	CBitmap* getIcon();
	// 윈도우 Show 상태를 가져옴
	UINT getShowState();
	// 에어로가 가능한지 아는 함수.
	bool isAeroPossible();
};