#include "stdafx.h"
#include "E_Hooking.h"

//Microsoft Windows 7 Enterprise K 32bits
//CPU Disasm
//Address   Hex dump          Command                                                Comments
//779BF2A4      90            NOP
//779BF2A5      90            NOP
//779BF2A6      90            NOP
//779BF2A7      90            NOP
//779BF2A8      90            NOP
//779BF2A9      B8 4F120000   MOV EAX, 124F; BOOL USER32.ShowWindow(hWnd, Show)
//779BF2AE | .BA 0003FE7F   MOV EDX, 7FFE0300
//779BF2B3 | .FF12          CALL DWORD PTR DS : [EDX]
//779BF2B5  \.C2 0800       RETN 8


//5����Ʈ ShowWindow�� ���� �� ��ġ
BOOL hook_by_hotpatch_ShowWindow() {
	PROC pShowWindowHook = (PROC)ShowWindowHook;
	FARPROC pShowWindow = NULL;
	LPCSTR dllName = "user32.dll";
	LPCSTR funcName = "ShowWindow";
	DWORD dwOldProtect;
	BYTE pJumpCode[5] = { 0xE9, 0, };
	BYTE pShortJump[5] = { 0xEB, 0xF9 ,0x90, 0x90, 0x90};
	BYTE pOriginalCodeShowWindow[5] = { 0xB8, 0x4F, 0x12, 0x00, 0x00 };

	//1)���� �Լ� �ּ� ����
	pShowWindow = GetProcAddress(GetModuleHandleA(dllName), funcName);
	
	//2)�б� ���� ���� �ο�
	VirtualProtect((LPVOID)((DWORD)pShowWindow - 5), 10, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	//3)Short Jump �ڵ� ����(hot fix
	memcpy(pShowWindow, pShortJump, 5);

	//3)Long Jump �ڵ� �ۼ�
	DWORD relativeAddress = (DWORD)pShowWindowHook - (DWORD)pShowWindow; //ShowWindowHook�� ShowWindow API ��� �ּ�
	memcpy(pJumpCode+1, &relativeAddress, 4);

	//4)Long Jump �ڵ� ���� (hot fix)
	memcpy((LPVOID)((DWORD)pShowWindow - 5), pJumpCode, 5);
	
	//5)������ �������� ����
	VirtualProtect((LPVOID)((DWORD)pShowWindow - 5), 10, dwOldProtect, &dwOldProtect);
	return TRUE;
}

//5����Ʈ ShowWindow�� ���� �� ��ġ ����
BOOL unhook_by_hotpatch_ShowWindow()
{
	PROC pShowWindowHook = (PROC)ShowWindowHook;
	FARPROC pShowWindow = NULL;
	LPCSTR dllName = "user32.dll";
	LPCSTR funcName = "ShowWindow";
	DWORD dwOldProtect;
	BYTE pOriginalJumpCode[5] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
	BYTE pOriginalCodeShowWindow[5] = { 0xB8, 0x4F, 0x12, 0x00, 0x00 };

	//1)���� �Լ� �ּ� ����
	pShowWindow = GetProcAddress(GetModuleHandleA(dllName), funcName);

	//2)�б� ���� ���� �ο�
	VirtualProtect((LPVOID)((DWORD)pShowWindow - 5), 10, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	//3)Short Jump �ڵ� ����(hot fix)
	memcpy(pShowWindow, pOriginalCodeShowWindow, 5);

	//4)Long Jump �ڵ� ���� (hot fix)
	memcpy((LPVOID)((DWORD)pShowWindow - 5), pOriginalJumpCode, 5);

	//5)������ �������� ����
	VirtualProtect((LPVOID)((DWORD)pShowWindow - 5), 10, dwOldProtect, &dwOldProtect);
	return TRUE;
}

//7����Ʈ �� ��ġ ����
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

//7����Ʈ �� ��ġ
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

//��ŷShowWindow API
BOOL WINAPI ShowWindowHook(
	_In_  HWND hWnd,
	_In_  int nCmdShow){
	OutputDebugStringA("ShowWindow");

	FARPROC pShowWindow = NULL;
	BOOL result = FALSE;


	//1) original API ȣ�� (����� ���̸� ����Ͽ� 2����Ʈ ���� ���� ȣ��)
	pShowWindow = GetProcAddress(GetModuleHandleA("user32.dll"), "ShowWindow");
	pShowWindow = (FARPROC)((DWORD)pShowWindow + 2);
	
	//2) inline asm
	__asm
	{
		MOV EAX, 0x124F; BOOL USER32.ShowWindow(hWnd, Show); ��ŷ �� ShowWindow 5����Ʈ ��� ����

		PUSH DWORD PTR SS : [EBP + 0x0C]; _In_  int nCmdShow
		PUSH DWORD PTR SS : [EBP + 0x8]; _In_  HWND hWnd
		CALL pShowWindow; ShowWIndow ȣ��
	}

	return result;
}



