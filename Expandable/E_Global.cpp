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

	//���� ������ �о�� ��
	desktopCount = desktopwidth*desktopheight;
	notifyAblePos[0] = notifyAblePos[1] = notifyAblePos[2] = notifyAblePos[3] = notifyAblePos[4] = 0;
	//����ũž ����
	for (int i = 0; i < desktopCount; i++)
		desktopList.push_back(new E_Desktop(i));

	//�ʱ� ����ũž 
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	settingTimer = value;
}
int E_Global::getTimer()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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

//��� ����� ���� �ݹ�
void E_Global::OnDualMonitorMode(bool dualMonitorMode){
	SendMessage(hwnd_frame, WM_CHANGEDISPLAY, 0, 0);
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
	object->windowListForUpdate.clear();	//�ʱ�ȭ
	
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
		// ����ȭ��, �½�Ʈ��, ���۹�ư ����, expandable �׽�Ʈ ������ ����, spy ++ ����
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
				{ //�ֻ��� �����츸 ����
					// ���� ����. �ߺ��Ǵ� �޸� �ּҰ� ������ �߰� ���� (CWND �� HWND �ߺ� ����)
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


// ���� ����ũž ��ȯ
E_Desktop* E_Global::getSelectedDesktop()
{
	return selectedDesktop;
}


// �ֱ��� ������Ʈ�� ���� Ÿ�̸�
void E_Global::startTimer()
{
	updateMode = true;
}


void E_Global::onTimer()
{
}

// ������Ʈ ����
bool E_Global::startUpdate()
{
	TRACE_WIN32A("E_Global::startUpdate() before");
	lock_guard<mutex> start(E_Mutex::updateStartStopMutex);
	if (currentThread == NULL && updateMode != true){
		TRACE_WIN32A("E_Global::startUpdate()");
		updateMode = true;

		thread* t = new thread{ &E_Global::loopUpdate, this};
		thread_map.insert(std::unordered_map<std::thread::id, bool>::value_type(t->get_id(), false));	//�÷��� ����
		thread_inner_map.insert(std::unordered_map<std::thread::id, bool>::value_type(t->get_id(), false));	//�÷��� ����
		thread_pmap.insert(std::unordered_map<std::thread::id, thread*>::value_type(t->get_id(), t));	//������ ����
		currentThread = t;

		TRACE_WIN32A("E_Global::startUpdate() after");
		return true;
	}
	TRACE_WIN32A("E_Global::startUpdate() after");
	return false;
}

// �ǽð����� ������ ������Ʈ
void E_Global::loopUpdate()
{
	bool update = false;
	//���� ��ȿ�� ��
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
		update = thread_map.find(std::this_thread::get_id())->second;//�÷��� ������Ʈ
	}
	if (update == true){
		thread_inner_map.find(std::this_thread::get_id())->second = true;
	}
}

// ������Ʈ ������// ��ȭ�� ��� ���� ��ȯ�Ѵ�.
bool E_Global::onUpdate()
{
	bool result = false;
	//������ ����Ʈ ������Ʈ
	//TRACE_WIN32A("E_Global::getAllWindows before");
	list<HWND> wlist = getAllWindows();	//UI �����忡�� ȣ��Ǵ� �ݹ��̶� ����� �߻��ϱ� ����
	//TRACE_WIN32A("E_Global::getAllWindows after");
	list<E_Window*> selectedWindows = selectedDesktop->getWindowList();
	//������ �߰�
	int wlistSize = wlist.size();
	int selectedSize = selectedWindows.size();

	//TRACE_WIN32A("E_Global::onUpdate ING...()");
	//����� �ٸ��ų� �������� �ٸ��ٸ�.. ������Ʈ ����
	if (wlist.size() != selectedWindows.size() || ((wlistSize != 0 && selectedSize != 0) && ((*wlist.rbegin()) != (*selectedWindows.rbegin())->getWindow()))){
		//�ٲ� �����츸 ������Ʈ
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
		//����Ʈ ������Ʈ
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
		//���� �����츦 ������ ������ ����
		selectedDesktop->clearWindow();
		//���� ����Ʈ�� ������Ʈ
		for (list<E_Window*>::iterator iter = noChangeList.begin(); iter != noChangeList.end(); iter++){
			selectedDesktop->insertWindow(*iter);
		}
		//������Ʈ
		result = true;
	}
	else{

	}
	//TRACE_WIN32A("E_Global::onUpdateEnd()");
	return result;
}


// ������Ʈ�� ���ߴ� �Լ� (������ �÷��׸� �ٲ���)
bool E_Global::stopUpdate()
{
	//����ȭ
	lock_guard<mutex> stop(E_Mutex::updateStartStopMutex);

	list<std::thread::id> colleting;
	//������ �÷��� (�Ҵ� ����)
	for (std::unordered_map<std::thread::id, bool>::iterator iter = thread_inner_map.begin(); iter != thread_inner_map.end(); iter++){
		if (iter->second == true){
			//���� �÷��� �˻�
			std::unordered_map<std::thread::id, thread*>::iterator target;
			if ((target = thread_pmap.find(iter->first)) != thread_pmap.end()){
				target->second->detach();
				//target->second->
				delete target->second;
				//������ �Ҵ� ����
				colleting.push_back(iter->first);
			}
		}
	}
	//������ �÷��� (�ʿ��� ����)
	for (list<std::thread::id>::iterator iter = colleting.begin(); iter != colleting.end(); iter++){
		thread_map.erase(*iter);
		thread_pmap.erase(*iter);
		thread_inner_map.erase(*iter);
	}

	TRACE_WIN32A("E_Global::stopUpdate() before");
	if (currentThread != NULL && updateMode == true){
		TRACE_WIN32A("E_Global::stopUpdate()");

		thread_map.find(currentThread->get_id())->second = true;		//���� �÷��׷� ���� //������ ��⸦ ������ �÷������� �ذ�

		//delete currentThread;
		//������Ʈ ����
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


// �����ڿ��� �ʱ�ȭ ���� ���ϴ� �͵��� �ʱ�ȭ �ϴ� �Լ�
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

	//��� ������
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


// ����ũž�� �����´�. �������� index �� ��� NULL ���� 
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


// ž �����츦 ���� ����ũž���� ���� (TEST OK)
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
			////���� ������ ����ũž
			//E_Desktop* desktop = getDesktop(selectedIndex + 1);
			//desktop->insertWindow(targetWindow);	//������ �߰�
			//selectedDesktop->removeWindow(targetWindow);	//������ ����
		}
		//else if ((selectedIndex % desktopwidth) == 0){
		//	//���� ���� ����ũž
		//	E_Desktop* desktop = getDesktop(selectedIndex + 1);
		//	desktop->insertWindow(targetWindow);	//������ �߰�
		//	selectedDesktop->removeWindow(targetWindow);	//������ ����
		//}
		else{
			//���� ����ũž
			E_Desktop* desktop = getDesktop(selectedIndex - 1);
			desktop->insertWindow(targetWindow);	//������ �߰�
			//twForHide = targetWindow;
			targetWindow->setHide();
			selectedDesktop->excludeWindow(targetWindow);	//������ ����

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

// ž �����츦 ������ ����ũž���� ���� (TEST OK)
void E_Global::moveTopWindowRight(){
	onUpdate();
	list<E_Window*> windowList = selectedDesktop->getWindowList();
	list<E_Window*>::reverse_iterator iter = windowList.rbegin();
	if (iter != windowList.rend()){
		E_Window* targetWindow = *iter;
		twForHide = targetWindow;
		TRACE_WIN32A("[E_Global::moveTopWindowRight] ������ �̸�: %s", targetWindow->getWindowName());

		if (desktopCount == 1)
			return;

		if (((selectedIndex + 1) % desktopwidth) == 0) {
			//nothing
			////���� ������ ����ũž
			//E_Desktop* desktop = getDesktop(selectedIndex + 1);
			//desktop->insertWindow(targetWindow);	//������ �߰�
			//selectedDesktop->removeWindow(targetWindow);	//������ ����
		}
		//else if ((selectedIndex % desktopwidth) == 0){
		//	//���� ���� ����ũž
		//	E_Desktop* desktop = getDesktop(selectedIndex + 1);
		//	desktop->insertWindow(targetWindow);	//������ �߰�
		//	selectedDesktop->removeWindow(targetWindow);	//������ ����
		//}
		else{
			//���� ����ũž
			E_Desktop* desktop = getDesktop(selectedIndex + 1);
			E_Global* e_global = E_Global::getSingleton();
			desktop->insertWindow(targetWindow);	//������ �߰�
			//twForHide = targetWindow;
			targetWindow->setHide();//������ �����
			selectedDesktop->excludeWindow(targetWindow);	//������ ����

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

// ž �����츦 �Ʒ��� ����ũž���� ����
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
			////���� ������ ����ũž
			//E_Desktop* desktop = getDesktop(selectedIndex + 1);
			//desktop->insertWindow(targetWindow);	//������ �߰�
			//selectedDesktop->removeWindow(targetWindow);	//������ ����
		}
		//else if ((selectedIndex % desktopwidth) == 0){
		//	//���� ���� ����ũž
		//	E_Desktop* desktop = getDesktop(selectedIndex + 1);
		//	desktop->insertWindow(targetWindow);	//������ �߰�
		//	selectedDesktop->removeWindow(targetWindow);	//������ ����
		//}
		else{
			//���� ����ũž
			E_Desktop* desktop = getDesktop(selectedIndex + desktopwidth);
			desktop->insertWindow(targetWindow);	//������ �߰�
			//twForHide = targetWindow;
			targetWindow->setHide();
			selectedDesktop->excludeWindow(targetWindow);	//������ ����

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


// ž �����츦 ���� ����ũž���� ����
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
			////���� ������ ����ũž
			//E_Desktop* desktop = getDesktop(selectedIndex + 1);
			//desktop->insertWindow(targetWindow);	//������ �߰�
			//selectedDesktop->removeWindow(targetWindow);	//������ ����
		}
		//else if ((selectedIndex % desktopwidth) == 0){
		//	//���� ���� ����ũž
		//	E_Desktop* desktop = getDesktop(selectedIndex + 1);
		//	desktop->insertWindow(targetWindow);	//������ �߰�
		//	selectedDesktop->removeWindow(targetWindow);	//������ ����
		//}
		else{
			//���� ����ũž
			E_Desktop* desktop = getDesktop(selectedIndex - desktopwidth);
			desktop->insertWindow(targetWindow);	//������ �߰�
			//twForHide = targetWindow;
			targetWindow->setHide();
			selectedDesktop->excludeWindow(targetWindow);	//������ ����

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

// ���� ����ũ ž���� �̵� (TEST OK)
void E_Global::moveDesktopLeft()
{
	onUpdate();
	if ((selectedIndex) % desktopwidth == 0){
		//nothing
		//int index = desktopCount - 1;
		//E_Desktop* last = (*desktopList.rbegin());
		//if (last != NULL){
		//	selectedDesktop->setAllHide();//����
		//	last->setAllShow();	//������

		//	selectedIndex = index;	//�ε��� ������Ʈ
		//	selectedDesktop = last; //������ ������Ʈ
		//}
	}
	else{
		int index = selectedIndex - 1;
		E_Desktop* last = getDesktop(index);
		if (last != NULL){
			selectedDesktop->setAllHide();//����
			last->setAllShow();	//������

			selectedIndex = index;	//�ε��� ������Ʈ
			selectedDesktop = last; //������ ������Ʈ
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
			//for (std::list<HWND>::iterator itr_dock = docklist.begin(); itr_dock != docklist.end(); itr_dock++)	//�� ����ũž �������
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
			//	if (strstr(pStr, "�������") != NULL || strstr(pStr, "���÷��̾�") || strstr(pStr, "��ƼĿ"))
			//		EnumWindows(EnumShow, 0);
			//	
			//}

			std::list<E_Desktop*> desklist2 = e_global->desktopList;
			for (std::list<E_Desktop*>::iterator itr_desk = desklist2.begin(); itr_desk != desklist2.end(); itr_desk++)	//�� ����ũž �������
			{
				std::list<E_Window*> winlist2 = (*itr_desk)->getWindowList();
				for (std::list<E_Window*>::iterator itr_window = winlist2.begin(); itr_window != winlist2.end(); itr_window++)	//�� ����ũž �������
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
						if (strstr(pStr, "�������") != NULL || strstr(pStr, "���÷��̾�") || strstr(pStr, "��ƼĿ"))
							EnumWindows(EnumShow, 0);

					}
				}
			}

		}
	}
}

// ������ ����ũž���� �̵� (TEST OK)
void E_Global::moveDesktopRight()
{
	onUpdate();
	if (((selectedIndex + 1) % desktopwidth) == 0){
		//nothing
		//int index = 0;
		//E_Desktop* last = (*desktopList.begin());
		//if (last != NULL){
		//	selectedDesktop->setAllHide();//����
		//	last->setAllShow();	//������

		//	selectedIndex = index;	//�ε��� ������Ʈ
		//	selectedDesktop = last; //������ ������Ʈ
		//}
	}
	else{
		int index = selectedIndex + 1;
		E_Desktop* last = getDesktop(index);

		if (last != NULL){
			selectedDesktop->setAllHide();//����
			last->setAllShow();	//������

			selectedIndex = index;	//�ε��� ������Ʈ
			selectedDesktop = last; //������ ������Ʈ
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
			for (std::list<E_Desktop*>::iterator itr_desk = desklist2.begin(); itr_desk != desklist2.end(); itr_desk++)	//�� ����ũž �������
			{
				std::list<E_Window*> winlist2 = (*itr_desk)->getWindowList();
				for (std::list<E_Window*>::iterator itr_window = winlist2.begin(); itr_window != winlist2.end(); itr_window++)	//�� ����ũž �������
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
						if (strstr(pStr, "�������") != NULL || strstr(pStr, "���÷��̾�") || strstr(pStr, "��ƼĿ"))
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
// ���� ����ũž���� �̵� (TEST OK)
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
			selectedDesktop->setAllHide();//����
			last->setAllShow();	//������

			selectedIndex = index;	//�ε��� ������Ʈ
			selectedDesktop = last; //������ ������Ʈ
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
			for (std::list<E_Desktop*>::iterator itr_desk = desklist2.begin(); itr_desk != desklist2.end(); itr_desk++)	//�� ����ũž �������
			{
				std::list<E_Window*> winlist2 = (*itr_desk)->getWindowList();
				for (std::list<E_Window*>::iterator itr_window = winlist2.begin(); itr_window != winlist2.end(); itr_window++)	//�� ����ũž �������
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
						if (strstr(pStr, "�������") != NULL || strstr(pStr, "���÷��̾�") || strstr(pStr, "��ƼĿ"))
							EnumWindows(EnumShow, 0);

					}
				}
			}

		
		
		
		}
	}
}
// �Ʒ��� ����ũž���� �̵� (TEST OK)
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
			selectedDesktop->setAllHide();//����
			last->setAllShow();	//������

			selectedIndex = index;	//�ε��� ������Ʈ
			selectedDesktop = last; //������ ������Ʈ
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
			for (std::list<E_Desktop*>::iterator itr_desk = desklist2.begin(); itr_desk != desklist2.end(); itr_desk++)	//�� ����ũž �������
			{
				std::list<E_Window*> winlist2 = (*itr_desk)->getWindowList();
				for (std::list<E_Window*>::iterator itr_window = winlist2.begin(); itr_window != winlist2.end(); itr_window++)	//�� ����ũž �������
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
						if (strstr(pStr, "�������") != NULL || strstr(pStr, "���÷��̾�") || strstr(pStr, "��ƼĿ"))
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
	WCHAR name3[] = L"��ƼĿ";
	//WCHAR name4[] = L"īī��";
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


// ����Ʈ�� Z-Index�� �����ؼ� ��ȯ�ϴ� �Լ�
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

//Z-Index�� ���� ��� �����츦 �����´�.
list<HWND> E_Global::getAllWindowsForZIndex()
{
	E_Global* object = E_Global::getSingleton();
	object->windowListForZIndex.clear();	//�ʱ�ȭ
	EnumWindows(E_Global::EnumCallBackForZIndex, 0);
	return object->windowListForZIndex;
}

//Z-Index ������ ���� �ݹ�
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

			selectedDesktop->setAllHide();//����
			for (list<HWND>::iterator iter = dockedWindowList.begin(); iter != dockedWindowList.end(); iter++){
				ShowWindow((*iter), SW_SHOW);
			}
			last->setAllShow();	//������

			selectedIndex = index;	//�ε��� ������Ʈ
			selectedDesktop = last; //������ ������Ʈ
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
	//�ϴ� ��� Desktop�� ����Ʈ�� �����ͼ� ������ ���������� ������ �ٽ� �۷ι���

	if (pastdesknum > newdesknum)	//������ Desktop�� ��������� ex) 9->6
	{
		list<E_Desktop*> newdesktopList = desktopList;
		desktopList.clear();
		E_Desktop* tmpDesk;
		int i = 0;
		for (std::list<E_Desktop*>::iterator itr_desktopto = newdesktopList.begin(); itr_desktopto != newdesktopList.end(); itr_desktopto++, i++)	//������ũž���� �ȿ� �ִ� ������ �ڵ� �����ͼ� ������ ���
		{
			if (i < newdesknum)// ������ Desktop�� �ش� 6�� �������� ���
			{
				desktopList.push_back(*itr_desktopto);
				tmpDesk = (*itr_desktopto);	//tmpDesk�� ���� ������ Desktop
			}
			else				//6�� ����
			{
				//������ Desktop �� tmpDesk ���ٰ� ������ (*itr_desktopto) �� ����ũ ž���� ��� ������ ����
				list<E_Window*> tmpwindow = (*itr_desktopto)->getWindowList();
				for (std::list<E_Window*>::iterator itr_tmpwindow = tmpwindow.begin(); itr_tmpwindow != tmpwindow.end(); itr_tmpwindow++)	//������ũž���� �ȿ� �ִ� ������ �ڵ� �����ͼ� ������ ���
				{
					tmpDesk->insertWindow((*itr_tmpwindow));
				}
			}
		}

	}
	else // ���ο� ����ũž�� �� �������
	{
		for (int i = pastvalue; i < newvalue; i++)
			desktopList.push_back(new E_Desktop(i));
	}
	std::list<E_Desktop*> desklist = e_global->desktopList;
	for (std::list<E_Desktop*>::iterator itr_desk = desklist.begin(); itr_desk != desklist.end(); itr_desk++)	//�� ����ũž �������
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
	WCHAR name3[] = L"��ƼĿ";
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