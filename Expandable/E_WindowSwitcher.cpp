#include "stdafx.h"
#include "E_WindowSwitcher.h"


E_WindowSwitcher* E_WindowSwitcher::singleton = NULL;

E_WindowSwitcher::E_WindowSwitcher() : running(false)
{
	envManager = E_EnvironmentManager::getSingleton();
}


E_WindowSwitcher::~E_WindowSwitcher()
{
	
}


E_WindowSwitcher* E_WindowSwitcher::getSingleton()
{
	if (singleton == NULL)
		singleton = new E_WindowSwitcher();
	return singleton;
}


// UI를 보여주고 입력을 받는 창을 활성화 시킴
void E_WindowSwitcher::startSwitcher()
{
	running = true;
	static long resWidth = envManager->getWidth();
	static long resHeight = envManager->getHeight();
	if (E_AeroPeekController::getSingleton()->isAeroPeekMode()) {
		//aero peek size...
		
	}
	else {
		//icon size...
		
	}
	E_AeroPeekController* aeroManager = E_AeroPeekController::getSingleton();
	RECT r;
	r.top =0;
	r.left =0;
	r.right =100;
	r.bottom =100;
	HTHUMBNAIL thumbnail;
	CWnd* val = E_Global::getSingleton()->getBackgroundWindow();
	HWND a = val->GetSafeHwnd();
	aeroManager->registerAero(E_Global::getSingleton()->getBackgroundWindow()->GetSafeHwnd(), this->GetSafeHwnd(), r, thumbnail);
	this->ShowWindow(SW_SHOWMAXIMIZED);
}


// UI를 없에고 창을 가리는 함수
void E_WindowSwitcher::terminateSwitcher()
{
	running = false;
	this->ShowWindow(SW_HIDE);
}


// ZOrderTop
void E_WindowSwitcher::setZOrderTop()
{
}
BEGIN_MESSAGE_MAP(E_WindowSwitcher, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()


void E_WindowSwitcher::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	static long resWidth = envManager->getWidth();
	static long resHeight = envManager->getHeight();
	//항목의 개수에 따라 크기를 조절하여 가운데 정렬할 것
	static int top = resHeight / 4;
	static int left = resWidth / 4;
	static int width = resWidth / 2;
	static long height = resHeight / 2;
	this->SetWindowPos(NULL, left, top, width, height, SWP_NOZORDER | SWP_SHOWWINDOW);
	
	
	OutputDebugStringA("OnPaint()");
	// 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.
}


// 현재 Switcher가 동작중인가?
bool E_WindowSwitcher::isRunning()
{
	return running;
}


// 해상도에 대비한 아이콘의 크기를 반환 1/26.6배
int E_WindowSwitcher::getIconSize(int res_width)
{
	//1280 : 48 = 화면 : 아이콘 (1/26.6배)
	static const double ratio = 26.6;
	double doubleTemp = res_width / ratio;
	int intTemp = (int)doubleTemp;
	
	//반 올림
	if ((doubleTemp - (double)intTemp) > 0.5) {
		intTemp++;
	}
	
	return intTemp;
}


// 해상도에 비례한 에어로픽 박스 크기 1/8.95배
int E_WindowSwitcher::getAeroSize(int res_width)
{
	//1280 : 143 = 화면 : 에어로 (1/8.95)
	static const double ratio = 8.95;
	double doubleTemp = res_width / ratio;
	int intTemp = (int)doubleTemp;

	//반 올림
	if ((doubleTemp - (double)intTemp) > 0.5) {
		intTemp++;
	}
	
	return intTemp;
}


// 박스 개수와 박스 크기를 이용한 전체 Window너비 반환
int E_WindowSwitcher::getAeroWindowWidth(int maxBoxCount, int boxwidth)
{
	return 0;
}


// 박스 개수와 너비를 이용해 높이 산출
int E_WindowSwitcher::getAeroWindowHeight(int maxBoxCount, int boxwidth)
{
	return 0;
}
