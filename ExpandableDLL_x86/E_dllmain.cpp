// dllmain.cpp : DLL 응용 프로그램의 진입점을 정의합니다.
#include "stdafx.h"
#include <windows.h>
#include "E_Hooking.h"
#include "changlib.h"
#include <thread>

//#if defined(_AMD64_)
//#pragma comment(lib, "..\\NetFX\\EasyHook64.lib")
//#else
//#pragma comment(lib, "..\\NetFX\\EasyHook32.lib")
//#endif
/*
최신 버전 크롬
iexplore 8
테스트 완료
*/

static bool isDetached = false;
static bool showhook = false;
static bool interfacehook = false;

BOOL WINAPI NShowWindow(
	_In_  HWND hWnd,
	_In_  int nCmdShow
	){

	TRACE_WIN32A("INSTALL HOOK");
	return FALSE;
}

void startHook(){
	//TRACE_WIN32A("BEFORE INSTALL HOOK");
	//LhInstallHook(ShowWindow, NShowWindow, NULL, NULL);
	//TRACE_WIN32A("AFTER INSTALL HOOK");
	if (showhook == false && hook_by_hotpatch_ShowWindow()){
		OutputDebugStringA("[Success] hook_by_hotpatch_ShowWindow");
		showhook = true;
	}
	else
		OutputDebugStringA("[Fail] hook_by_hotpatch_ShowWindow");
}

void startInterfaceHook(){
	if (interfacehook == false && hook_by_hotpatch_SetProgressValue()){
		OutputDebugStringA("[Success] hook_by_hotpatch_SetProgressValue");
		interfacehook = true;
	}
	else
		OutputDebugStringA("[Fail] hook_by_hotpatch_SetProgressValue");

}

void startGlobalHook(){
	// hook
	OutputDebugStringA("startGlobalHook");
	hook_by_code("ntdll.dll", "ZwResumeThread",
		(PROC)NewZwResumeThread, g_pZWRT);
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	char fileName[255] = { 0, };
	GetModuleFileNameA(NULL, fileName, 255);
	char *name = strrchr(fileName, '\\');

	TRACE_WIN32A("Full name is %s", fileName);
	TRACE_WIN32A("Short name is %s", name + 1);

	if ((name != NULL && !_stricmp(name + 1, "iexplore.exe"))
		|| (name != NULL && !_stricmp(name + 1, "chrome.exe")))
	{
		switch (ul_reason_for_call)
		{
		case DLL_PROCESS_ATTACH:
			TRACE_WIN32A("[Attach] DLL_PROCESS_ATTACH");
			startHook();
			startGlobalHook();
			break;
		case DLL_THREAD_ATTACH:
			OutputDebugStringA("[Attach] DLL_THREAD_ATTACH");
			startInterfaceHook();
			break;
		case DLL_THREAD_DETACH:
			OutputDebugStringA("[Attach] DLL_THREAD_DETACH");
		case DLL_PROCESS_DETACH:
			isDetached = true;
			OutputDebugStringA("[Attach] DLL_PROCESS_DETACH");
			break;
		}
	}
	return TRUE;
}


