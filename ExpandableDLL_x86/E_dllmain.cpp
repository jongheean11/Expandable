// dllmain.cpp : DLL 응용 프로그램의 진입점을 정의합니다.
#include "stdafx.h"
#include <windows.h>
#include "E_Hooking.h"
#include "changlib.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		OutputDebugStringA("[Attach] ExpandableDLL");
		//MessageBoxA(NULL, "[Attach] ExpandableDLL", NULL, 0);
		//hook_by_hotpatch_ShowWindow();
		if (hook_by_hotpatch_SetProgressValue())
			OutputDebugStringA("[Success] hook_by_hotpatch_SetProgressValue");
		else
			OutputDebugStringA("[Fail] hook_by_hotpatch_SetProgressValue");

		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

