#include "stdafx.h"
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <stdio.h>
#include <conio.h>
#include "E_Util.h"


struct CallbackInfo
{
	DWORD m_dwPID;
	HWND m_hWnd;
};

BOOL CALLBACK EnumProc(HWND hWnd, LPARAM lParam)
{
	DWORD dwPID = 0;
	CallbackInfo *pst_Callback = (CallbackInfo *)lParam;

	GetWindowThreadProcessId(hWnd, &dwPID);

	if (pst_Callback->m_dwPID != dwPID)
		return TRUE;

	pst_Callback->m_hWnd = hWnd;
	return FALSE;
}

HWND GetWindowFromPID(DWORD dwPID)
{
	CallbackInfo st_Callback;

	if (dwPID == 0)
		return NULL;

	st_Callback.m_dwPID = dwPID;
	st_Callback.m_hWnd = NULL;

	EnumWindows(EnumProc, (LPARAM)&st_Callback);

	return st_Callback.m_hWnd;
}

BOOL GetProcessModule(DWORD dwPID, LPCTSTR pszProcessName)
{
	HANDLE        hModuleSnap = NULL;
	MODULEENTRY32 me32 = { 0 };

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

	if (hModuleSnap == (HANDLE)-1)
		return FALSE;

	me32.dwSize = sizeof(MODULEENTRY32);
	//해당 프로세스의 모듈리스트를 루프로 돌려서 프로세스이름과 동일하면
	//true를 리턴한다.
	if (Module32First(hModuleSnap, &me32))
	{
		do
		{
			if (wcscmp(me32.szModule, pszProcessName) == 0)
			{
				CloseHandle(hModuleSnap);
				return TRUE;
			}
		} while (Module32Next(hModuleSnap, &me32));
	}

	CloseHandle(hModuleSnap);

	return FALSE;
}

DWORD GetProcessID(LPCTSTR pszProcessName)
{

	HANDLE         hProcessSnap = NULL;
	BOOL           bRet = FALSE;
	PROCESSENTRY32 pe32 = { 0 };

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == (HANDLE)-1)
		return false;

	pe32.dwSize = sizeof(PROCESSENTRY32);
	DWORD dwProcessID = 0;

	//프로세스가 메모리상에 있으면 첫번째 프로세스를 얻는다
	if (Process32First(hProcessSnap, &pe32))
	{
		BOOL          bCurrent = FALSE;
		MODULEENTRY32 me32 = { 0 };
		do
		{
			if (!_wcsicmp(pe32.szExeFile, pszProcessName))
				bCurrent = TRUE;
			//bCurrent = GetProcessModule(pe32.th32ProcessID, pszProcessName);
			if (bCurrent)
			{
				dwProcessID = pe32.th32ProcessID;
				break;
			}

			//TRACE_WIN32(L"filesname :%s", pe32.szExeFile);
		} while (Process32Next(hProcessSnap, &pe32)); //다음 프로세스의 정보를 구하여 있으면 루프
	}

	CloseHandle(hProcessSnap);
	return dwProcessID;
}

BOOL CALLBACK MyChildWindowsProc(HWND hWnd, LPARAM lParam)
{
	TCHAR szClass[1024] = { 0, };

	int nFind = GetClassName(hWnd, szClass, 1024);
	if (nFind > 0)
		::PostMessage(hWnd, WM_CHAR, 'q', 0);//'q' 문자 메시지를 보낸다

	return TRUE;
}

DWORD getppid(LPCTSTR pszProcessName, DWORD pid){

	HANDLE         hProcessSnap = NULL;
	BOOL           bRet = FALSE;
	PROCESSENTRY32 pe32 = { 0 };

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == (HANDLE)-1)
		return false;

	pe32.dwSize = sizeof(PROCESSENTRY32);
	DWORD dwProcessID = 0;

	//프로세스가 메모리상에 있으면 첫번째 프로세스를 얻는다
	if (Process32First(hProcessSnap, &pe32))
	{
		BOOL          bCurrent = FALSE;
		MODULEENTRY32 me32 = { 0 };
		do
		{

			if (!_wcsicmp(pe32.szExeFile, pszProcessName) && pe32.th32ProcessID == pid)
				bCurrent = TRUE;
			if (bCurrent)
			{
				dwProcessID = pe32.th32ParentProcessID;
				break;
			}

		} while (Process32Next(hProcessSnap, &pe32)); //다음 프로세스의 정보를 구하여 있으면 루프
	}

	CloseHandle(hProcessSnap);
	return dwProcessID;
}


BOOL isVisible(char* processName){

	//wchar_t* transName = ConverCtoWC(processName);
	//TRACE_WIN32(L"pname %s", transName);

	DWORD pid = GetCurrentProcessId();
	//DWORD pid = GetProcessID(transName);
	//TRACE_WIN32A("my pid: %d", pid);
	HWND hwnd = GetWindowFromPID(pid);
	//TRACE_WIN32A("hwnd: %d", hwnd);
	
	/*if (transName != NULL)
		delete transName;*/
	if (hwnd > 0)
		return TRUE;
	else
		return FALSE;
}

wchar_t* ConverCtoWC(char* str)
{
	//wchar_t형 변수 선언
	wchar_t* pStr = NULL;
	//멀티 바이트 크기 계산 길이 반환
	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
	//wchar_t 메모리 할당
	pStr = new WCHAR[strSize];
	//형 변환
	MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);
	return pStr;
}