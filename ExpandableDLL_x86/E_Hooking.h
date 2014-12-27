BOOL hook_by_hotpatch_ShowWindow();
BOOL unhook_by_hotpatch_ShowWindow();
BOOL unhook_by_hotpatch(LPCSTR szDllName, LPCSTR szFuncName);
BOOL hook_by_hotpatch(LPCSTR szDllName, LPCSTR szFuncName, PROC pfnNew);
BOOL WINAPI ShowWindowHook(
	_In_  HWND hWnd,
	_In_  int nCmdShow);