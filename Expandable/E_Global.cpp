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
	TRACE_WIN32A("E_Global::startUpdate()");
	if (currentThread == NULL && updateMode != true){
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
		Sleep(100);
}
}

// ������Ʈ ������// ��ȭ�� ��� ���� ��ȯ�Ѵ�.
bool E_Global::onUpdate()
{
	bool result = false;
	//TRACE_WIN32A("E_Global::onUpdate()");
	//���ؽ�
	//lock_guard<mutex> lockGuard(E_Mutex::updateMutex);//�����
	//������ ����Ʈ ������Ʈ
	list<HWND> wlist = getAllWindows();
	list<E_Window*> selectedWindows = selectedDesktop->getWindowList();
	//selectedDesktop->clearWindow();
	//������ �߰�

	Sleep(2000);
	//TRACE_WIN32A("E_Global::onUpdate ING...()");
	//����� �ٸ��ų� �������� �ٸ��ٸ�.. ������Ʈ ����
	if (wlist.size() != selectedWindows.size() || ((*wlist.rbegin()) != (*selectedWindows.rbegin())->getWindow())){
		//�ٲ� �����츸 ������Ʈ
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
	TRACE_WIN32A("E_Global::stopUpdate()");
	if (currentThread != NULL && updateMode == true){
		updateMode = false;
		currentThread->join();	//�����尡 ���������� ���
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
		targetWindow->setHide();
		selectedDesktop->excludeWindow(targetWindow);	//������ ����
	}
}

// ž �����츦 ������ ����ũž���� ���� (TEST OK)
void E_Global::moveTopWindowRight(){
	onUpdate();
	list<E_Window*> windowList = selectedDesktop->getWindowList();
	list<E_Window*>::reverse_iterator iter = windowList.rbegin();
	E_Window* targetWindow = *iter;
	
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
		desktop->insertWindow(targetWindow);	//������ �߰�
		targetWindow->setHide();//������ �����
		selectedDesktop->excludeWindow(targetWindow);	//������ ����
	}
}

// ž �����츦 �Ʒ��� ����ũž���� ����
void E_Global::moveTopWindowDown(){
	onUpdate();
	list<E_Window*> windowList = selectedDesktop->getWindowList();
	list<E_Window*>::reverse_iterator iter = windowList.rbegin();
	E_Window* targetWindow = *iter;

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
		targetWindow->setHide();
		selectedDesktop->excludeWindow(targetWindow);	//������ ����
	}
}


// ž �����츦 ���� ����ũž���� ����
void E_Global::moveTopWindowUp(){
	onUpdate();
	list<E_Window*> windowList = selectedDesktop->getWindowList();
	list<E_Window*>::reverse_iterator iter = windowList.rbegin();
	E_Window* targetWindow = *iter;

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
		targetWindow->setHide();
		selectedDesktop->excludeWindow(targetWindow);	//������ ����
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
		}
	}
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
		}
	}
}