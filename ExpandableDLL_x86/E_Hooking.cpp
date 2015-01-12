#include "stdafx.h"
#include "E_Hooking.h"
//5바이트 ShowWindow를 위한 핫 패치 7652F2A9
BOOL hook_by_hotpatch_ShowWindow() {
	OutputDebugStringA("[CALL] hook_by_hotpatch_ShowWindow");

	PROC pShowWindowHook = (PROC)ShowWindowHook;
	FARPROC pShowWindow = NULL;
	LPCSTR dllName = "user32.dll";
	LPCSTR funcName = "ShowWindow";
	DWORD dwOldProtect;
	BYTE pJumpCode[5] = { 0xE9, 0, };
	BYTE pShortJump[5] = { 0xEB, 0xF9, 0x90, 0x90, 0x90 };
	BYTE pOriginalCodeShowWindow[5] = { 0xB8, 0x4F, 0x12, 0x00, 0x00 };

	//1)원래 함수 주소 저장
	pShowWindow = GetProcAddress(GetModuleHandleA(dllName), funcName);

	//2)읽기 쓰기 권한 부여
	VirtualProtect((LPVOID)((DWORD)pShowWindow - 5), 10, PAGE_EXECUTE_READWRITE, &dwOldProtect);

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
		ptr = (DWORD*)*(ptr + 9);
		pfSetProgressValue = (FARPROC)ptr;
		//char dbgmsg[255] = { 0, };
		//sprintf_s(dbgmsg, "ptr %x", (DWORD)ptr);
		//OutputDebugStringA(dbgmsg);
		//	pTaskList3->Release();
	}
	else {
		return FALSE;
	}

	//2)후킹 (PVOID)(&(PVOID&)HookingClass::HookingFu
	if (!hook_by_hotpatch(pfSetProgressValue, (PROC)((DWORD)SetProgressValueHook))) {
		return FALSE;
	}

	//3)SetProgresssValue 함수주소 유지를 위해 인터페이스 객체를 해제하지 않는다.

	return result;
}

BOOL unhook_by_hotpatch_SetProgressValue()
{

	return FALSE;
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
BOOL hook_by_hotpatch(LPCSTR szDllName, LPCSTR szFuncName, PROC pfnNew)
{
	FARPROC pFunc;
	DWORD dwOldProtect, dwAddress;
	BYTE pBuf[5] = { 0xE9, 0, };
	BYTE pBuf2[2] = { 0xEB, 0xF9 };
	PBYTE pByte;

	pFunc = (FARPROC)GetProcAddress(GetModuleHandleA(szDllName), szFuncName);
	pByte = (PBYTE)pFunc;
	if (pByte[0] == 0xEB)
		return FALSE;

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
	if (pByte[0] == 0xEB)
		return FALSE;

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

//후킹ShowWindow API
BOOL WINAPI ShowWindowHook(
	_In_  HWND hWnd,
	_In_  int nCmdShow){
	OutputDebugStringA("[ShowWindow] Called");

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
	TRACE_WIN32A("[CALL] SetProgressValueHook (%d, %d)", ullCompleted, ullTotal);
	//OutputDebugStringA("[CALL] SetProgressValueHook");

	PFSETPROGRESSVALUE pfProgressValue;
	ITaskbarList3* pTaskList3 = NULL;

	HRESULT hresult = CoCreateInstance(CLSID_TaskbarList, NULL,
		CLSCTX_SERVER, IID_ITaskbarList3,
		(LPVOID *)&pTaskList3);

	if (SUCCEEDED(hresult)) {

		//hresult = pTaskList3->SetProgressValue(hwnd, ullCompleted, ullTotal);
		//	pTaskList3->Release();
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
