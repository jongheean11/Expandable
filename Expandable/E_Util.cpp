#include "stdafx.h"
#include "E_Util.h"

HMODULE getModulePid(DWORD processID, wchar_t* searchStr);
HMODULE getModule(wchar_t* searchStr);
HMODULE getModuleHwnd(HWND hwnd);
BOOL CALLBACK shownWindow(HWND hwnd, LPARAM lParam);
HMODULE getModuleByWndTitle(wchar_t* searchStr);

HICON E_Util::getIconHandle(HWND hwnd) {
	HICON icon = NULL;
	wchar_t path[1024] = { 0, };
	DWORD pid;
	DWORD res = GetWindowThreadProcessId(hwnd, &pid); // !!??!!
	HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	
	int copyLength;
	copyLength = GetModuleFileNameEx(handle, NULL, path, 1024); // 32bit API
	
	if (copyLength == 0)
	{
		DWORD dwLen = sizeof(path) / sizeof(wchar_t);
		::QueryFullProcessImageName(handle, 0, path, &dwLen); // Windows Vista 이상 API
	}
	
	ExtractIconEx(path, 0, &icon, NULL, 1);
	CloseHandle(handle);
	return icon;
}

// =======================================
int E_Util::getSystemLargeIconSize()
{
	int width = GetSystemMetrics(SM_CXICONSPACING);
	return width;
}

int E_Util::getSystemSmallIconSize()
{
	int width = 2* GetSystemMetrics(SM_CXSMICON);
	return width;
}

MYICON_INFO E_Util::getIconInfo(HICON hIcon)
{
	MYICON_INFO myinfo;
	ZeroMemory(&myinfo, sizeof(myinfo));

	ICONINFO info;
	ZeroMemory(&info, sizeof(info));

	BOOL bRes = FALSE;

	bRes = GetIconInfo(hIcon, &info);
	if (!bRes)
		return myinfo;

	BITMAP bmp;
	ZeroMemory(&bmp, sizeof(bmp));

	if (info.hbmColor)
	{
		const int nWrittenBytes = GetObject(info.hbmColor, sizeof(bmp), &bmp);
		if (nWrittenBytes > 0)
		{
			myinfo.nWidth = bmp.bmWidth;
			myinfo.nHeight = bmp.bmHeight;
			myinfo.nBitsPerPixel = bmp.bmBitsPixel;
		}
	}
	else if (info.hbmMask)
	{
		// Icon has no color plane, image data stored in mask
		const int nWrittenBytes = GetObject(info.hbmMask, sizeof(bmp), &bmp);
		if (nWrittenBytes > 0)
		{
			myinfo.nWidth = bmp.bmWidth;
			myinfo.nHeight = bmp.bmHeight / 2;
			myinfo.nBitsPerPixel = 1;
		}
	}

	if (info.hbmColor)
		DeleteObject(info.hbmColor);
	if (info.hbmMask)
		DeleteObject(info.hbmMask);

	return myinfo;
}

HBITMAP E_Util::ConvertIconToBitmap(HICON hIcon, int nWidth, int nHeight, COLORREF clrBackground)
{
	if (hIcon == NULL)
		return NULL;
	HDC hDC = NULL;
	HDC hMemDC = NULL;
	HBITMAP hBitmap = NULL;
	HBITMAP hOldBitmap = NULL;
	HBRUSH hBrush = NULL;
	HBRUSH hOldBrush = NULL;



	BOOL bGotBitmap = false;

	try {
		hDC = ::GetDC(NULL);
		if (hDC != NULL) {
			hBitmap = CreateCompatibleBitmap(hDC, nWidth, nHeight);
			if (hBitmap != NULL) {
				hMemDC = CreateCompatibleDC(hDC);
				if (hMemDC != NULL) {
					// Select the bitmap into the memory device context
					hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

					hBrush = CreateSolidBrush(clrBackground);
					if (hBrush != NULL) {
						// Fill the bitmap with the background color
						hOldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
						PatBlt(hMemDC, 0, 0, nWidth, nHeight, PATCOPY);

						// Draw the icon
						DrawIconEx(hMemDC, 0, 0, hIcon, nWidth, nHeight, 0, NULL, DI_IMAGE);
						bGotBitmap = true;
					}
				}
			}
		}

		// Cleanup
		if (hOldBrush != NULL)
			SelectObject(hMemDC, hOldBrush);
		if (hOldBitmap != NULL)
			SelectObject(hMemDC, hOldBitmap);
		if (hBrush != NULL)
			DeleteObject(hBrush);
		if (hMemDC != NULL)
			DeleteDC(hMemDC);
		if (hDC != NULL)
			::ReleaseDC(NULL, hDC);

		if (!bGotBitmap && hBitmap != NULL) {
			DeleteObject(hBitmap);
			hBitmap = NULL;
		}
	}
	catch (...) {
	}
	return hBitmap;
}
E_Util::E_Util()
{
}


E_Util::~E_Util()
{
}


/* http://stackoverflow.com/questions/18729137/how-to-get-the-handle-id-from-process-hwnd-programmaticaly
*/

