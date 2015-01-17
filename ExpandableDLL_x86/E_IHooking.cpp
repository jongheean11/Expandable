#include "stdafx.h"
#include "E_IHooking.h"

bool showhook = false;
bool interfacehook = false;
bool interfacestatehook = false;
bool globalhook = false;

void startShowHook(){
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

void startInterfaceStateHook(){

	interfacestatehook = false;
	if (interfacestatehook == false && hook_by_hotpatch_SetProgressState()){
		OutputDebugStringA("[Success] hook_by_hotpatch_SetProgressState");
		interfacestatehook = true;
	}
	else
		OutputDebugStringA("[Fail] hook_by_hotpatch_SetProgressState");
}

void startGlobalHook(){
	// hook
	if (globalhook == false){
		if (hook_by_code("ntdll.dll", "ZwResumeThread",
			(PROC)NewZwResumeThread, g_pZWRT)){
			OutputDebugStringA("[Success] hook_by_ZwResumeThread");
			globalhook = true;
		}
		else{
			OutputDebugStringA("[Fail] hook_by_ZwResumeThread");
		}
	}
}
void stopShowHook(){
	if (showhook == true && unhook_by_hotpatch_ShowWindow()){
		OutputDebugStringA("[Success] unhook_by_hotpatch_ShowWindow");
		showhook = false;
	}
	else
		OutputDebugStringA("[Fail] unhook_by_hotpatch_ShowWindow");
}

void stopInterfaceHook(){
	if (interfacehook == true && unhook_by_hotpatch_SetProgressValue()){
		OutputDebugStringA("[Success] unhook_by_hotpatch_SetProgressValue");
		interfacehook = false;
	}
	else
		OutputDebugStringA("[Fail] unhook_by_hotpatch_SetProgressValue");
}

void stopGlobalHook(){
	// unhook
	if (globalhook == true){
		if (unhook_by_code("ntdll.dll", "ZwResumeThread", g_pZWRT)){
			OutputDebugStringA("[Success] unhook_by_ZwResumeThread");
			globalhook = false;
		}
		else{
			OutputDebugStringA("[Fail] unhook_by_ZwResumeThread");
		}
	}
}