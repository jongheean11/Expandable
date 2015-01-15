// dllmain.cpp : DLL ���� ���α׷��� �������� �����մϴ�.
#include "stdafx.h"
#include <windows.h>
#include "E_Hooking.h"
#include "changlib.h"
#include <thread>

/*
�ֽ� ���� ũ��
iexplore 8
�׽�Ʈ �Ϸ�
*/

static bool isDetached = false;
static bool showhook = false;
static bool interfacehook = false;

void startHook(){
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
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	void* pointer = (void*)hModule;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		TRACE_WIN32A("[Attach] DLL_PROCESS_ATTACH %x", pointer);
		startHook();
		break;
	case DLL_THREAD_ATTACH:
		OutputDebugStringA("[Attach] DLL_THREAD_ATTACH");
		if (isDetached== true)
			startInterfaceHook();
		break;
	case DLL_THREAD_DETACH:
		OutputDebugStringA("[Attach] DLL_THREAD_DETACH");
	case DLL_PROCESS_DETACH:
		isDetached = true;
		OutputDebugStringA("[Attach] DLL_PROCESS_DETACH");
		break;
	}
	return TRUE;
}


