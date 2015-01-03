#include "stdafx.h"
#include "E_EnvironmentManager.h"

E_EnvironmentManager* E_EnvironmentManager::singleton = NULL;
const wchar_t* E_EnvironmentManager::ExeFile = L"Expandable.exe";

E_EnvironmentManager::E_EnvironmentManager()
{
	this->width = GetSystemMetrics(SM_CXSCREEN);
	this->height = GetSystemMetrics(SM_CYSCREEN);
}

long E_EnvironmentManager::getWidth(){
	return width;
}
long E_EnvironmentManager::getHeight(){
	return height;
}
void E_EnvironmentManager::setGlobalUpdater(E_GlobalUpdater* updater){
	this->globalUpdater = updater;
}


E_EnvironmentManager::~E_EnvironmentManager()
{
}

// GlobalWindow에 해상도 변경을 알리는 함수
void E_EnvironmentManager::notifyChangeResolution()
{
	
}

// GlobalWindow에 듀얼모니터 상태 변화를 알리는 함수
void E_EnvironmentManager::notifyDualMonitorMode()
{
}

// 이미 동작하는 Expandable이 있는지??
bool E_EnvironmentManager::getRunningMainProcess()
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	DWORD pid = GetProcessId(GetModuleHandleA(NULL));
	if (hSnapshot)
	{
		PROCESSENTRY32 ProcessEntry32;
		BOOL bProcessFound;
		ProcessEntry32.dwSize = sizeof(PROCESSENTRY32);
		bProcessFound = Process32First(hSnapshot, &ProcessEntry32);
		while (bProcessFound)
		{
			/*WCHAR dbgmsg[9999];
			wsprintf(dbgmsg,L"%s [%d]\n", ProcessEntry32.szExeFile, ProcessEntry32.th32ProcessID);
			OutputDebugString(dbgmsg);*/
			if (wcscmp(ProcessEntry32.szExeFile, E_EnvironmentManager::ExeFile) == 0 && ProcessEntry32.th32ProcessID != pid){
				//중복 프로세스 존재
			}
			bProcessFound = Process32Next(hSnapshot, &ProcessEntry32);
		}
		CloseHandle(hSnapshot);
	}
	return 0;
}

// 동작하는 핫키 프로그램이 있는지?
bool E_EnvironmentManager::getRunningAutoHotkey()
{
	return 0;
}

// updater를 매개변수로 받는 싱글톤 생성함수
E_EnvironmentManager* E_EnvironmentManager::getSingleton()
{
	if (singleton == NULL) {
		singleton = new E_EnvironmentManager();
	}
	return singleton;
}
BEGIN_MESSAGE_MAP(E_EnvironmentManager, CWnd)
	ON_WM_CREATE()
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
END_MESSAGE_MAP()

/*
Dual Moniotor 및 해상도 변화 감지
*/
LRESULT E_EnvironmentManager::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{
	bool dualMonitorMode = false;
	//Some code for resolution...
	int monitorCount = GetSystemMetrics(SM_CMONITORS);
	if (monitorCount > 1) {
		dualMonitorMode = true;
	} else {
		dualMonitorMode = false;
	}

	double width = lParam & 0x0000ffff;
	double height = (lParam >> 16);

	if (dualMonitorMode == false) {
		//It's not about Dual Monitor
		//if (this->width != width || this->height != height) {
		//	//듀얼모니터가 아닐때만 동작 
		//	//해상도 변경 관련...
		//	//globalUpdater->OnChangeResolution(width, height);
		//}
		if (this->dualMonitorMode != dualMonitorMode){
			//이전 상태와 달라졌다면
			globalUpdater->OnDualMonitorMode(dualMonitorMode);
		}
	} else if(dualMonitorMode == true) {

		//It's about Dual Monitor
		if (this->dualMonitorMode != dualMonitorMode){
			//이전 상태와 달라졌다면
			globalUpdater->OnDualMonitorMode(dualMonitorMode);
		}
	}

	this->dualMonitorMode = dualMonitorMode;
	this->width = width;
	this->height = height;
	
	char dbgmsg[9999];
	sprintf(dbgmsg, "width: %lf, height: %lf. mode %d", width, height, dualMonitorMode);
	OutputDebugStringA(dbgmsg);
	
	return 0;
}

int E_EnvironmentManager::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}
