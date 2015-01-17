// dllmain.cpp : DLL ���� ���α׷��� �������� �����մϴ�.
#include "stdafx.h"
#include <windows.h>
#include "E_IHooking.h"
#include "E_Util.h"
#include <thread>

//#if defined(_AMD64_)
//#pragma comment(lib, "..\\NetFX\\EasyHook64.lib")
//#else
//#pragma comment(lib, "..\\NetFX\\EasyHook32.lib")
//#endif

//BOOL WINAPI NShowWindow(
//	_In_  HWND hWnd,
//	_In_  int nCmdShow
//	){
//
//	TRACE_WIN32A("INSTALL HOOK");
//	return FALSE;
//}

/*
x86 ���
�ֽ� ���� ũ��
iexplore 8, 11
http://dev.mysql.com/downloads/file.php?id=454673
�׽�Ʈ �Ϸ�
*/

//��ŷ ���� ����
extern bool showhook;
extern bool interfacehook;
extern bool interfacestatehook;
extern bool globalhook;

//���μ��� ����Ʈ ����
#define PROCESSSIZE 2
static char processList[PROCESSSIZE][255] = { "chrome.exe", "iexplore.exe" };
static wchar_t wprocessList[PROCESSSIZE][255] = { L"chrome.exe", L"iexplore.exe" };
static const char* EXPLORER = "explorer.exe";
static const wchar_t* WEXPLORER = L"explorer.exe";

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{

	char fileName[255] = { 0, };
	GetModuleFileNameA(NULL, fileName, 255);
	char *name = strrchr(fileName, '\\');
	
	TRACE_WIN32A("[Process]%s", name + 1);

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		TRACE_WIN32A("[Attach] DLL_PROCESS_ATTACH");

		//Gloal Hook�� Explorer ������ ���
		if (name != NULL
			&& !_stricmp(name + 1, EXPLORER)
			&& globalhook == false){
			startGlobalHook();
		}

		//ShowHook�� Explorer�� ������ ���� �������� ������ ���
		if (name != NULL
			&& _stricmp(name + 1, EXPLORER)
			&& showhook == false){
			startShowHook();
		}

		break;
	case DLL_THREAD_ATTACH:
	{
		TRACE_WIN32A("[Attach] DLL_THREAD_ATTACH");
		
		//Interface Hook�� iexplore�� ���� �������� ������ ��ŷ
		if ((interfacestatehook == false || interfacehook == false) && name != NULL){
			for (int i = 0; i < PROCESSSIZE; i++){
				if (!_stricmp(name + 1, processList[i])
					){
					DWORD current = GetCurrentProcessId();
					DWORD parentPID = getppid(wprocessList[i], current);	//���� PID�� �θ�
					DWORD explorerPID = GetProcessID(WEXPLORER);	//Explorer.exe
					TRACE_WIN32(L"[PROCESS] target: %s, parent: %u, explorer: %u", wprocessList[i], parentPID, explorerPID);

					if (interfacehook == false && parentPID == explorerPID && isVisible(NULL)){
						startInterfaceHook();	//ũ��, �ͽ��÷η� .. ��ŷ
					}
					if (interfacestatehook == false && parentPID == explorerPID && isVisible(NULL)){
						startInterfaceStateHook(); //������ ����.. ��ŷ
					}

				}
			}
		}
	}
	break;
	case DLL_THREAD_DETACH:
		//OutputDebugStringA("[Attach] DLL_THREAD_DETACH");
		break;
	case DLL_PROCESS_DETACH:
		OutputDebugStringA("[Attach] DLL_PROCESS_DETACH");

		//Gloal Hook�� Explorer ������ ���
		if (name != NULL
			&& !_stricmp(name + 1, EXPLORER)
			&& globalhook == true){
			stopGlobalHook();
		}

		//ShowHook�� Explorer�� ������ ���� �������� ������ ���
		if (name != NULL
			&& _stricmp(name + 1, EXPLORER)
			&& showhook == true){
			stopShowHook();
		}

		if (name != NULL){
			for (int i = 0; i < PROCESSSIZE; i++){
				if (!_stricmp(name + 1, processList[i])
					&& interfacehook == true){
					stopInterfaceHook();	//ũ��, �ͽ��÷η� .. ��ŷ
				}
			}
		}
		break;
	}

	return TRUE;
}


