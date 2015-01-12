#include "stdafx.h"
#include "E_Global.h"
#pragma once
#include <list>

E_Global* E_Global::singleton = NULL;

const wchar_t* E_Global::testFrameName = L"expandable";

E_Global::E_Global() : selectedDesktop(NULL), updateMode(false), currentThread(NULL)
{
	mapsize = 0.06;
	iconsize = 1;
	desktopwidth = 4;
	desktopheight = 3;
	//설정 파일을 읽어온 후
	desktopCount = desktopwidth*desktopheight;
	
	//데스크탑 생성
	for (int i = 0; i < desktopCount; i++)
		desktopList.push_back(new E_Desktop(i));
	
	//초기 데스크탑 
	selectedDesktop = *(desktopList.begin());
	selectedIndex = 0;

	//윈도우 리스트 초기화 // 생성자 안이라 초기화 불가능
	//
}

E_Global::~E_Global()
{

}
double E_Global::getMapsize()
{
	return mapsize;
}
double E_Global::getIconsize()
{
	return iconsize;
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
	object->windowListForUpdate.clear();	//초기화
	EnumWindows(E_Global::EnumCallBack, 0);
	return object->windowListForUpdate;
}

BOOL CALLBACK  E_Global::EnumCallBack(HWND hwnd, LPARAM lParam)
{

	HWND myhwnd = FindWindow(NULL, E_Global::testFrameName);
	E_Global *global = E_Global::getSingleton();

	WCHAR Cap[255];
	int length;
	::GetWindowText(hwnd, Cap, 254);
	length = ::GetWindowTextLength(hwnd);

	if (IsWindowVisible(hwnd))
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
	TRACE_WIN32A("E_Global::startUpdate()");
	if (currentThread == NULL && updateMode != true){
		updateMode = true;
		thread* t = new thread{ &E_Global::loopUpdate, this };
		currentThread = t;
		return true;
	}
	return false;
}

// 실시간으로 윈도우 업데이트
void E_Global::loopUpdate()
{
	//TRACE_WIN32A("E_Global::loopUpdate()");
	while (updateMode){
		if (onUpdate()){
			//TRACE_WIN32A("E_Global::loopUpdate()");
			windowSwitcher->updateSelectedDesktop();
			desktopSwitcher->updateSelectedDesktop();
			mapSwitcher->updateSelectedDesktop();
			dragSwitcher->updateSelectedDesktop();
	}
		Sleep(100);
}
}

// 업데이트 쓰레드// 변화될 경우 참을 반환한다.
bool E_Global::onUpdate()
{
	bool result = false;
	//TRACE_WIN32A("E_Global::onUpdate()");
	//뮤텍스
	//lock_guard<mutex> lockGuard(E_Mutex::updateMutex);//데드락
	//윈도우 리스트 업데이트
	list<HWND> wlist = getAllWindows();
	list<E_Window*> selectedWindows = selectedDesktop->getWindowList();
	//selectedDesktop->clearWindow();
	//윈도우 추가

	Sleep(2000);
	//TRACE_WIN32A("E_Global::onUpdate ING...()");
	//사이즈가 다르거나 마지막이 다르다면.. 업데이트 수행
	if (wlist.size() != selectedWindows.size() || ((*wlist.rbegin()) != (*selectedWindows.rbegin())->getWindow())){
		//바뀐 윈도우만 업데이트
		int wlistSize = wlist.size();
		int selectedSize = selectedWindows.size();
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
	TRACE_WIN32A("E_Global::stopUpdate()");
	if (currentThread != NULL && updateMode == true){
		updateMode = false;
		currentThread->join();	//스레드가 끝날때까지 대기
		delete currentThread;
		currentThread = NULL;
		return true;
	}
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
	TRACE_WIN32A("HELL");
	window->updateSelectedDesktop();
	desktop->updateSelectedDesktop();
	map->updateSelectedDesktop();
	drag->updateSelectedDesktop();*/
	this->windowSwitcher = window;
	this->dragSwitcher = drag;
	this->mapSwitcher = map;
	this->desktopSwitcher = desktop;
	list<HWND> wlist = getAllWindows();
	for (list<HWND>::iterator iter = wlist.begin(); iter != wlist.end(); iter++) {
		E_Window* window = new E_Window(*iter);
		selectedDesktop->insertWindow(window);
	}
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
	E_Window* targetWindow = *iter;

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
		targetWindow->setHide();
		selectedDesktop->excludeWindow(targetWindow);	//윈도우 삭제
	}
}

// 탑 윈도우를 오른쪽 데스크탑으로 전달 (TEST OK)
void E_Global::moveTopWindowRight(){
	onUpdate();
	list<E_Window*> windowList = selectedDesktop->getWindowList();
	list<E_Window*>::reverse_iterator iter = windowList.rbegin();
	E_Window* targetWindow = *iter;
	
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
		desktop->insertWindow(targetWindow);	//윈도우 추가
		targetWindow->setHide();//윈도우 숨기기
		selectedDesktop->excludeWindow(targetWindow);	//윈도우 삭제
	}
}

// 탑 윈도우를 아래로 데스크탑으로 전달
void E_Global::moveTopWindowDown(){
	onUpdate();
	list<E_Window*> windowList = selectedDesktop->getWindowList();
	list<E_Window*>::reverse_iterator iter = windowList.rbegin();
	E_Window* targetWindow = *iter;

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
		targetWindow->setHide();
		selectedDesktop->excludeWindow(targetWindow);	//윈도우 삭제
	}
}


// 탑 윈도우를 위쪽 데스크탑으로 전달
void E_Global::moveTopWindowUp(){
	onUpdate();
	list<E_Window*> windowList = selectedDesktop->getWindowList();
	list<E_Window*>::reverse_iterator iter = windowList.rbegin();
	E_Window* targetWindow = *iter;

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
		targetWindow->setHide();
		selectedDesktop->excludeWindow(targetWindow);	//윈도우 삭제
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
		}
	}
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
		}
	}
}