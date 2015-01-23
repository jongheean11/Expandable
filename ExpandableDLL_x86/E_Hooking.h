
#include <windows.h>
#include "E_DLLClient.h"

#define STATUS_SUCCESS						(0x00000000L) 

extern BYTE g_pZWRT[5];
extern char* processName;
extern int pid;
extern HWND hwnd;

BOOL hook_by_hotpatch_ShowWindow();
BOOL unhook_by_hotpatch_ShowWindow();
BOOL hook_by_hotpatch_SetProgressState();
BOOL unhook_by_hotpatch_SetProgressState();
BOOL hook_by_hotpatch_SetProgressValue();
BOOL unhook_by_hotpatch_SetProgressValue();
BOOL unhook_by_hotpatch(FARPROC pFunc);
BOOL unhook_by_hotpatch(LPCSTR szDllName, LPCSTR szFuncName);
BOOL hook_by_hotpatch(LPCSTR szDllName, LPCSTR szFuncName, PROC pfnNew);
BOOL hook_by_hotpatch(FARPROC pFunc, PROC pfnNew);

BOOL hook_by_code(LPCSTR szDllName, LPCSTR szFuncName, PROC pfnNew, PBYTE pOrgBytes);
BOOL unhook_by_code(LPCSTR szDllName, LPCSTR szFuncName, PBYTE pOrgBytes);
BOOL MyCreateRemoteThread(HANDLE hProcess, LPTHREAD_START_ROUTINE pThreadProc, LPVOID pRemoteBuf);
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath);
BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);
NTSTATUS WINAPI NewZwResumeThread(HANDLE ThreadHandle, PULONG SuspendCount);

BOOL WINAPI ShowWindowHook(
	_In_  HWND hWnd,
	_In_  int nCmdShow);

HRESULT WINAPI SetProgressValueHook(_In_ void* thisPtr,
	_In_  HWND hwnd,
	_In_  ULONGLONG ullCompleted,
	_In_  ULONGLONG ullTotal);

HRESULT WINAPI SetProgressStateHook(_In_ void* thisPtr,
	HWND hwnd,
	TBPFLAG tbpFlags
	);

typedef BOOL WINAPI PFSHOWWINDOW(
	_In_  HWND hWnd,
	_In_  int nCmdShow);

typedef HRESULT PFSETPROGRESSVALUE(
	_In_  HWND hwnd,
	_In_  ULONGLONG ullCompleted,
	_In_  ULONGLONG ullTotal
	);

typedef HRESULT PFSETPROGRESSSTATE(
	_In_  HWND hwnd,
	_In_ TBPFLAG tbpFlags
	);

typedef struct _CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID;

typedef struct _THREAD_BASIC_INFORMATION {
	NTSTATUS ExitStatus;
	PVOID TebBaseAddress;
	CLIENT_ID ClientId;
	ULONG AffinityMask;
	LONG Priority;
	LONG BasePriority;
} THREAD_BASIC_INFORMATION;

typedef NTSTATUS(WINAPI *PFZWRESUMETHREAD)
(
HANDLE ThreadHandle,
PULONG SuspendCount
);


typedef NTSTATUS(WINAPI *PFZWQUERYINFORMATIONTHREAD)
(
HANDLE ThreadHandle,
ULONG ThreadInformationClass,
PVOID ThreadInformation,
ULONG ThreadInformationLength,
PULONG ReturnLength
);


BOOL IsVistaLater();

typedef DWORD(WINAPI *PFNTCREATETHREADEX)
(
PHANDLE                 ThreadHandle,
ACCESS_MASK             DesiredAccess,
LPVOID                  ObjectAttributes,
HANDLE                  ProcessHandle,
LPTHREAD_START_ROUTINE  lpStartAddress,
LPVOID                  lpParameter,
BOOL	                CreateSuspended,
DWORD                   dwStackSize,
DWORD                   dw1,
DWORD                   dw2,
LPVOID                  Unknown
);
