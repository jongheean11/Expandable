#include "stdafx.h"
#include "E_Global.h"
#pragma once
#include <list>
E_Global* E_Global::singleton = NULL;

E_Global::E_Global()
{
}

E_Global::~E_Global()
{

}

CWnd* E_Global::getBackgroundWindow(){
	HWND background = GetShellWindow();

	return CWnd::FromHandle(background);
}
CWnd* E_Global::getTaskbarWindow(){
	string mainWindowStr = "Shell_TrayWnd",
		exWindowStr = "ReBarWindow32";
	std::wstring l_temp = std::wstring(mainWindowStr.begin(), mainWindowStr.end());
	LPCWSTR l_str = l_temp.c_str();
	std::wstring t_temp = std::wstring(exWindowStr.begin(), exWindowStr.end());
	LPCWSTR t_str = t_temp.c_str();
	HRESULT hr = S_OK;
	// Register the thumbnail
	HTHUMBNAIL thumbnail = NULL;
	HWND taskbar = FindWindowW(l_str, NULL);

	return CWnd::FromHandle(taskbar);
}

/*
void E_Global::setresolutionWidth(int paramWidth)
{
	resolutionWidth = paramWidth;
}

double E_Global::getresolutionWidth()
{
	return resolutionWidth;
}

void E_Global::setresolutionHeight(int paramHeight)
{
	resolutionHeight = paramHeight;
}

double E_Global::getresolutionHeight()
{
	return resolutionHeight;
}

void E_Global:: setVirtualWidth(int paramWidth)
{
	virtualWidth = paramWidth;
}

double E_Global::getVirtualWidth()
{
	return virtualWidth;
}

void E_Global::setVirtualHeight(int paramHeight)
{
	virtualHeight = paramHeight;
}

double E_Global::getVirtualHeight()
{
	return virtualHeight;
}
*/

//듀얼 모니터 관련 콜백
void E_Global::OnDualMonitorMode(bool dualMonitorMode){
	
}

void E_Global::setSelectedDesktopIndex(int paramIndex)
{
	selectedDesktopIndex = paramIndex;
}

int E_Global::getSelectedDesktopIndex()
{
	return selectedDesktopIndex;
}

void E_Global::startUpdateSelectedDesktop()
{

}

void E_Global::stopUpdateSelectedDesktop()
{

}

E_Global* E_Global::getSingleton()
{
	if (singleton == NULL)
		singleton = new E_Global();
	return singleton;
}


// 카카오톡 핸들
CWnd* E_Global::getKakaoWindow()
{
	HWND hkakao = FindWindow(NULL, L"카카오톡");
	
	return CWnd::FromHandle(hkakao);
}

list<HWND> E_Global::getAllWindows()
{
	E_Global* object = E_Global::getSingleton();

	EnumWindows(E_Global::EnumCallBack, 0);

	return object->windowList;
}

BOOL CALLBACK  E_Global::EnumCallBack(HWND hwnd, LPARAM lParam)
{
	E_Global *global = E_Global::getSingleton();

	WCHAR Cap[255];
	int length;
	::GetWindowText(hwnd, Cap, 254);
	length = ::GetWindowTextLength(hwnd);

	if (IsWindowVisible(hwnd) && length > 0)
	{
		global->windowList.push_front(hwnd);
	}

	return TRUE;
}