
#include <windows.h>
BOOL hook_by_hotpatch_ShowWindow();
BOOL unhook_by_hotpatch_ShowWindow();
BOOL hook_by_hotpatch_SetProgressValue();
BOOL unhook_by_hotpatch_SetProgressValue();
BOOL unhook_by_hotpatch(LPCSTR szDllName, LPCSTR szFuncName);
BOOL hook_by_hotpatch(LPCSTR szDllName, LPCSTR szFuncName, PROC pfnNew);
BOOL hook_by_hotpatch(FARPROC pFunc, PROC pfnNew);

BOOL WINAPI ShowWindowHook(
	_In_  HWND hWnd,
	_In_  int nCmdShow);

HRESULT WINAPI SetProgressValueHook(_In_ void* thisPtr,
	_In_  HWND hwnd,
	_In_  ULONGLONG ullCompleted,
	_In_  ULONGLONG ullTotal);

typedef BOOL WINAPI PFSHOWWINDOW(
	_In_  HWND hWnd,
	_In_  int nCmdShow);

typedef HRESULT PFSETPROGRESSVALUE(
	_In_  HWND hwnd,
	_In_  ULONGLONG ullCompleted,
	_In_  ULONGLONG ullTotal
);