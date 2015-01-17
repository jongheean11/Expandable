#include "stdafx.h"
#include "E_Global.h"
#pragma once
#include <list>
#include "E_Map.h"

E_Global* E_Global::singleton = NULL;

const wchar_t* E_Global::testFrameName = L"expandable";
#define WM_TRAY_EVENT (WM_USER + 3)
E_Global::E_Global() : selectedDesktop(NULL), updateMode(false), currentThread(NULL)
{
	nowActiveNotify = 0;
	nextActiveNotify = 0;
	mapopen = false;
	hotkeyinvalidate = false;
	settingTimer = 5;
	transparent = 160;
	mapsize = 0.06;
	iconsize = 1;
	desktopwidth = 3;
	desktopheight = 3;
	//���� ������ �о�� ��
	desktopCount = desktopwidth*desktopheight;

	//����ũž ����
	for (int i = 0; i < desktopCount; i++)
		desktopList.push_back(new E_Desktop(i));

	//�ʱ� ����ũž 
	selectedDesktop = *(desktopList.begin());
	selectedIndex = 0;

	//������ ����Ʈ �ʱ�ȭ // ������ ���̶� �ʱ�ȭ �Ұ���
	//
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
	int length;
	::GetWindowText(hwnd, Cap, 254);
	length = ::GetWindowTextLength(hwnd);

	if (IsWindowVisible(hwnd))
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
	lock_guard<mutex> start(E_Mutex::updateStartStopMutex);
	if (currentThread == NULL && updateMode != true){
		TRACE_WIN32A("E_Global::startUpdate()");
		updateMode = true;
		thread* t = new thread{ &E_Global::loopUpdate, this };
		currentThread = t;
		return true;
	}
	return false;
}

// �ǽð����� ������ ������Ʈ
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
		Sleep(200);
	}
}

// ������Ʈ ������// ��ȭ�� ��� ���� ��ȯ�Ѵ�.
bool E_Global::onUpdate()
{
	bool result = false;
	//������ ����Ʈ ������Ʈ
	list<HWND> wlist = getAllWindows();
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
		lock_guard<mutex> lockGuard(E_Mutex::updateMutex);
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
	lock_guard<mutex> stop(E_Mutex::updateStartStopMutex);
	if (currentThread != NULL && updateMode == true){
		TRACE_WIN32A("E_Global::stopUpdate()");
		updateMode = false;
		currentThread->join();	//�����尡 ���������� ���
		currentThread->detach();
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

// ž �����츦 ������ ����ũž���� ���� (TEST OK)
void E_Global::moveTopWindowRight(){
	onUpdate();
	list<E_Window*> windowList = selectedDesktop->getWindowList();
	list<E_Window*>::reverse_iterator iter = windowList.rbegin();
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

// ž �����츦 �Ʒ��� ����ũž���� ����
void E_Global::moveTopWindowDown(){
	onUpdate();
	list<E_Window*> windowList = selectedDesktop->getWindowList();
	list<E_Window*>::reverse_iterator iter = windowList.rbegin();
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


// ž �����츦 ���� ����ũž���� ����
void E_Global::moveTopWindowUp(){
	onUpdate();
	list<E_Window*> windowList = selectedDesktop->getWindowList();
	list<E_Window*>::reverse_iterator iter = windowList.rbegin();
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
		}
	}
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

		int index = selectedIndex + 1;
		E_Desktop* last = getDesktop(index);
		if (last != NULL){
			selectedDesktop->setAllHide();//����
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