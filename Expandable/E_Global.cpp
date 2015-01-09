#include "stdafx.h"
#include "E_Global.h"
#pragma once
#include <list>
E_Global* E_Global::singleton = NULL;

const wchar_t* E_Global::testFrameName = L"expandable";


E_Global::E_Global() : selectedDesktop(NULL), updateMode(false)
{
	int desktopCount;
	
	//���� ������ �о�� ��
	desktopCount = 4;
	
	//����ũž ����
	for (int i = 0; i < desktopCount; i++)
		desktopList.push_back(new E_Desktop(i+1));
	
	//�ʱ� ����ũž 
	selectedDesktop = *(desktopList.begin());
	selectedIndex = 0;

	//������ ����Ʈ �ʱ�ȭ // ������ ���̶� �ʱ�ȭ �Ұ���
	//
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
	HWND myhwnd = FindWindow(NULL, E_Global::testFrameName);
	E_Global *global = E_Global::getSingleton();
	
	WCHAR Cap[255];
	int length;
	::GetWindowText(hwnd, Cap, 254);
	length = ::GetWindowTextLength(hwnd);

	if (IsWindowVisible(hwnd) )
	{
		// Tool windows should not be displayed either, these do not appear in the
		// task bar.
		// ����ȭ��, �½�Ʈ��, ���۹�ư ����, expandable �׽�Ʈ ������ ����
		if (!(GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) && hwnd != myhwnd){
			HWND handle = GetParent(hwnd);
			if (handle == NULL)
			{ //�ֻ��� �����츸 ����
				// ���� ����. �ߺ��Ǵ� �޸� �ּҰ� ������ �߰� ���� (CWND �� HWND �ߺ� ����)
				bool already_exist = false;
				for (std::list<HWND>::iterator itr_inspect = global->windowList.begin(); itr_inspect != global->windowList.end(); itr_inspect++)
				{
					if (hwnd == *itr_inspect)
					{
						already_exist = true;
					}
				}

				if (!already_exist)
				{
					global->windowList.push_front(hwnd);
				}
			}
		}
	}
	
	return TRUE;
}

// ���� ����ũž ��ȯ
E_Desktop* E_Global::getSelectedDesktop()
{
	return selectedDesktop;
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
	return windowList.size();
}


void E_Global::setExpandableRunningFlag()
{
}


bool E_Global::getExpandableRunningFlag()
{
	return false;
}


int E_Global::getSelectedIndex()
{
	return selectedIndex;
}


void E_Global::setSelectedIndex(int index)
{
	this->selectedIndex = index;
	list<E_Desktop*>::iterator iter = desktopList.begin();
	for (int i = 0; i < selectedIndex; i++)
		iter++;
	selectedDesktop = *iter;
}


// �����ڿ��� �ʱ�ȭ ���� ���ϴ� �͵��� �ʱ�ȭ �ϴ� �Լ�
void E_Global::init()
{
	list<HWND> wlist = getAllWindows();
	for (list<HWND>::iterator iter = wlist.begin(); iter != wlist.end(); iter++) {
		E_Window* window = new E_Window(*iter);
		selectedDesktop->insertWindow(window);
	}
}