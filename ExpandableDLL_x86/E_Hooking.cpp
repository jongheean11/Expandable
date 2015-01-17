#include "stdafx.h"
#include "E_Hooking.h"

BYTE g_pZWRT[5] = { 0, };
BYTE g_pICW[5] = { 0, };

//5바이트 ShowWindow를 위한 핫 패치 7652F2A9
BOOL hook_by_hotpatch_ShowWindow() {
	PROC pShowWindowHook = (PROC)ShowWindowHook;
	FARPROC pShowWindow = NULL;
	LPCSTR dllName = "user32.dll";
	LPCSTR funcName = "ShowWindow";
	DWORD dwOldProtect;
	BYTE pJumpCode[5] = { 0xE9, 0, };
	BYTE pShortJump[5] = { 0xEB, 0xF9, 0x90, 0x90, 0x90 };
	BYTE pOriginalCodeShowWindow[5] = { 0xB8, 0x4F, 0x12, 0x00, 0x00 };

	//1)원래 함수 주소 저장
	pShowWindow = (FARPROC)ShowWindow;

	TRACE_WIN32A("[CALL] hook_by_hotpatch_ShowWindow [%x]", (void*)pShowWindow);

	if (((BYTE*)pShowWindow)[0] == 0xEB){
		TRACE_WIN32A("[hook_by_hotpatch_ShowWindow] Already hotpatch", (void*)pShowWindow);
		return FALSE;
	}

	//2)읽기 쓰기 권한 부여
	if (!VirtualProtect((LPVOID)((DWORD)pShowWindow - 5), 10, PAGE_EXECUTE_READWRITE, &dwOldProtect)){
		TRACE_WIN32A("[hook_by_hotpatch_ShowWindow] VirtualProtect 실패");
		return FALSE;
	}

	//3)Short Jump 코드 복사(hot fix
	memcpy(pShowWindow, pShortJump, 5);

	//3)Long Jump 코드 작성
	DWORD relativeAddress = (DWORD)pShowWindowHook - (DWORD)pShowWindow; //ShowWindowHook와 ShowWindow API 상대 주소
	memcpy(pJumpCode + 1, &relativeAddress, 4);

	//4)Long Jump 코드 복사 (hot fix)
	memcpy((LPVOID)((DWORD)pShowWindow - 5), pJumpCode, 5);

	//5)원래의 권한으로 수정
	VirtualProtect((LPVOID)((DWORD)pShowWindow - 5), 10, dwOldProtect, &dwOldProtect);
	return TRUE;
}

