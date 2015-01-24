// dllmain.cpp : DLL ���� ���α׷��� �������� �����մϴ�.
#include "stdafx.h"
#include <windows.h>
#include "E_IHooking.h"
#include "E_Util.h"
#include "E_DLLClient.h"
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
char* processName = NULL;
int pid = NULL;
char fileName[255] = { 0, };

//���μ��� ����Ʈ ����
#define PROCESSSIZE 4
static char processList[PROCESSSIZE][255] = { "chrome.exe", "iexplore.exe", "firefox.exe", "Safari.exe" };
static wchar_t wprocessList[PROCESSSIZE][255] = { L"chrome.exe", L"iexplore.exe",L"firefox.exe",L"Safari.exe" };
static const char* EXPLORER = "explorer.exe";
static const wchar_t* WEXPLORER = L"explorer.exe";

//���μ��� ���� ����Ʈ
#define EXCLUDESIZE 1
static char excludeList[EXCLUDESIZE][255] = { "dbgview.exe" };

#define TEST_EXECUTABLE
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	GetModuleFileNameA(NULL, fileName, 255);
	if (processName == NULL){
		processName = strrchr(fileName, '\\');
		processName = processName + 1;
		pid = GetCurrentProcessId();
		HWND hwnd = GetWindowFromPID(pid);
		TRACE_WIN32A("[DLLMain] %s", processName);
	}

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		TRACE_WIN32A("[Attach] DLL_PROCESS_ATTACH");
		for (int i = 0; i < EXCLUDESIZE; i++){
			if (processName != NULL
				&& !_stricmp(processName, excludeList[i])){
				return FALSE;
			}
		}
		//Gloal Hook�� Explorer ������ ���
		if (processName != NULL
			&& !_stricmp(processName, EXPLORER)
			&& globalhook == false){
			startGlobalHook();
			//}

			////ShowHook�� Explorer�� ������ ���� �������� ������ ���
			//if (processName != NULL
			//	&& _stricmp(processName, EXPLORER)
			//	&& showhook == false){
			//	startShowHook();
			//}

			break;
	case DLL_THREAD_ATTACH:
	{
							  TRACE_WIN32A("[Attach] DLL_THREAD_ATTACH");

							  //Interface Hook�� iexplore�� ���� �������� ������ ��ŷ
							  if ((interfacestatehook == false || interfacehook == false) && processName != NULL){
								  for (int i = 0; i < PROCESSSIZE; i++){
									  if (!_stricmp(processName, processList[i])
										  ){
										  DWORD current = GetCurrentProcessId();
										  DWORD parentPID = getppid(wprocessList[i], current);	//���� PID�� �θ�
										  DWORD explorerPID = GetProcessID(WEXPLORER);	//Explorer.exe
										  TRACE_WIN32(L"[PROCESS] target: %s, parent: %u, explorer: %u", wprocessList[i], parentPID, explorerPID);

										  if (interfacehook == false && isVisible(NULL)){// && parentPID == explorerPID
											  startInterfaceHook();	//ũ��, �ͽ��÷η� .. ��ŷ
										  }
										  if (interfacestatehook == false && isVisible(NULL)){ // && parentPID == explorerPID 
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
		if (processName != NULL
			&& !_stricmp(processName, EXPLORER)
			&& globalhook == true){
			stopGlobalHook();
		}

		//ShowHook�� Explorer�� ������ ���� �������� ������ ���
		if (processName != NULL
			&& _stricmp(processName, EXPLORER)
			&& showhook == true){
			stopShowHook();
		}

		if (processName != NULL){
			for (int i = 0; i < PROCESSSIZE; i++){
				if (!_stricmp(processName, processList[i])
					){
					if (interfacehook == true){
						stopInterfaceHook();	//ũ��, �ͽ��÷η� .. ��ŷ
					}
					if (interfacestatehook == true){
						stopInterfaceStateHook();
					}
				}
			}
		}
		break;
		}
	}
	return TRUE;
}



// NT32Debug-Executable configuration�� ���,
// WinMain�� �ּ�, ���డ���� DLL ���°� �ǵ��� �Ѵ�.
#ifdef TEST_EXECUTABLE
#include <stdio.h>
#include <stdlib.h>
#include "E_TestDLLClient.h"
//int __stdcall WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd)
//{
//}
int main(int argc, char* argv[]){
	//testWriteJSON();
	//testSocket();
	//printf("sizeof: %d ", sizeof(processList));
	//printf("sizeof: %d ", sizeof(excludeList));
	testConnectServer();
	return 0;

}
#endif // DEBUG_EXECUTABLE