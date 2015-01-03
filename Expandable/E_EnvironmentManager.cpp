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

// GlobalWindow�� �ػ� ������ �˸��� �Լ�
void E_EnvironmentManager::notifyChangeResolution()
{
	
}

// GlobalWindow�� ������� ���� ��ȭ�� �˸��� �Լ�
void E_EnvironmentManager::notifyDualMonitorMode()
{
}

// �̹� �����ϴ� Expandable�� �ִ���??
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
				//�ߺ� ���μ��� ����
			}
			bProcessFound = Process32Next(hSnapshot, &ProcessEntry32);
		}
		CloseHandle(hSnapshot);
	}
	return 0;
}

// �����ϴ� ��Ű ���α׷��� �ִ���?
bool E_EnvironmentManager::getRunningAutoHotkey()
{
	return 0;
}

// updater�� �Ű������� �޴� �̱��� �����Լ�
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
Dual Moniotor �� �ػ� ��ȭ ����
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
		//	//������Ͱ� �ƴҶ��� ���� 
		//	//�ػ� ���� ����...
		//	//globalUpdater->OnChangeResolution(width, height);
		//}
		if (this->dualMonitorMode != dualMonitorMode){
			//���� ���¿� �޶����ٸ�
			globalUpdater->OnDualMonitorMode(dualMonitorMode);
		}
	} else if(dualMonitorMode == true) {

		//It's about Dual Monitor
		if (this->dualMonitorMode != dualMonitorMode){
			//���� ���¿� �޶����ٸ�
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

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.

	return 0;
}
