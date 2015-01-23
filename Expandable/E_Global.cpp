#include "stdafx.h"
#include "E_Global.h"
#pragma once
#include <list>
#include "E_Map.h"
#include "InjDll.h"

E_Global* E_Global::singleton = NULL;

const wchar_t* E_Global::testFrameName = L"E_expandable";
wchar_t* const E_Global::excludeWindows[] = { L"Spy++" };
#define WM_TRAY_EVENT (WM_USER + 3)
#define WM_CHANGEDISPLAY (WM_USER + 7)
const int E_Global::DLLINJECTIONTIMER = 50;

E_Global::E_Global() : selectedDesktop(NULL), updateMode(false), currentThread(NULL)
{
	//Setting init
	SettingInfo info;
	E_SettingManager* smanager = E_SettingManager::getSingleton();
	
	//
	dockcount = 0;
	nowActiveNotify = -1;
	nextActiveNotify = 0;
	mapopen = false;
	hotkeyinvalidate = false;

	//Setting Load
	if (smanager->loadSetting(info) == false){
		settingTimer = 5;
		transparent = 160;
		mapsize = 0.06;
		iconsize = 1;
		desktopwidth = 3;
		desktopheight = 3;
	}
	else {
		settingTimer = info.settingTimer;
		transparent = info.transparent;
		mapsize = info.mapsize;
		iconsize = info.iconsize;
		desktopwidth = info.desktopwidth;
		desktopheight = info.desktopheight;
	}

	//설정 파일을 읽어온 후
	desktopCount = desktopwidth*desktopheight;
	notifyAblePos[0] = notifyAblePos[1] = notifyAblePos[2] = notifyAblePos[3] = notifyAblePos[4] = 0;
	//데스크탑 생성
	for (int i = 0; i < desktopCount; i++)
		desktopList.push_back(new E_Desktop(i));

	//초기 데스크탑 
	selectedDesktop = *(desktopList.begin());
	selectedIndex = 0;
	
	E_EnvironmentManager *envm = E_EnvironmentManager::getSingleton();

	if (envm->is64bitsWindows() == FALSE){
		if(!E_Util::isContainDLL(L"explorer.exe", L"ExpandableDLL_x86.dll"))
			Injector(L"explorer.exe", INJECTION_MODE, L"ExpandableDLL_x86.dll");
		if (!E_Util::isContainDLL(L"chrome.exe", L"ExpandableDLL_x86.dll"))
			Injector(L"chrome.exe", INJECTION_MODE, L"ExpandableDLL_x86.dll");
		if (!E_Util::isContainDLL(L"iexplorer.exe", L"ExpandableDLL_x86.dll"))
			Injector(L"iexplorer.exe", INJECTION_MODE, L"ExpandableDLL_x86.dll");
	}
	else{
		if (!E_Util::isContainDLL(L"chrome.exe", L"ExpandableDLL_x86.dll"))
			Injector(L"chrome.exe", INJECTION_MODE, L"ExpandableDLL_x86.dll");
		if (!E_Util::isContainDLL(L"iexplorer.exe", L"ExpandableDLL_x86.dll"))
			Injector(L"iexplorer.exe", INJECTION_MODE, L"ExpandableDLL_x86.dll");
		
	}
	//::SetWindowPos(this->hwnd_frame, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}
void E_Global::setTimer(int value)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	settingTimer = value;
}
int E_Global::getTimer()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return settingTimer;
}

