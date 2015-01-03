#include "stdafx.h"
#include "E_WindowSwitcher.h"


E_WindowSwitcher* E_WindowSwitcher::singleton = NULL;

E_WindowSwitcher::E_WindowSwitcher()
{

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
	this->ShowWindow(SW_SHOW);
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
