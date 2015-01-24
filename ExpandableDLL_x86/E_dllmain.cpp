// dllmain.cpp : DLL 응용 프로그램의 진입점을 정의합니다.
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
x86 기반
최신 버전 크롬
iexplore 8, 11
http://dev.mysql.com/downloads/file.php?id=454673
테스트 완료
*/

//후킹 제어 변수
extern bool showhook;
extern bool interfacehook;
extern bool interfacestatehook;
extern bool globalhook;
char* processName = NULL;
int pid = NULL;
char fileName[255] = { 0, };

//프로세스 리스트 변수
#define PROCESSSIZE 4
static char processList[PROCESSSIZE][255] = { "chrome.exe", "iexplore.exe", "firefox.exe", "Safari.exe" };
static wchar_t wprocessList[PROCESSSIZE][255] = { L"chrome.exe", L"iexplore.exe",L"firefox.exe",L"Safari.exe" };
static const char* EXPLORER = "explorer.exe";
static const wchar_t* WEXPLORER = L"explorer.exe";

//프로세스 예외 리스트
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
		//Gloal Hook은 Explorer 고유의 기능
		if (processName != NULL
			&& !_stricmp(processName, EXPLORER)
			&& globalhook == false){
			startGlobalHook();
			//}

			////ShowHook은 Explorer를 제외한 메인 윈도우의 고유의 기능
			//if (processName != NULL
			//	&& _stricmp(processName, EXPLORER)
			//	&& showhook == false){
			//	startShowHook();
			//}

			break;
	case DLL_THREAD_ATTACH:
	{
							  TRACE_WIN32A("[Attach] DLL_THREAD_ATTACH");

							  //Interface Hook은 iexplore와 메인 윈도우의 공간에 후킹
							  if ((interfacestatehook == false || interfacehook == false) && processName != NULL){
								  for (int i = 0; i < PROCESSSIZE; i++){
									  if (!_stricmp(processName, processList[i])
										  ){
										  DWORD current = GetCurrentProcessId();
										  DWORD parentPID = getppid(wprocessList[i], current);	//현재 PID의 부모
										  DWORD explorerPID = GetProcessID(WEXPLORER);	//Explorer.exe
										  TRACE_WIN32(L"[PROCESS] target: %s, parent: %u, explorer: %u", wprocessList[i], parentPID, explorerPID);

										  if (interfacehook == false && isVisible(NULL)){// && parentPID == explorerPID
											  startInterfaceHook();	//크롬, 익스플로러 .. 후킹
										  }
										  if (interfacestatehook == false && isVisible(NULL)){ // && parentPID == explorerPID 
											  startInterfaceStateHook(); //마지막 상태.. 후킹
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

		//Gloal Hook은 Explorer 고유의 기능
		if (processName != NULL
			&& !_stricmp(processName, EXPLORER)
			&& globalhook == true){
			stopGlobalHook();
		}

		//ShowHook은 Explorer를 제외한 메인 윈도우의 고유의 기능
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
						stopInterfaceHook();	//크롬, 익스플로러 .. 후킹
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



// NT32Debug-Executable configuration인 경우,
// WinMain을 둬서, 실행가능한 DLL 형태가 되도록 한다.
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