E_Global::~E_Global()
{
}
void E_Global::setTransparent(int value)
{
	transparent = value;
}
int  E_Global::getTransparent()
{
	return transparent;
}
double E_Global::getMapsize()
{
	return mapsize;
}
double E_Global::getIconsize()
{
	return iconsize;
}
void E_Global::setIconsize(double value)
{
	iconsize = value / 10;
}
void E_Global::setMapsize(double value)
{
	mapsize = value / 100;
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

//듀얼 모니터 관련 콜백
void E_Global::OnDualMonitorMode(bool dualMonitorMode){
	SendMessage(hwnd_frame, WM_CHANGEDISPLAY, 0, 0);
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
	object->windowListForUpdate.clear();	//초기화
	
	EnumWindows(E_Global::EnumCallBack, 0);

	return object->windowListForUpdate;
}

BOOL CALLBACK  E_Global::EnumCallBack(HWND hwnd, LPARAM lParam)
{
	HWND myhwnd = FindWindow(NULL, E_Global::testFrameName);
	E_Global *global = E_Global::getSingleton();
	
	WCHAR Cap[255];
	WCHAR expandable[100];
	WCHAR expan[] = TEXT("E_expandable");
	WCHAR expan1[] = TEXT("E_DragAndDropSwitcher");
	WCHAR expan2[] = TEXT("E_DesktopSwitcher");
	WCHAR expan3[] = TEXT("E_WindowSwitcher");

	::GetWindowText(hwnd, expandable, 100);
	if (wcscmp(expan1, expandable) == 0 || wcscmp(expan2, expandable) == 0 || wcscmp(expan3, expandable) == 0)
		return TRUE;
	int length;
	::GetWindowText(hwnd, Cap, 254);
	length = ::GetWindowTextLength(hwnd);

	if (IsWindowVisible(hwnd) && !(wcscmp(expandable, expan) == 0) )
	{
		// Tool windows should not be displayed either, these do not appear in the
		// task bar.
		// 바탕화면, 태스트바, 시작버튼 제거, expandable 테스트 윈도우 제거, spy ++ 제거
		if (!(GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) && hwnd != myhwnd){
			wchar_t* cap = NULL;
			int arrSize = sizeof(excludeWindows) / sizeof(char*);
			for (int i = 0; i < arrSize; i++){
				cap = wcsstr(Cap, excludeWindows[i]);
				if (cap != NULL){
					break;
				}
			}
			if (global->hwnd_desk.find(hwnd) != global->hwnd_desk.end()){
				std::hash_map<HWND, int>::iterator iter = global->hwnd_desk.find(hwnd);
				if (iter->second != global->selectedIndex) {
					return TRUE;
				}
			}
			if (cap == NULL){
				HWND handle = GetParent(hwnd);
				if (handle == NULL)
				{ //최상위 윈도우만 포함
					// 정훈 수정. 중복되는 메모리 주소값 있을시 추가 안함 (CWND 및 HWND 중복 방지)
					bool already_exist = false;
					for (std::list<HWND>::iterator itr_inspect = global->windowListForUpdate.begin(); itr_inspect != global->windowListForUpdate.end(); itr_inspect++)
					{
						if (hwnd == *itr_inspect)
						{
							already_exist = true;
						}
					}

					if (!already_exist)
					{
						global->windowListForUpdate.push_front(hwnd);
					}
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
	updateMode = true;
}


void E_Global::onTimer()
{
}

// 업데이트 시작
bool E_Global::startUpdate()
{
	TRACE_WIN32A("E_Global::startUpdate() before");
	lock_guard<mutex> start(E_Mutex::updateStartStopMutex);
	if (currentThread == NULL && updateMode != true){
		TRACE_WIN32A("E_Global::startUpdate()");
		updateMode = true;

		thread* t = new thread{ &E_Global::loopUpdate, this};
		thread_map.insert(std::unordered_map<std::thread::id, bool>::value_type(t->get_id(), false));	//플래그 관리
		thread_inner_map.insert(std::unordered_map<std::thread::id, bool>::value_type(t->get_id(), false));	//플래그 관리
		thread_pmap.insert(std::unordered_map<std::thread::id, thread*>::value_type(t->get_id(), t));	//포인터 관리
		currentThread = t;

		TRACE_WIN32A("E_Global::startUpdate() after");
		return true;
	}
	TRACE_WIN32A("E_Global::startUpdate() after");
	return false;
}

// 실시간으로 윈도우 업데이트
void E_Global::loopUpdate()
{
	bool update = false;
	//맵이 유효할 때
	if (thread_map.find(std::this_thread::get_id()) != thread_map.end())
		bool update = thread_map.find(std::this_thread::get_id())->second;
	
	while (update == false){
		if (onUpdate()){
			//TRACE_WIN32A("E_Global::loopUpdate()");
			windowSwitcher->updateSelectedDesktop();
			desktopSwitcher->updateSelectedDesktop();
			mapSwitcher->updateSelectedDesktop();
			dragSwitcher->updateSelectedDesktop();
		}
		Sleep(200);
		update = thread_map.find(std::this_thread::get_id())->second;//플래그 업데이트
	}
	if (update == true){
		thread_inner_map.find(std::this_thread::get_id())->second = true;
	}
}

// 업데이트 쓰레드// 변화될 경우 참을 반환한다.
bool E_Global::onUpdate()
{
	bool result = false;
	//윈도우 리스트 업데이트
	//TRACE_WIN32A("E_Global::getAllWindows before");
	list<HWND> wlist = getAllWindows();	//UI 쓰레드에서 호출되는 콜백이라 데드락 발생하기 쉬움
	//TRACE_WIN32A("E_Global::getAllWindows after");
	list<E_Window*> selectedWindows = selectedDesktop->getWindowList();
	//윈도우 추가
	int wlistSize = wlist.size();
	int selectedSize = selectedWindows.size();

	//TRACE_WIN32A("E_Global::onUpdate ING...()");
	//사이즈가 다르거나 마지막이 다르다면.. 업데이트 수행
	if (wlist.size() != selectedWindows.size() || ((wlistSize != 0 && selectedSize != 0) && ((*wlist.rbegin()) != (*selectedWindows.rbegin())->getWindow()))){
		//바뀐 윈도우만 업데이트
		list<E_Window*> noChangeList;
		/*for (list<E_Window*>::iterator iter_window = selectedWindows.begin(); iter_window != selectedWindows.end(); iter_window++){
			HWND findWindow = 0;
			for (list<HWND>::iterator iter = wlist.begin(); iter != wlist.end(); iter++){
			if (*iter == (*iter_window)->getWindow()){
			noChangeList.push_back(*iter_window);
			selectedDesktop->excludeWindow(*iter_window);
			findWindow = *iter;
			}
			}
			if (findWindow == NULL){
			E_Window* window = new E_Window(findWindow);
			noChangeList.push_back(window);
			}
			}*/
		//리스트 업데이트
		for (list<HWND>::iterator iter = wlist.begin(); iter != wlist.end(); iter++){
			HWND findWindow = NULL;
			for (list<E_Window*>::iterator iter_window = selectedWindows.begin(); iter_window != selectedWindows.end(); iter_window++){
				if (*iter == (*iter_window)->getWindow()){
					noChangeList.push_back(*iter_window);
					selectedDesktop->excludeWindow(*iter_window);
					findWindow = *iter;
				}
			}
			if (findWindow == NULL){
				E_Window* window = new E_Window(*iter);
				noChangeList.push_back(window);
			}
		}
		//TRACE_WIN32A("E_Global::updateMutex before");
		lock_guard<mutex> lockGuard(E_Mutex::updateMutex);
		//TRACE_WIN32A("E_Global::updateMutex after");
		//기존 윈도우를 제외한 윈도우 제거
		selectedDesktop->clearWindow();
		//실제 리스트에 업데이트
		for (list<E_Window*>::iterator iter = noChangeList.begin(); iter != noChangeList.end(); iter++){
			selectedDesktop->insertWindow(*iter);
		}
		//업데이트
		result = true;
	}
	else{

	}
	//TRACE_WIN32A("E_Global::onUpdateEnd()");
	return result;
}


// 업데이트를 멈추는 함수 (스레드 플래그를 바꿔줌)
bool E_Global::stopUpdate()
{
	//동기화
	lock_guard<mutex> stop(E_Mutex::updateStartStopMutex);

	list<std::thread::id> colleting;
	//스레드 컬렉팅 (할당 해제)
	for (std::unordered_map<std::thread::id, bool>::iterator iter = thread_inner_map.begin(); iter != thread_inner_map.end(); iter++){
		if (iter->second == true){
			//중지 플래그 검사
			std::unordered_map<std::thread::id, thread*>::iterator target;
			if ((target = thread_pmap.find(iter->first)) != thread_pmap.end()){
				target->second->detach();
				//target->second->
				delete target->second;
				//스레드 할당 해제
				colleting.push_back(iter->first);
			}
		}
	}
	//스레드 컬렉팅 (맵에서 제거)
	for (list<std::thread::id>::iterator iter = colleting.begin(); iter != colleting.end(); iter++){
		thread_map.erase(*iter);
		thread_pmap.erase(*iter);
		thread_inner_map.erase(*iter);
	}

	TRACE_WIN32A("E_Global::stopUpdate() before");
	if (currentThread != NULL && updateMode == true){
		TRACE_WIN32A("E_Global::stopUpdate()");

		thread_map.find(currentThread->get_id())->second = true;		//중지 플래그로 변경 //스레드 대기를 없에고 컬렉팅으로 해결

		//delete currentThread;
		//업데이트 가능
		currentThread = NULL;
		updateMode = false;
		return true;
	}
	TRACE_WIN32A("E_Global::stopUpdate() after");
	return false;
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
	return desktopList.size();
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
void E_Global::init(E_ISwitcherUpdator* desktop, E_ISwitcherUpdator* map, E_ISwitcherUpdator* drag, E_ISwitcherUpdator* window)
{
	window->updateSelectedDesktop();
	desktop->updateSelectedDesktop();
	map->updateSelectedDesktop();
	drag->updateSelectedDesktop();
	this->windowSwitcher = window;
	this->dragSwitcher = drag;
	this->mapSwitcher = map;
	this->desktopSwitcher = desktop;

	//배경 윈도우
	backgroundWindow = new E_Window(getBackgroundWindow()->GetSafeHwnd());

	onUpdate();
}

int E_Global::getDesktopWidth()
{
	return desktopwidth;
}

int E_Global::getDesktopHeight()
{
	return desktopheight;
}


// 데스크탑을 가져온다. 옳지않은 index 일 경우 NULL 가능 
E_Desktop* E_Global::getDesktop(int index)
{
	E_Desktop* result = NULL;
	for (list<E_Desktop*>::iterator iter = desktopList.begin(); iter != desktopList.end(); iter++){
		if ((*iter)->getIndex() == index){
			result = (*iter);
			break;
		}
	}

	return result;
}


// 탑 윈도우를 왼쪽 데스크탑으로 전달 (TEST OK)
void E_Global::moveTopWindowLeft(){
	onUpdate();
	list<E_Window*> windowList = selectedDesktop->getWindowList();
	list<E_Window*>::reverse_iterator iter = windowList.rbegin();
	if (iter != windowList.rend()){
		E_Window* targetWindow = *iter;
		twForHide = targetWindow;
		if (desktopCount == 1)
			return;

		if ((selectedIndex) % desktopwidth == 0) {
			//nothing
			////가장 오른쪽 데스크탑
			//E_Desktop* desktop = getDesktop(selectedIndex + 1);
			//desktop->insertWindow(targetWindow);	//윈도우 추가
			//selectedDesktop->removeWindow(targetWindow);	//윈도우 삭제
		}
		//else if ((selectedIndex % desktopwidth) == 0){
		//	//가장 왼쪽 데스크탑
		//	E_Desktop* desktop = getDesktop(selectedIndex + 1);
		//	desktop->insertWindow(targetWindow);	//윈도우 추가
		//	selectedDesktop->removeWindow(targetWindow);	//윈도우 삭제
		//}
		else{
			//안쪽 데스크탑
			E_Desktop* desktop = getDesktop(selectedIndex - 1);
			desktop->insertWindow(targetWindow);	//윈도우 추가
			//twForHide = targetWindow;
			targetWindow->setHide();
			selectedDesktop->excludeWindow(targetWindow);	//윈도우 삭제

			//
			E_Global* e_global = E_Global::getSingleton();
			if (e_global->mapopen && hwnd_cwnd->m_hWnd != NULL)
			{
				hotkeyinvalidate = true;
				::SendMessage(hwnd_cwnd->m_hWnd, WM_USER_EVENT, 0, 0);
				//E_Map* e_map = E_Map::getSingleton(); e_map->leave2 = false;
			}
			//
			if (!(e_global->mapopen))
			{
				E_Map* e_map = E_Map::getSingleton();
				e_map->drawMap();
			}
			EnumWindows(E_Global::EnumHide, 0);
		}
	}
}

// 탑 윈도우를 오른쪽 데스크탑으로 전달 (TEST OK)
void E_Global::moveTopWindowRight(){
	onUpdate();
	list<E_Window*> windowList = selectedDesktop->getWindowList();
	list<E_Window*>::reverse_iterator iter = windowList.rbegin();
	if (iter != windowList.rend()){
		E_Window* targetWindow = *iter;
		twForHide = targetWindow;
		TRACE_WIN32A("[E_Global::moveTopWindowRight] 윈도우 이름: %s", targetWindow->getWindowName());

		if (desktopCount == 1)
			return;

		if (((selectedIndex + 1) % desktopwidth) == 0) {
			//nothing
			////가장 오른쪽 데스크탑
			//E_Desktop* desktop = getDesktop(selectedIndex + 1);
			//desktop->insertWindow(targetWindow);	//윈도우 추가
			//selectedDesktop->removeWindow(targetWindow);	//윈도우 삭제
		}
		//else if ((selectedIndex % desktopwidth) == 0){
		//	//가장 왼쪽 데스크탑
		//	E_Desktop* desktop = getDesktop(selectedIndex + 1);
		//	desktop->insertWindow(targetWindow);	//윈도우 추가
		//	selectedDesktop->removeWindow(targetWindow);	//윈도우 삭제
		//}
		else{
			//안쪽 데스크탑
			E_Desktop* desktop = getDesktop(selectedIndex + 1);
			E_Global* e_global = E_Global::getSingleton();
			desktop->insertWindow(targetWindow);	//윈도우 추가
			//twForHide = targetWindow;
			targetWindow->setHide();//윈도우 숨기기
			selectedDesktop->excludeWindow(targetWindow);	//윈도우 삭제

			//
			if (e_global->mapopen && hwnd_cwnd->m_hWnd != NULL)
			{
				hotkeyinvalidate = true;
				::SendMessage(hwnd_cwnd->m_hWnd, WM_USER_EVENT, 0, 0);
				//E_Map* e_map = E_Map::getSingleton(); e_map->leave2 = false;
			}
			if (!(e_global->mapopen))
			{
				E_Map* e_map = E_Map::getSingleton();
				e_map->drawMap();
			}
			//
			EnumWindows(E_Global::EnumHide, 0);

		}
	}
}

// 탑 윈도우를 아래로 데스크탑으로 전달
void E_Global::moveTopWindowDown(){
	onUpdate();
	list<E_Window*> windowList = selectedDesktop->getWindowList();
	list<E_Window*>::reverse_iterator iter = windowList.rbegin();
	if (iter != windowList.rend()){
		E_Window* targetWindow = *iter;
		twForHide = targetWindow;
		if (desktopCount == 1)
			return;

		if (((desktopCount - desktopwidth) <= selectedIndex) && selectedIndex < desktopCount) {
			//nothing
			////가장 오른쪽 데스크탑
			//E_Desktop* desktop = getDesktop(selectedIndex + 1);
			//desktop->insertWindow(targetWindow);	//윈도우 추가
			//selectedDesktop->removeWindow(targetWindow);	//윈도우 삭제
		}
		//else if ((selectedIndex % desktopwidth) == 0){
		//	//가장 왼쪽 데스크탑
		//	E_Desktop* desktop = getDesktop(selectedIndex + 1);
		//	desktop->insertWindow(targetWindow);	//윈도우 추가
		//	selectedDesktop->removeWindow(targetWindow);	//윈도우 삭제
		//}
		else{
			//안쪽 데스크탑
			E_Desktop* desktop = getDesktop(selectedIndex + desktopwidth);
			desktop->insertWindow(targetWindow);	//윈도우 추가
			//twForHide = targetWindow;
			targetWindow->setHide();
			selectedDesktop->excludeWindow(targetWindow);	//윈도우 삭제

			//
			E_Global* e_global = E_Global::getSingleton();
			if (e_global->mapopen && hwnd_cwnd->m_hWnd != NULL)
			{
				hotkeyinvalidate = true;
				::SendMessage(hwnd_cwnd->m_hWnd, WM_USER_EVENT, 0, 0);
				//E_Map* e_map = E_Map::getSingleton(); e_map->leave2 = false;
			}

			if (!(e_global->mapopen))
			{
				E_Map* e_map = E_Map::getSingleton();
				e_map->drawMap();
			}
			EnumWindows(E_Global::EnumHide, 0);

			//
		}
	}
}


// 탑 윈도우를 위쪽 데스크탑으로 전달
void E_Global::moveTopWindowUp(){
	onUpdate();
	list<E_Window*> windowList = selectedDesktop->getWindowList();
	list<E_Window*>::reverse_iterator iter = windowList.rbegin();
	if (iter != windowList.rend()){
		E_Window* targetWindow = *iter;
		twForHide = targetWindow;
		if (desktopCount == 1)
			return;

		if (0 <= selectedIndex && selectedIndex < desktopwidth) {
			//nothing
			////가장 오른쪽 데스크탑
			//E_Desktop* desktop = getDesktop(selectedIndex + 1);
			//desktop->insertWindow(targetWindow);	//윈도우 추가
			//selectedDesktop->removeWindow(targetWindow);	//윈도우 삭제
		}
		//else if ((selectedIndex % desktopwidth) == 0){
		//	//가장 왼쪽 데스크탑
		//	E_Desktop* desktop = getDesktop(selectedIndex + 1);
		//	desktop->insertWindow(targetWindow);	//윈도우 추가
		//	selectedDesktop->removeWindow(targetWindow);	//윈도우 삭제
		//}
		else{
			//안쪽 데스크탑
			E_Desktop* desktop = getDesktop(selectedIndex - desktopwidth);
			desktop->insertWindow(targetWindow);	//윈도우 추가
			//twForHide = targetWindow;
			targetWindow->setHide();
			selectedDesktop->excludeWindow(targetWindow);	//윈도우 삭제

			//
			E_Global* e_global = E_Global::getSingleton();
			if (e_global->mapopen && hwnd_cwnd->m_hWnd != NULL)
			{
				hotkeyinvalidate = true;
				::SendMessage(hwnd_cwnd->m_hWnd, WM_USER_EVENT, 0, 0);
				//E_Map* e_map = E_Map::getSingleton(); e_map->leave2 = false;
			}
			//

			if (!(e_global->mapopen))
			{
				E_Map* e_map = E_Map::getSingleton();
				e_map->drawMap();
			}
			EnumWindows(E_Global::EnumHide, 0);
		}
	}
}

// 왼쪽 데스크 탑으로 이동 (TEST OK)
void E_Global::moveDesktopLeft()
{
	onUpdate();
	if ((selectedIndex) % desktopwidth == 0){
		//nothing
		//int index = desktopCount - 1;
		//E_Desktop* last = (*desktopList.rbegin());
		//if (last != NULL){
		//	selectedDesktop->setAllHide();//숨김
		//	last->setAllShow();	//보여줌

		//	selectedIndex = index;	//인덱스 업데이트
		//	selectedDesktop = last; //포인터 업데이트
		//}
	}
	else{
		int index = selectedIndex - 1;
		E_Desktop* last = getDesktop(index);
		if (last != NULL){
			selectedDesktop->setAllHide();//숨김
			last->setAllShow();	//보여줌

			selectedIndex = index;	//인덱스 업데이트
			selectedDesktop = last; //포인터 업데이트
			//
			E_Global* e_global = E_Global::getSingleton();
			if (e_global->mapopen && hwnd_cwnd->m_hWnd != NULL)
			{
				hotkeyinvalidate = true;
				::SendMessage(hwnd_cwnd->m_hWnd, WM_USER_EVENT, 0, 0);
			}
			//
			//E_Map* e_map = E_Map::getSingleton();
			//e_map->leave2 = false;
			::SendMessage(hwnd_frame, WM_TRAY_EVENT, selectedIndex, 0);
			//::BringWindowToTop(e_map->maphwnd);
			//
			//std::list<HWND> docklist = e_global->dockedWindowList;
			//for (std::list<HWND>::iterator itr_dock = docklist.begin(); itr_dock != docklist.end(); itr_dock++)	//각 데스크탑 별로출력
			//{
			//	::ShowWindow((*itr_dock), SW_SHOW);
			//		
			//	WCHAR name[60];
			//	int get = 0;
			//	char* pStr;
			//	int strSize = WideCharToMultiByte(CP_ACP, 0, name, -1, NULL, 0, NULL, NULL);
			//	pStr = new char[strSize];
			//	WideCharToMultiByte(CP_ACP, 0, name, -1, pStr, strSize, 0, 0);
			//	int resutr = 0;
			//	parnetpid = GetWindowThreadProcessId((*itr_dock), NULL);
			//	if (strstr(pStr, "곰오디오") != NULL || strstr(pStr, "곰플레이어") || strstr(pStr, "스티커"))
			//		EnumWindows(EnumShow, 0);
			//	
			//}

			std::list<E_Desktop*> desklist2 = e_global->desktopList;
			for (std::list<E_Desktop*>::iterator itr_desk = desklist2.begin(); itr_desk != desklist2.end(); itr_desk++)	//각 데스크탑 별로출력
			{
				std::list<E_Window*> winlist2 = (*itr_desk)->getWindowList();
				for (std::list<E_Window*>::iterator itr_window = winlist2.begin(); itr_window != winlist2.end(); itr_window++)	//각 데스크탑 별로출력
				{
					if ((*itr_window)->dock)
					{
						::ShowWindow((*itr_window)->getWindow(), SW_SHOW);

						WCHAR name[60];
						int get = 0;
						char* pStr;
						int strSize = WideCharToMultiByte(CP_ACP, 0, name, -1, NULL, 0, NULL, NULL);
						pStr = new char[strSize];
						WideCharToMultiByte(CP_ACP, 0, name, -1, pStr, strSize, 0, 0);
						int resutr = 0;
						parnetpid = GetWindowThreadProcessId((*itr_window)->getWindow(), NULL);
						if (strstr(pStr, "곰오디오") != NULL || strstr(pStr, "곰플레이어") || strstr(pStr, "스티커"))
							EnumWindows(EnumShow, 0);

					}
				}
			}

		}
	}
}

// 오른쪽 데스크탑으로 이동 (TEST OK)
void E_Global::moveDesktopRight()
{
	onUpdate();
	if (((selectedIndex + 1) % desktopwidth) == 0){
		//nothing
		//int index = 0;
		//E_Desktop* last = (*desktopList.begin());
		//if (last != NULL){
		//	selectedDesktop->setAllHide();//숨김
		//	last->setAllShow();	//보여줌

		//	selectedIndex = index;	//인덱스 업데이트
		//	selectedDesktop = last; //포인터 업데이트
		//}
	}
	else{
		int index = selectedIndex + 1;
		E_Desktop* last = getDesktop(index);

		if (last != NULL){
			selectedDesktop->setAllHide();//숨김
			last->setAllShow();	//보여줌

			selectedIndex = index;	//인덱스 업데이트
			selectedDesktop = last; //포인터 업데이트
			//
			E_Global* e_global = E_Global::getSingleton();
			if (e_global->mapopen && hwnd_cwnd->m_hWnd != NULL)
			{
				hotkeyinvalidate = true;
				::SendMessage(hwnd_cwnd->m_hWnd, WM_USER_EVENT, 0, 0);

			}
			//
			//E_Map* e_map = E_Map::getSingleton();
			//e_map->leave2 = false;
			::SendMessage(hwnd_frame, WM_TRAY_EVENT, selectedIndex, 0);
			//::BringWindowToTop(e_map->maphwnd);
			//

			std::list<E_Desktop*> desklist2 = e_global->desktopList;
			for (std::list<E_Desktop*>::iterator itr_desk = desklist2.begin(); itr_desk != desklist2.end(); itr_desk++)	//각 데스크탑 별로출력
			{
				std::list<E_Window*> winlist2 = (*itr_desk)->getWindowList();
				for (std::list<E_Window*>::iterator itr_window = winlist2.begin(); itr_window != winlist2.end(); itr_window++)	//각 데스크탑 별로출력
				{
					if ((*itr_window)->dock)
					{
						::ShowWindow((*itr_window)->getWindow(), SW_SHOW);

						WCHAR name[60];
						int get = 0;
						char* pStr;
						int strSize = WideCharToMultiByte(CP_ACP, 0, name, -1, NULL, 0, NULL, NULL);
						pStr = new char[strSize];
						WideCharToMultiByte(CP_ACP, 0, name, -1, pStr, strSize, 0, 0);
						int resutr = 0;
						parnetpid = GetWindowThreadProcessId((*itr_window)->getWindow(), NULL);
						if (strstr(pStr, "곰오디오") != NULL || strstr(pStr, "곰플레이어") || strstr(pStr, "스티커"))
							EnumWindows(EnumShow, 0);

					}
				}
			}

		}
	}
}
bool E_Global::gethotkey()
{
	return hotkeyinvalidate;
}
void E_Global::sethotkey(bool value)
{
	hotkeyinvalidate = value;
}
// 위쪽 데스크탑으로 이동 (TEST OK)
void E_Global::moveDesktopUp()
{
	onUpdate();
	if (0 <= selectedIndex && selectedIndex < desktopwidth){
		//nothing
	}
	else{
		int index = selectedIndex - desktopwidth;
		E_Desktop* last = getDesktop(index);
		if (last != NULL){
			selectedDesktop->setAllHide();//숨김
			last->setAllShow();	//보여줌

			selectedIndex = index;	//인덱스 업데이트
			selectedDesktop = last; //포인터 업데이트
			//
			E_Global* e_global = E_Global::getSingleton();
			if (e_global->mapopen && hwnd_cwnd->m_hWnd != NULL)
			{
				hotkeyinvalidate = true;
				::SendMessage(hwnd_cwnd->m_hWnd, WM_USER_EVENT, 0, 0);
			}
			//
			//E_Map* e_map = E_Map::getSingleton();
			//e_map->leave2 = false;
			::SendMessage(hwnd_frame, WM_TRAY_EVENT, selectedIndex, 0);
			//::BringWindowToTop(e_map->maphwnd);
			std::list<E_Desktop*> desklist2 = e_global->desktopList;
			for (std::list<E_Desktop*>::iterator itr_desk = desklist2.begin(); itr_desk != desklist2.end(); itr_desk++)	//각 데스크탑 별로출력
			{
				std::list<E_Window*> winlist2 = (*itr_desk)->getWindowList();
				for (std::list<E_Window*>::iterator itr_window = winlist2.begin(); itr_window != winlist2.end(); itr_window++)	//각 데스크탑 별로출력
				{
					if ((*itr_window)->dock)
					{
						::ShowWindow((*itr_window)->getWindow(), SW_SHOW);

						WCHAR name[60];
						int get = 0;
						char* pStr;
						int strSize = WideCharToMultiByte(CP_ACP, 0, name, -1, NULL, 0, NULL, NULL);
						pStr = new char[strSize];
						WideCharToMultiByte(CP_ACP, 0, name, -1, pStr, strSize, 0, 0);
						int resutr = 0;
						parnetpid = GetWindowThreadProcessId((*itr_window)->getWindow(), NULL);
						if (strstr(pStr, "곰오디오") != NULL || strstr(pStr, "곰플레이어") || strstr(pStr, "스티커"))
							EnumWindows(EnumShow, 0);

					}
				}
			}

		
		
		
		}
	}
}
// 아래쪽 데스크탑으로 이동 (TEST OK)
void E_Global::moveDesktopDown()
{
	onUpdate();
	if (((desktopCount - desktopwidth) <= selectedIndex) && selectedIndex < desktopCount){
		//nothing
	}
	else{
		int index = selectedIndex + desktopwidth;
		E_Desktop* last = getDesktop(index);
		if (last != NULL){
			selectedDesktop->setAllHide();//숨김
			last->setAllShow();	//보여줌

			selectedIndex = index;	//인덱스 업데이트
			selectedDesktop = last; //포인터 업데이트
			//
			E_Global* e_global = E_Global::getSingleton();
			if (e_global->mapopen && hwnd_cwnd->m_hWnd != NULL)
			{
				hotkeyinvalidate = true;
				::SendMessage(hwnd_cwnd->m_hWnd, WM_USER_EVENT, 0, 0);
			}
			//
			//E_Map* e_map = E_Map::getSingleton();
			//e_map->leave2 = false;
			::SendMessage(hwnd_frame, WM_TRAY_EVENT, selectedIndex, 0);
			//::BringWindowToTop(e_map->maphwnd);
			std::list<E_Desktop*> desklist2 = e_global->desktopList;
			for (std::list<E_Desktop*>::iterator itr_desk = desklist2.begin(); itr_desk != desklist2.end(); itr_desk++)	//각 데스크탑 별로출력
			{
				std::list<E_Window*> winlist2 = (*itr_desk)->getWindowList();
				for (std::list<E_Window*>::iterator itr_window = winlist2.begin(); itr_window != winlist2.end(); itr_window++)	//각 데스크탑 별로출력
				{
					if ((*itr_window)->dock)
					{
						::ShowWindow((*itr_window)->getWindow(), SW_SHOW);
						WCHAR name[60];
						::GetWindowText((*itr_window)->getWindow(), name, 60);
						char* pStr;
						int strSize = WideCharToMultiByte(CP_ACP, 0, name, -1, NULL, 0, NULL, NULL);
						pStr = new char[strSize];
						WideCharToMultiByte(CP_ACP, 0, name, -1, pStr, strSize, 0, 0);
						int resutr = 0;
						parnetpid = GetWindowThreadProcessId((*itr_window)->getWindow(), NULL);
						if (strstr(pStr, "곰오디오") != NULL || strstr(pStr, "곰플레이어") || strstr(pStr, "스티커"))
							EnumWindows(EnumShow, 0);
						
					}
				}
			}
		
		
		
		}
	}
}
BOOL CALLBACK  E_Global::EnumShow(HWND hwnd, LPARAM lParam)
{
	WCHAR name[10];
	WCHAR name2[4];
	WCHAR name3[] = L"스티커";
	//WCHAR name4[] = L"카카오";
	::GetWindowText(hwnd, name2, 4);

	if ((::GetWindowText(hwnd, name, 10) && ::IsWindowVisible(hwnd)) || wcscmp(name2, name3) == 0)//|| wcscmp(name4, name5) == 0)
	{
		E_Global* e_global = E_Global::getSingleton();
		DWORD childprocessId;
		childprocessId = GetWindowThreadProcessId(hwnd, NULL);
		if (childprocessId == e_global->parnetpid)
			::ShowWindow(hwnd, SW_SHOW);
	}
	return true;
}


// 리스트를 Z-Index로 정렬해서 반환하는 함수
void E_Global::sortZIndexWindowList(list<E_Window*>& target)
{
	list<HWND> allWindows = getAllWindowsForZIndex();
	list<E_Window*> result;
	for (list<HWND>::reverse_iterator iterWindows = allWindows.rbegin(); iterWindows != allWindows.rend(); iterWindows++){
		for (list<E_Window*>::iterator iter = target.begin(); iter != target.end(); iter++){
			if (*iterWindows == (*iter)->getWindow()){
				result.push_back(*iter);
			}
		}
	}
	target.clear();
	target = result;
}

//Z-Index를 위한 모든 윈도우를 가져온다.
list<HWND> E_Global::getAllWindowsForZIndex()
{
	E_Global* object = E_Global::getSingleton();
	object->windowListForZIndex.clear();	//초기화
	EnumWindows(E_Global::EnumCallBackForZIndex, 0);
	return object->windowListForZIndex;
}

//Z-Index 정렬을 위한 콜백
BOOL CALLBACK  E_Global::EnumCallBackForZIndex(HWND hwnd, LPARAM lParam)
{
	E_Global *global = E_Global::getSingleton();

	WCHAR Cap[255];
	int length;
	::GetWindowText(hwnd, Cap, 254);
	length = ::GetWindowTextLength(hwnd);

	global->windowListForZIndex.push_back(hwnd);

	return TRUE;
}

void E_Global::moveDesktop(int index)
{
	onUpdate();
	if (0 <= index && index < desktopCount){
		//index = selectedIndex + 1;
		E_Desktop* last = getDesktop(index);
		if (last != NULL){

			selectedDesktop->setAllHide();//숨김
			for (list<HWND>::iterator iter = dockedWindowList.begin(); iter != dockedWindowList.end(); iter++){
				ShowWindow((*iter), SW_SHOW);
			}
			last->setAllShow();	//보여줌

			selectedIndex = index;	//인덱스 업데이트
			selectedDesktop = last; //포인터 업데이트
		}
	}
}

void E_Global::changeDesktop(int pastvalue, int newvalue)
{
	E_Global* e_global = E_Global::getSingleton();
	E_Map* e_map = E_Map::getSingleton();
	int pastdesknum = pastvalue;
	int newdesknum = newvalue;
	setSelectedIndex(0);
	e_global->desktopCount = newvalue;
	//일단 모든 Desktop의 리스트를 가져와서 복사후 지역변수로 저장후 다시 글로벌로

	if (pastdesknum > newdesknum)	//기존의 Desktop이 더많은경우 ex) 9->6
	{
		list<E_Desktop*> newdesktopList = desktopList;
		desktopList.clear();
		E_Desktop* tmpDesk;
		int i = 0;
		for (std::list<E_Desktop*>::iterator itr_desktopto = newdesktopList.begin(); itr_desktopto != newdesktopList.end(); itr_desktopto++, i++)	//각데스크탑별로 안에 있는 윈도우 핸들 가져와서 아이콘 출력
		{
			if (i < newdesknum)// 기존의 Desktop의 해당 6개 전까지의 경우
			{
				desktopList.push_back(*itr_desktopto);
				tmpDesk = (*itr_desktopto);	//tmpDesk는 가장 마지막 Desktop
			}
			else				//6개 이후
			{
				//마지막 Desktop 인 tmpDesk 에다가 나머지 (*itr_desktopto) 인 데스크 탑들의 모든 윈도우 넣음
				list<E_Window*> tmpwindow = (*itr_desktopto)->getWindowList();
				for (std::list<E_Window*>::iterator itr_tmpwindow = tmpwindow.begin(); itr_tmpwindow != tmpwindow.end(); itr_tmpwindow++)	//각데스크탑별로 안에 있는 윈도우 핸들 가져와서 아이콘 출력
				{
					tmpDesk->insertWindow((*itr_tmpwindow));
				}
			}
		}

	}
	else // 새로운 데스크탑이 더 많은경우
	{
		for (int i = pastvalue; i < newvalue; i++)
			desktopList.push_back(new E_Desktop(i));
	}
	std::list<E_Desktop*> desklist = e_global->desktopList;
	for (std::list<E_Desktop*>::iterator itr_desk = desklist.begin(); itr_desk != desklist.end(); itr_desk++)	//각 데스크탑 별로출력
	{
		if ((*itr_desk)->getIndex() == e_global->getSelectedDesktop()->getIndex())
		{
			(*itr_desk)->setAllShow();
			continue;
		}
		(*itr_desk)->setAllHide();
	}
	//e_map->leave2 = false;
	::BringWindowToTop(e_map->maphwnd);

}

BOOL CALLBACK E_Global::EnumHide(HWND hwnd, LPARAM lParam)
{
	WCHAR name[10];
	WCHAR name2[4];
	WCHAR name3[] = L"스티커";
	::GetWindowText(hwnd, name2, 4);
	if ((::GetWindowText(hwnd, name, 10) && ::IsWindowVisible(hwnd)) || wcscmp(name2, name3) == 0)//|| wcscmp(name4, name5) == 0)
	{
		E_Map* e_map = E_Map::getSingleton();
		E_Global* e_global = E_Global::getSingleton();
		DWORD childprocessId, parentprocessId;
		parentprocessId = GetWindowThreadProcessId(e_global->twForHide->getWindow(), NULL);
		childprocessId = GetWindowThreadProcessId(hwnd, NULL);
		if (childprocessId == e_map->parentprocessId)
		{
			::ShowWindow(hwnd, SW_HIDE);
		}
	}
	
	return true;
}