HMODULE getModulePid(DWORD processID, wchar_t* searchStr){ // gets the module by the module name from an explicit process

	HANDLE hProcess;
	HMODULE hMods[1024];
	TCHAR szModName[MAX_PATH];
	DWORD cbNeeded;

	if (hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID))
	{
		if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
		{
			unsigned int k;
			for (k = 0; k < (cbNeeded / sizeof(HMODULE)); ++k)
			{
				if (GetModuleFileNameEx(hProcess, hMods[k], szModName, sizeof(szModName) / sizeof(TCHAR)))
				{

					//printf( "fess pid: %u modname: %s\n", processID, szModName );

					if (wcsstr(szModName, searchStr))
					{
						//printf("pid: &#37;u modname: %s\n", processID, szModName);
						CloseHandle(hProcess);
						return hMods[k];
					}
				}
			}//for
		}
	}
	CloseHandle(hProcess);
	return NULL;
}

HMODULE getModule(wchar_t* searchStr){ // gets the module by the modul name from all processes
	DWORD aProcesses[1024], cbNeeded, cProcesses;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) return NULL;
	cProcesses = cbNeeded / sizeof(DWORD);

	HMODULE hmodule;
	unsigned int i;
	for (i = 0; i < cProcesses; ++i)
	{
		if (hmodule = getModulePid(aProcesses[i], searchStr))
		{
			return hmodule;
		}
	}
	return NULL;
}

HMODULE getModuleHwnd(HWND hwnd){ // gets the module from a window
	DWORD pid;
	DWORD tid = GetWindowThreadProcessId(hwnd, &pid); // !!??!!
	//printf("hwnd tid: %u\n", tid);
	//printf("hwnd pid: %u\n", pid);
	return getModulePid(pid, L".exe");
}

HMODULE hModuleT;
wchar_t* searchStrT;

BOOL CALLBACK shownWindow(HWND hwnd, LPARAM lParam){ // EnumWindows callback
	if (hModuleT) return TRUE;

	wchar_t pcWinTitle[256];

	if (GetWindow(hwnd, GW_OWNER)) return TRUE; // whats that?
	GetWindowText(hwnd, pcWinTitle, 1024);

	if (wcsstr(pcWinTitle, searchStrT))
	{
		//printf("wndtitle: %s\n", pcWinTitle);
		hModuleT = getModuleHwnd(hwnd);

		//char szBuff[512];
		//sprintf(szBuff, "%p", hwnd);

		//printf("Found .... hWnd: %s\n", szBuff);

	}

	return TRUE;
}

HMODULE getModuleByWndTitle(wchar_t* searchStr){ // gets the module from a window title
	searchStrT = searchStr;
	EnumWindows(shownWindow, 0);
	return hModuleT;
}

int PrintModules(DWORD processID)
{
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	unsigned int i;

	// Print the process identifier.

	printf("\nProcess ID: %u\n", processID);

	// Get a handle to the process.

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);
	if (NULL == hProcess)
		return 1;

	// Get a list of all the modules in this process.

	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];

			// Get the full path to the module's file.

			if (GetModuleFileNameEx(hProcess, hMods[i], szModName,
				sizeof(szModName) / sizeof(TCHAR)))
			{
				// Print the module name and handle value.
				TRACE_WIN32A("[%d]Module List %s", i, szModName);
				//_tprintf(TEXT("\t%s (0x%08X)\n"), szModName, hMods[i]);
			}
		}
	}

	// Release the handle to the process.

	CloseHandle(hProcess);

	return 0;
}

//존재하지 않거나 이미 DLL을 포함할때 true를 반환
bool E_Util::isContainDLL(wchar_t* processName, wchar_t* dllname)
{
	DWORD processID = GetProcessID(processName);
	if (processID == 0)
		return true;//프로세스아디가 존재하지 않음
	bool result = false;
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	unsigned int i;

	// Print the process identifier.

	//printf("\nProcess ID: %u\n", processID);

	// Get a handle to the process.

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);
	if (NULL == hProcess)
		return result;

	// Get a list of all the modules in this process.

	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];

			// Get the full path to the module's file.

			if (GetModuleFileNameEx(hProcess, hMods[i], szModName,
				sizeof(szModName) / sizeof(TCHAR)))
			{
				// Print the module name and handle value.
				//TRACE_WIN32A("[isContainDLL]%d. Module List %s", i, szModName);
				if (wcsstr(szModName, dllname) != 0)
				{
					//TRACE_WIN32A("[isContainDLL]IT HAS EXPANDABLE DLL");
					result = true;
					break;
				}
				//_tprintf(TEXT("\t%s (0x%08X)\n"), szModName, hMods[i]);
			}
		}
	}

	// Release the handle to the process.

	CloseHandle(hProcess);

	return result;
}

DWORD E_Util::GetProcessID(LPCTSTR pszProcessName)
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


//int main()
//{
//
//	//EnumWindows(EnumWindowsProc, 0);
//
//	printf("find by name ... \n");
//	getModule(L"calc.exe");
//	printf("\nfind by title ... \n");
//	getModuleByWndTitle(L"Calculator");
//
//	printf("Done");
//
//
//	return 0;
//}
