#pragma once
#include "E_AeroPeekController.h"
#include "E_Util.h"
using namespace Gdiplus;
using namespace std;

class E_Window
{
private:
	HWND window;
	CBitmap icon;
	CBitmap screenshot;
	char windowName[255];

	bool tpmode;	//투명 모드인가?
	WINDOWPLACEMENT savedRect;	//투명 모드때 원래의 위치 저장
public:
	unsigned int savedState;	//saveShowState로 저장한 상태

	bool dock;
	E_Window(HWND);
	~E_Window();
	bool takeScreenshot();
	//최소/최대화 애니메이션 제거 및 활성화
	static void SetMinimizeMaximizeAnimation(bool status);

	static bool setIconInvisible(HWND hwnd);
	static bool setIconVisible(HWND hwnd);
	void setTransparent();
	void setOpaque();
	void setHide();
	void setShow();
	void setNormal();
	void setNormal(RECT& rect);
	void setMinimize();
	// 윈도우 세팅
	void setWindow(HWND window);
	HWND getWindow();
	char* getWindowName();

	void operator=(const E_Window &tmp);
	bool operator==(const E_Window &tmp);
	CBitmap* getScreenshot();

	CBitmap* getIcon();
	// 윈도우 Show 상태를 가져옴
	UINT getShowState();
	// 에어로가 가능한지 아는 함수.
	bool isAeroPossible();
	bool getTPMode();
	// 윈도우 쇼 상태
	void saveShowState();
	// 저장된 상태 복구
	void restoreShowState();
	void setRestore();
	// 상태 검사시 사용 (일반적으로 최대화 여부 검사)
	bool isMaximized();

	void doTake(int state);
	void saveRect(WINDOWPLACEMENT& rect);
	void loadRect(WINDOWPLACEMENT& rect);
};