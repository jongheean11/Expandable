#include "stdafx.h"
#include "E_Global.h"
#pragma once
#include <list>
E_Global* E_Global::singleton = NULL;

const wchar_t* E_Global::testFrameName = L"expandable";


E_Global::E_Global() : selectedDesktop(NULL), updateMode(false)
{
	int desktopCount;
	
	//설정 파일을 읽어온 후
	desktopCount = 4;
	
	//데스크탑 생성
	for (int i = 0; i < desktopCount; i++)
		desktopList.push_back(new E_Desktop(i+1));
	
	//초기 데스크탑 
	selectedDesktop = *(desktopList.begin());
	selectedIndex = 0;

	//윈도우 리스트 초기화 // 생성자 안이라 초기화 불가능
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

//듀얼 모니터 관련 콜백
void E_Global::OnDualMonitorMode(bool dualMonitorMode){
	
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

//현재 윈도우 핸들 가져옴
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
		// 바탕화면, 태스트바, 시작버튼 제거, expandable 테스트 윈도우 제거
		if (!(GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) && hwnd != myhwnd){
			HWND handle = GetParent(hwnd);
			if (handle == NULL)
			{ //최상위 윈도우만 포함
				// 정훈 수정. 중복되는 메모리 주소값 있을시 추가 안함 (CWND 및 HWND 중복 방지)
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

// 현재 데스크탑 반환
E_Desktop* E_Global::getSelectedDesktop()
{
	return selectedDesktop;
}


// 주기적 업데이트를 위한 타이머
void E_Global::startTimer()
{
}


void E_Global::onTimer()
{
}


// 업데이트 시작
bool E_Global::startUpdate()
{
	
	return false;
}


// 업데이트 쓰레드
void E_Global::onUpdate()
{
}


// 업데이트를 멈추는 함수 (스레드 플래그를 바꿔줌)
void E_Global::stopUpdate()
{
}


bool E_Global::getUpdateMode()
{
	return false;
}


// 리스트의 끝에 데스크탑 추가
bool E_Global::addOneDesktop()
{
	return false;
}


// 데스크탑 제거
bool E_Global::delDesktop()
{
	return false;
}


// 데스크탑 개수 반환
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


// 생성자에서 초기화 하지 못하는 것들을 초기화 하는 함수
void E_Global::init()
{
	list<HWND> wlist = getAllWindows();
	for (list<HWND>::iterator iter = wlist.begin(); iter != wlist.end(); iter++) {
		E_Window* window = new E_Window(*iter);
		selectedDesktop->insertWindow(window);
	}
}