//5바이트 ShowWindow를 위한 핫 패치 복원
BOOL unhook_by_hotpatch_ShowWindow()
{
	OutputDebugStringA("[CALL] unhook_by_hotpatch_ShowWindow");

	PROC pShowWindowHook = (PROC)ShowWindowHook;
	FARPROC pShowWindow = NULL;
	LPCSTR dllName = "user32.dll";
	LPCSTR funcName = "ShowWindow";
	DWORD dwOldProtect;
	BYTE pOriginalJumpCode[5] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
	BYTE pOriginalCodeShowWindow[5] = { 0xB8, 0x4F, 0x12, 0x00, 0x00 };

	//1)원래 함수 주소 저장
	pShowWindow = GetProcAddress(GetModuleHandleA(dllName), funcName);

	//2)읽기 쓰기 권한 부여
	VirtualProtect((LPVOID)((DWORD)pShowWindow - 5), 10, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	//3)Short Jump 코드 복원(hot fix)
	memcpy(pShowWindow, pOriginalCodeShowWindow, 5);

	//4)Long Jump 코드 복원 (hot fix)
	memcpy((LPVOID)((DWORD)pShowWindow - 5), pOriginalJumpCode, 5);

	//5)원래의 권한으로 수정
	VirtualProtect((LPVOID)((DWORD)pShowWindow - 5), 10, dwOldProtect, &dwOldProtect);
	return TRUE;
}

//7바이트 SetProgressValue를 위한 핫 패치 (인터페이스)
BOOL hook_by_hotpatch_SetProgressState()
{
	OutputDebugStringA("[CALL] hook_by_hotpatch_SetProgressState");
	BOOL result = TRUE;
	HRESULT hresult = NULL;
	DWORD* ptr;
	FARPROC pfSetProgressState = NULL;;
	ITaskbarList3 *pTaskList3 = NULL;

	hresult = CoInitialize(NULL);
	//1)인터페이스 생성
	if (SUCCEEDED(hresult)) {
		hresult = CoCreateInstance(CLSID_TaskbarList, NULL,
			CLSCTX_SERVER, IID_ITaskbarList3,
			(LPVOID *)&pTaskList3);
	}
	else {
		return FALSE;
	}

	if (SUCCEEDED(hresult)) {
		ptr = (DWORD*)pTaskList3;
		ptr = (DWORD*)*ptr;
		ptr = (DWORD*)*(ptr + 10); // 0x28 == 40
		pfSetProgressState = (FARPROC)ptr;
		//char dbgmsg[255] = { 0, };
		//sprintf_s(dbgmsg, "ptr %x", (DWORD)ptr);
		//OutputDebugStringA(dbgmsg);
		//pTaskList3->Release();
	}
	else {
		pTaskList3->Release();
		return FALSE;
	}
	//2)후킹 (PVOID)(&(PVOID&)HookingClass::HookingFu
	if (!hook_by_hotpatch(pfSetProgressState, (PROC)((DWORD)SetProgressStateHook))) {
		pTaskList3->Release();
		return FALSE;
	}

	//3)SetProgresssValue 함수주소 유지를 위해 인터페이스 객체를 해제하지 않는다.
	//pTaskList3->Release();

	return result;
}

//7바이트 SetProgressValue를 위한 핫 패치 (인터페이스)
BOOL hook_by_hotpatch_SetProgressValue()
{
	OutputDebugStringA("[CALL] hook_by_hotpatch_SetProgressValue");
	BOOL result = TRUE;
	HRESULT hresult = NULL;
	DWORD* ptr;
	FARPROC pfSetProgressValue = NULL;;
	ITaskbarList3 *pTaskList3 = NULL;

	hresult = CoInitialize(NULL);
	//1)인터페이스 생성
	if (SUCCEEDED(hresult)) {
		hresult = CoCreateInstance(CLSID_TaskbarList, NULL,
			CLSCTX_SERVER, IID_ITaskbarList3,
			(LPVOID *)&pTaskList3);
	}
	else {
		return FALSE;
	}
	if (SUCCEEDED(hresult)) {
		ptr = (DWORD*)pTaskList3;
		ptr = (DWORD*)*ptr;
		ptr = (DWORD*)*(ptr + 9); // 0x24 == 36
		pfSetProgressValue = (FARPROC)ptr;
		//char dbgmsg[255] = { 0, };
		//sprintf_s(dbgmsg, "ptr %x", (DWORD)ptr);
		//OutputDebugStringA(dbgmsg); 
		//pTaskList3->Release();
	}
	else {
		pTaskList3->Release();
		return FALSE;
	}
	//2)후킹 (PVOID)(&(PVOID&)HookingClass::HookingFu
	if (!hook_by_hotpatch(pfSetProgressValue, (PROC)((DWORD)SetProgressValueHook))) {
		pTaskList3->Release();
		return FALSE;
	}

	//3)SetProgresssValue 함수주소 유지를 위해 인터페이스 객체를 해제하지 않는다.
	//pTaskList3->Release();

	return result;
}

BOOL unhook_by_hotpatch_SetProgressValue()
{
	OutputDebugStringA("[CALL] unhook_by_hotpatch_SetProgressValue");
	BOOL result = TRUE;
	HRESULT hresult = NULL;
	DWORD* ptr;
	FARPROC pfSetProgressValue = NULL;;
	ITaskbarList3 *pTaskList3 = NULL;

	hresult = CoInitialize(NULL);
	//1)인터페이스 생성
	if (SUCCEEDED(hresult)) {
		hresult = CoCreateInstance(CLSID_TaskbarList, NULL,
			CLSCTX_SERVER, IID_ITaskbarList3,
			(LPVOID *)&pTaskList3);
	}
	else {
		return FALSE;
	}

	if (SUCCEEDED(hresult)) {
		ptr = (DWORD*)pTaskList3;
		ptr = (DWORD*)*ptr;
		ptr = (DWORD*)*(ptr + 9);
		pfSetProgressValue = (FARPROC)ptr;
		//char dbgmsg[255] = { 0, };
		//sprintf_s(dbgmsg, "ptr %x", (DWORD)ptr);
		//OutputDebugStringA(dbgmsg);
		//pTaskList3->Release();
	}
	else {
		pTaskList3->Release();
		return FALSE;
	}

	//2)언후킹
	if (!unhook_by_hotpatch(pfSetProgressValue)){
		pTaskList3->Release();
		return FALSE;
	}

	pTaskList3->Release();
	return TRUE;
}

//7바이트 핫 패치 복원
BOOL unhook_by_hotpatch(LPCSTR szDllName, LPCSTR szFuncName)
{
	FARPROC pFunc;
	DWORD dwOldProtect;
	PBYTE pByte;
	BYTE pBuf[5] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
	BYTE pBuf2[2] = { 0x8B, 0xFF };


	pFunc = (FARPROC)GetProcAddress(GetModuleHandleA(szDllName), szFuncName);
	pByte = (PBYTE)pFunc;
	if (pByte[0] != 0xEB)
		return FALSE;

	VirtualProtect((LPVOID)pFunc, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	// 1. NOP (0x90)
	memcpy((LPVOID)((DWORD)pFunc - 5), pBuf, 5);

	// 2. MOV EDI, EDI (0x8BFF)
	memcpy(pFunc, pBuf2, 2);

	VirtualProtect((LPVOID)pFunc, 5, dwOldProtect, &dwOldProtect);

	return TRUE;
}
//7바이트 핫 패치
BOOL unhook_by_hotpatch(FARPROC pFunc)
{
	DWORD dwOldProtect;
	PBYTE pByte;
	BYTE pBuf[5] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
	BYTE pBuf2[2] = { 0x8B, 0xFF };

	pByte = (PBYTE)pFunc;
	if (pByte[0] != 0xEB)
		return FALSE;

	VirtualProtect((LPVOID)pFunc, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	// 1. NOP (0x90)
	memcpy((LPVOID)((DWORD)pFunc - 5), pBuf, 5);

	// 2. MOV EDI, EDI (0x8BFF)
	memcpy(pFunc, pBuf2, 2);

	VirtualProtect((LPVOID)pFunc, 5, dwOldProtect, &dwOldProtect);

	return TRUE;
}

//7바이트 핫 패치
BOOL hook_by_hotpatch(LPCSTR szDllName, LPCSTR szFuncName, PROC pfnNew)
{
	FARPROC pFunc;
	DWORD dwOldProtect, dwAddress;
	BYTE pBuf[5] = { 0xE9, 0, };
	BYTE pBuf2[2] = { 0xEB, 0xF9 };
	PBYTE pByte;

	pFunc = (FARPROC)GetProcAddress(GetModuleHandleA(szDllName), szFuncName);
	pByte = (PBYTE)pFunc;
	if (pByte[0] == 0xEB){
		return FALSE;
	}

	VirtualProtect((LPVOID)((DWORD)pFunc - 5), 7, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	// 1. NOP (0x90)
	dwAddress = (DWORD)pfnNew - (DWORD)pFunc;
	memcpy(&pBuf[1], &dwAddress, 4);
	memcpy((LPVOID)((DWORD)pFunc - 5), pBuf, 5);

	// 2. MOV EDI, EDI (0x8BFF)
	memcpy(pFunc, pBuf2, 2);

	VirtualProtect((LPVOID)((DWORD)pFunc - 5), 7, dwOldProtect, &dwOldProtect);

	return TRUE;
}

//7바이트 핫 패치
BOOL hook_by_hotpatch(FARPROC pFunc, PROC pfnNew)
{
	DWORD dwOldProtect, dwAddress;
	BYTE pBuf[5] = { 0xE9, 0, };
	BYTE pBuf2[2] = { 0xEB, 0xF9 };
	PBYTE pByte;

	pByte = (PBYTE)pFunc;

	//이미 후킹 되었으면
	if (pByte[0] == 0xEB){
		TRACE_WIN32A("[hook_by_hotpatch] Already hotpatch");
		return FALSE;
	}

	VirtualProtect((LPVOID)((DWORD)pFunc - 5), 7, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	// 1. NOP (0x90)
	dwAddress = (DWORD)pfnNew - (DWORD)pFunc;
	memcpy(&pBuf[1], &dwAddress, 4);
	memcpy((LPVOID)((DWORD)pFunc - 5), pBuf, 5);

	// 2. MOV EDI, EDI (0x8BFF)
	memcpy(pFunc, pBuf2, 2);

	VirtualProtect((LPVOID)((DWORD)pFunc - 5), 7, dwOldProtect, &dwOldProtect);

	return TRUE;
}



BOOL hook_by_code(LPCSTR szDllName, LPCSTR szFuncName, PROC pfnNew, PBYTE pOrgBytes)
{
	FARPROC pFunc = NULL;
	DWORD dwOldProtect = 0, dwAddress = 0;
	BYTE pBuf[5] = { 0xE9, 0, };
	PBYTE pByte = NULL;
	HMODULE hMod = NULL;

	hMod = GetModuleHandleA(szDllName);
	if (hMod == NULL)
	{
		TRACE_WIN32A("hook_by_code() : GetModuleHandle(\"%s\") failed!!! [%d]\n",
			szDllName, GetLastError());
		return FALSE;
	}

	pFunc = (FARPROC)GetProcAddress(hMod, szFuncName);
	if (pFunc == NULL)
	{
		TRACE_WIN32A("hook_by_code() : GetProcAddress(\"%s\") failed!!! [%d]\n",
			szFuncName, GetLastError());
		return FALSE;
	}

	pByte = (PBYTE)pFunc;
	if (pByte[0] == 0xE9)
	{
		TRACE_WIN32A("hook_by_code() : The API is hooked already!!!\n");
		return FALSE;
	}

	if (!VirtualProtect((LPVOID)pFunc, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		TRACE_WIN32A("hook_by_code() : VirtualProtect(#1) failed!!! [%d]\n", GetLastError());
		return FALSE;
	}

	memcpy(pOrgBytes, pFunc, 5);

	dwAddress = (DWORD)pfnNew - (DWORD)pFunc - 5;
	memcpy(&pBuf[1], &dwAddress, 4);

	memcpy(pFunc, pBuf, 5);

	if (!VirtualProtect((LPVOID)pFunc, 5, dwOldProtect, &dwOldProtect))
	{
		TRACE_WIN32A("hook_by_code() : VirtualProtect(#2) failed!!! [%d]\n", GetLastError());
		return FALSE;
	}

	return TRUE;
}

BOOL unhook_by_code(LPCSTR szDllName, LPCSTR szFuncName, PBYTE pOrgBytes)
{
	FARPROC pFunc = NULL;
	DWORD dwOldProtect = 0;
	PBYTE pByte = NULL;
	HMODULE hMod = NULL;

	hMod = GetModuleHandleA(szDllName);
	if (hMod == NULL)
	{
		TRACE_WIN32A("unhook_by_code() : GetModuleHandle(\"%s\") failed!!! [%d]\n",
			szDllName, GetLastError());
		return FALSE;
	}

	pFunc = (FARPROC)GetProcAddress(hMod, szFuncName);
	if (pFunc == NULL)
	{
		TRACE_WIN32A("unhook_by_code() : GetProcAddress(\"%s\") failed!!! [%d]\n",
			szFuncName, GetLastError());
		return FALSE;
	}

	pByte = (PBYTE)pFunc;
	if (pByte[0] != 0xE9)
	{
		TRACE_WIN32A("unhook_by_code() : The API is unhooked already!!!");
		return FALSE;
	}

	if (!VirtualProtect((LPVOID)pFunc, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		TRACE_WIN32A("unhook_by_code() : VirtualProtect(#1) failed!!! [%d]\n", GetLastError());
		return FALSE;
	}

	memcpy(pFunc, pOrgBytes, 5);

	if (!VirtualProtect((LPVOID)pFunc, 5, dwOldProtect, &dwOldProtect))
	{
		TRACE_WIN32A("unhook_by_code() : VirtualProtect(#2) failed!!! [%d]\n", GetLastError());
		return FALSE;
	}

	return TRUE;
}


BOOL MyCreateRemoteThread(HANDLE hProcess, LPTHREAD_START_ROUTINE pThreadProc, LPVOID pRemoteBuf)
{
	HANDLE      hThread = NULL;
	FARPROC     pFunc = NULL;

	if (IsVistaLater())    // Vista, 7, Server2008
	{
		pFunc = GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtCreateThreadEx");
		if (pFunc == NULL)
		{
			TRACE_WIN32A("MyCreateRemoteThread() : GetProcAddress() failed!!! [%d]\n",
				GetLastError());
			return FALSE;
		}

		((PFNTCREATETHREADEX)pFunc)(&hThread,
			0x1FFFFF,
			NULL,
			hProcess,
			pThreadProc,
			pRemoteBuf,
			FALSE,
			NULL,
			NULL,
			NULL,
			NULL);
		if (hThread == NULL)
		{
			TRACE_WIN32A("MyCreateRemoteThread() : NtCreateThreadEx() failed!!! [%d]\n", GetLastError());
			return FALSE;
		}
	}
	else                    // 2000, XP, Server2003
	{
		hThread = CreateRemoteThread(hProcess, NULL, 0,
			pThreadProc, pRemoteBuf, 0, NULL);
		if (hThread == NULL)
		{
			TRACE_WIN32A("MyCreateRemoteThread() : CreateRemoteThread() failed!!! [%d]\n", GetLastError());
			return FALSE;
		}
	}

	if (WAIT_FAILED == WaitForSingleObject(hThread, INFINITE))
	{
		TRACE_WIN32A("MyCreateRemoteThread() : WaitForSingleObject() failed!!! [%d]\n", GetLastError());
		return FALSE;
	}

	return TRUE;
}


BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath)
{
	HANDLE                  hProcess = NULL;
	HANDLE                  hThread = NULL;
	LPVOID                  pRemoteBuf = NULL;
	DWORD                   dwBufSize = (DWORD)(wcslen(szDllPath) + 1) * sizeof(TCHAR);
	LPTHREAD_START_ROUTINE  pThreadProc = NULL;
	BOOL                    bRet = FALSE;
	HMODULE                 hMod = NULL;

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
	{
		TRACE_WIN32A("InjectDll() : OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());
		goto INJECTDLL_EXIT;
	}

	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize,
		MEM_COMMIT, PAGE_READWRITE);
	if (pRemoteBuf == NULL)
	{
		TRACE_WIN32A("InjectDll() : VirtualAllocEx() failed!!! [%d]\n", GetLastError());
		goto INJECTDLL_EXIT;
	}

	if (!WriteProcessMemory(hProcess, pRemoteBuf,
		(LPVOID)szDllPath, dwBufSize, NULL))
	{
		TRACE_WIN32A("InjectDll() : WriteProcessMemory() failed!!! [%d]\n", GetLastError());
		goto INJECTDLL_EXIT;
	}

	hMod = GetModuleHandle(L"kernel32.dll");
	if (hMod == NULL)
	{
		TRACE_WIN32A("InjectDll() : GetModuleHandle() failed!!! [%d]\n", GetLastError());
		goto INJECTDLL_EXIT;
	}

	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");
	if (pThreadProc == NULL)
	{
		TRACE_WIN32A("InjectDll() : GetProcAddress() failed!!! [%d]\n", GetLastError());
		goto INJECTDLL_EXIT;
	}

	if (!MyCreateRemoteThread(hProcess, pThreadProc, pRemoteBuf))
	{
		TRACE_WIN32A("InjectDll() : MyCreateRemoteThread() failed!!!\n");
		goto INJECTDLL_EXIT;
	}

	bRet = TRUE;

INJECTDLL_EXIT:

	if (pRemoteBuf)
		VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);

	if (hThread)
		CloseHandle(hThread);

	if (hProcess)
		CloseHandle(hProcess);

	return bRet;
}

BOOL IsVistaLater()
{
	OSVERSIONINFO osvi;

	/*ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);

	if (osvi.dwMajorVersion >= 6)
	return TRUE;*/

	return TRUE;
	//return FALSE;
}


BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES tp;
	HANDLE hToken;
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&hToken))
	{
		TRACE_WIN32A("OpenProcessToken error: %u\n", GetLastError());
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL,             // lookup privilege on local system
		lpszPrivilege,    // privilege to lookup 
		&luid))          // receives LUID of privilege
	{
		TRACE_WIN32A("LookupPrivilegeValue error: %u\n", GetLastError());
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.
	if (!AdjustTokenPrivileges(hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL))
	{
		TRACE_WIN32A("AdjustTokenPrivileges error: %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		TRACE_WIN32A("The token does not have the specified privilege. \n");
		return FALSE;
	}

	return TRUE;
}

NTSTATUS WINAPI NewZwResumeThread(HANDLE ThreadHandle, PULONG SuspendCount)
{
	NTSTATUS status, statusThread;
	FARPROC pFunc = NULL, pFuncThread = NULL;
	DWORD dwPID = 0;
	static DWORD dwPrevPID = 0;
	THREAD_BASIC_INFORMATION tbi;
	HMODULE hMod = NULL;
	TCHAR szModPath[MAX_PATH] = { 0, };
	if (processName != NULL)
	TRACE_WIN32A("[%s] NewZwResumeThread", processName);

	hMod = GetModuleHandle(L"ntdll.dll");
	if (hMod == NULL)
	{
		TRACE_WIN32A("NewZwResumeThread() : GetModuleHandle() failed!!! [%d]\n",
			GetLastError());
		return NULL;
	}

	// call ntdll!ZwQueryInformationThread()
	pFuncThread = GetProcAddress(hMod, "ZwQueryInformationThread");
	if (pFuncThread == NULL)
	{
		TRACE_WIN32A("NewZwResumeThread() : GetProcAddress() failed!!! [%d]\n",
			GetLastError());
		return NULL;
	}

	statusThread = ((PFZWQUERYINFORMATIONTHREAD)pFuncThread)
		(ThreadHandle, 0, &tbi, sizeof(tbi), NULL);
	if (statusThread != STATUS_SUCCESS)
	{
		TRACE_WIN32A("NewZwResumeThread() : pFuncThread() failed!!! [%d]\n",
			GetLastError());
		return NULL;
	}

	dwPID = (DWORD)tbi.ClientId.UniqueProcess;
	if ((dwPID != GetCurrentProcessId()) && (dwPID != dwPrevPID))
	{
		TRACE_WIN32A("NewZwResumeThread() => call InjectDll()\n");

		dwPrevPID = dwPID;

		// change privilege
		if (!SetPrivilege(SE_DEBUG_NAME, TRUE))
			TRACE_WIN32A("NewZwResumeThread() : SetPrivilege() failed!!!\n");

		// get injection dll path
		GetModuleFileName(GetModuleHandleA("ExpandableDLL_x86.dll"),
			szModPath,
			MAX_PATH);

		if (!InjectDll(dwPID, szModPath))
			TRACE_WIN32A("NewZwResumeThread() : InjectDll(%d) failed!!!\n", dwPID);
	}

	// call ntdll!ZwResumeThread()
	if (!unhook_by_code("ntdll.dll", "ZwResumeThread", g_pZWRT))
	{
		TRACE_WIN32A("NewZwResumeThread() : unhook_by_code() failed!!!\n");
		return NULL;
	}

	pFunc = GetProcAddress(hMod, "ZwResumeThread");
	if (pFunc == NULL)
	{
		TRACE_WIN32A("NewZwResumeThread() : GetProcAddress() failed!!! [%d]\n",
			GetLastError());
		goto __NTRESUMETHREAD_END;
	}

	status = ((PFZWRESUMETHREAD)pFunc)(ThreadHandle, SuspendCount);
	if (status != STATUS_SUCCESS)
	{
		TRACE_WIN32A("NewZwResumeThread() : pFunc() failed!!! [%d]\n", GetLastError());
		goto __NTRESUMETHREAD_END;
	}

__NTRESUMETHREAD_END:

	if (!hook_by_code("ntdll.dll", "ZwResumeThread",
		(PROC)NewZwResumeThread, g_pZWRT))
	{
		TRACE_WIN32A("NewZwResumeThread() : hook_by_code() failed!!!\n");
	}

	TRACE_WIN32A("NewZwResumeThread() : end!!!\n");

	return status;
}

//후킹ShowWindow API
BOOL WINAPI ShowWindowHook(
	_In_  HWND hWnd,
	_In_  int nCmdShow){
	if (processName != NULL)
		TRACE_WIN32A("[%s] ShowWindowHook (%u, %d)", processName, hWnd, nCmdShow);
	//nCmdShow = SW_HIDE;

	FARPROC pShowWindow = NULL;
	BOOL result = FALSE;


	//1) original API 호출 (명령의 길이를 고려하여 2바이트 이후 부터 호출)
	pShowWindow = GetProcAddress(GetModuleHandleA("user32.dll"), "ShowWindow");
	pShowWindow = (FARPROC)((DWORD)pShowWindow + 2);

	//2) inline asm
	__asm
	{
			MOV EAX, 0x124F; BOOL USER32.ShowWindow(hWnd, Show); 후킹 전 ShowWindow 5바이트 명령 실행

			PUSH DWORD PTR SS : [EBP + 0x0C]; _In_  int nCmdShow
			PUSH DWORD PTR SS : [EBP + 0x8]; _In_  HWND hWnd
			CALL pShowWindow; ShowWIndow 호출
	}

	return result;
}

//후킹SetProgressValueHook
HRESULT WINAPI SetProgressValueHook(_In_ void* thisPtr,
	_In_  HWND hwnd,
	_In_  ULONGLONG ullCompleted,
	_In_  ULONGLONG ullTotal)
{
	if (processName != NULL)
	TRACE_WIN32A("[%s] SetProgressValueHook (%I64u, %I64u)", processName, ullCompleted, ullTotal);

	PFSETPROGRESSVALUE pfProgressValue;
	ITaskbarList3* pTaskList3 = NULL;

	HRESULT hresult = CoCreateInstance(CLSID_TaskbarList, NULL,
		CLSCTX_SERVER, IID_ITaskbarList3,
		(LPVOID *)&pTaskList3);

	if (SUCCEEDED(hresult)) {

		//hresult = pTaskList3->SetProgressValue(hwnd, ullCompleted, ullTotal);
		//	pTaskList3->Release();

		// 실제 인터페이스 위치 가져오기 위한
		__asm
		{
			PUSH DWORD PTR SS : [EBP + 0x1C]
				PUSH	DWORD PTR SS : [EBP + 0x18]
				PUSH	DWORD PTR SS : [EBP + 0x14]
				PUSH	DWORD PTR SS : [EBP + 0x10]
				PUSH DWORD PTR SS : [EBP + 0x0C]
				MOV EDX, DWORD PTR SS : [EBP - 0x0C]
				MOV EAX, DWORD PTR DS : [EDX]
				MOV ECX, DWORD PTR SS : [EBP - 0x0C]
				PUSH ECX
				MOV EDX, DWORD PTR DS : [EAX + 0x24]
				ADD EDX, 2; 숏 점프+2	-	실제SetProgressValue 함수 호출
				CALL EDX;	
		}
		pTaskList3->Release();
	}
	return hresult;
}

//후킹SetProgressValueHook
HRESULT WINAPI SetProgressStateHook(_In_ void* thisPtr,
	HWND hwnd,
	TBPFLAG tbpFlags
	)
{
	if (processName != NULL)
	TRACE_WIN32A("[%s] SetProgressStateHook (%d, %d)", processName, hwnd, tbpFlags);

	PFSETPROGRESSSTATE pfProgressState;
	ITaskbarList3* pTaskList3 = NULL;

	HRESULT hresult = CoCreateInstance(CLSID_TaskbarList, NULL,
		CLSCTX_SERVER, IID_ITaskbarList3,
		(LPVOID *)&pTaskList3);

	if (SUCCEEDED(hresult)) {
		//hresult = pTaskList3->SetProgressState(hwnd, TBPF_NOPROGRESS);
		//	pTaskList3->Release();

		//실제 함수의 +2를 호출하기 위한 코드
		__asm
		{
			PUSH	DWORD PTR SS : [EBP + 0x10]
				PUSH DWORD PTR SS : [EBP + 0x0C]
				MOV EDX, DWORD PTR SS : [EBP - 0x0c]
				MOV EAX, DWORD PTR DS : [EDX]
				MOV ECX, DWORD PTR SS : [EBP - 0x0c]
				PUSH ECX
				MOV EDX, DWORD PTR DS : [EAX + 0x28]
				ADD EDX, 2; 숏 점프 + 2 - 실제SetProgressState 함수 호출
				CALL EDX;
		}
		pTaskList3->Release();
	}
	return hresult;
}

//CPU Disasm
//Address   Hex dump          Command                                  Comments
//633814AB | .  8B45 1C       MOV EAX, DWORD PTR SS : [EBP + 1C]
//633814AE | .  50            PUSH EAX
//633814AF | .  8B4D 18       MOV ECX, DWORD PTR SS : [EBP + 18]
//633814B2 | .  51            PUSH ECX
//633814B3 | .  8B55 14       MOV EDX, DWORD PTR SS : [EBP + 14]
//633814B6 | .  52            PUSH EDX
//633814B7 | .  8B45 10       MOV EAX, DWORD PTR SS : [EBP + 10]
//633814BA | .  50            PUSH EAX
//633814BB | .  8B4D 0C       MOV ECX, DWORD PTR SS : [EBP + 0C]
//633814BE | .  51            PUSH ECX
//633814BF | .  8B55 F4       MOV EDX, DWORD PTR SS : [EBP - 0C]
//633814C2 | .  8B02          MOV EAX, DWORD PTR DS : [EDX]
//633814C4 | .  8B4D F4       MOV ECX, DWORD PTR SS : [EBP - 0C]
//633814C7 | .  51            PUSH ECX
//633814C8      8B50 24       MOV EDX, DWORD PTR DS : [EAX + 24]
//633814CB | .FFD2          CALL EDX


