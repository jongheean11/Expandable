#include "stdafx.h"
#include "E_Global.h"
#pragma once
#include <list>
E_Global* E_Global::singleton = NULL;

E_Global::E_Global()
{
	int desktopCount;
	//���� ������ �о�� ��

	desktopCount = 4;
	//����ũž ����
	
	
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

//��� ����� ���� �ݹ�
void E_Global::OnDualMonitorMode(bool dualMonitorMode){
	
}


E_Global* E_Global::getSingleton()
{
	if (singleton == NULL)
		singleton = new E_Global();
	return singleton;
}


// īī���� �ڵ�
CWnd* E_Global::getKakaoWindow()
{
	HWND hkakao = FindWindow(NULL, L"īī����");
	
	return CWnd::FromHandle(hkakao);
}

//���� ������ �ڵ� ������
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

	if (IsWindowVisible(hwnd))
	{
		// Tool windows should not be displayed either, these do not appear in the
		// task bar.
		// ����ȭ��, �½�Ʈ��, ���۹�ư ����
		if (!(GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW))
			global->windowList.push_front(hwnd);
	}

	return TRUE;
}

// ���� ����ũž ����
bool E_Global::setSelectedDesktop()
{
	return false;
}


// ���� ����ũž ��ȯ
E_Desktop* E_Global::getSelectedDesktop()
{
	return NULL;
}


// �ֱ��� ������Ʈ�� ���� Ÿ�̸�
void E_Global::startTimer()
{
}


void E_Global::onTimer()
{
}


// ������Ʈ ����
bool E_Global::startUpdate()
{
	
	return false;
}


// ������Ʈ ������
void E_Global::onUpdate()
{
}


// ������Ʈ�� ���ߴ� �Լ� (������ �÷��׸� �ٲ���)
void E_Global::stopUpdate()
{
}


bool E_Global::getUpdateMode()
{
	return false;
}


// ����Ʈ�� ���� ����ũž �߰�
bool E_Global::addOneDesktop()
{
	return false;
}


// ����ũž ����
bool E_Global::delDesktop()
{
	return false;
}


// ����ũž ���� ��ȯ
int E_Global::getDesktopCount()
{
	return 0;
}


void E_Global::setExpandableRunningFlag()
{
}


bool E_Global::getExpandableRunningFlag()
{
	return false;
}
