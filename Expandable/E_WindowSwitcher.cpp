#include "stdafx.h"
#include "E_WindowSwitcher.h"


E_WindowSwitcher* E_WindowSwitcher::singleton = NULL;

E_WindowSwitcher::E_WindowSwitcher()
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
	this->ShowWindow(SW_SHOWMAXIMIZED);
}


// UI를 없에고 창을 가리는 함수
void E_WindowSwitcher::terminateSwitcher()
{
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
	static long resHeight = envManager->getWidth();
	//항목의 개수에 따라 크기를 조절하여 가운데 정렬할 것
	static int top = resWidth / 4;
	static int left = resHeight / 4;
	static int width = resWidth / 4;
	static long height = resWidth / 4;
	this->SetWindowPos(NULL, top, left, width, height, SWP_NOZORDER | SWP_SHOWWINDOW);
	
	// 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.
}
