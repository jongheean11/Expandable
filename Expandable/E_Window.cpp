#include "stdafx.h"
#include "E_Window.h"

E_Window::E_Window()
{
}

E_Window::E_Window(const E_Window &tmp)
{
	start_x = tmp.start_x;
	start_y = tmp.start_y;
	width = tmp.width;
	height = tmp.height;
	zIndex = tmp.zIndex;
	windowState = tmp.windowState;
	dockState = tmp.dockState;
	alertState = tmp.alertState;
	hwnd = tmp.hwnd;
}

E_Window::~E_Window()
{
}

void E_Window::operator=(const E_Window &tmp)
{
	start_x = tmp.start_x;
	start_y = tmp.start_y;
	width = tmp.width;
	height = tmp.height;
	zIndex = tmp.zIndex;
	windowState = tmp.windowState;
	dockState = tmp.dockState;
	alertState = tmp.alertState;
	hwnd = tmp.hwnd;
}

bool E_Window::operator==(const E_Window &tmp)
{
	if (hwnd == tmp.hwnd)
		return true;
	else
		false;
}

void E_Window::setHWND(string mainWindowStr, string exWindowStr)
{
	std::wstring l_temp = std::wstring(mainWindowStr.begin(), mainWindowStr.end());
	LPCWSTR l_str = l_temp.c_str();
	std::wstring t_temp = std::wstring(exWindowStr.begin(), exWindowStr.end());
	LPCWSTR t_str = t_temp.c_str();
	HRESULT hr = S_OK;

	// Register the thumbnail
	HTHUMBNAIL thumbnail = NULL;
	HWND child = NULL;
	HWND targethwnd = FindWindowW(NULL, l_str);

	if (NULL != targethwnd) {
		child = FindWindowExW(targethwnd, NULL, t_str, NULL);
		if (NULL != child)
			OutputDebugString(L"CHILD FOUND\n");
		else
			OutputDebugString(L"NO CHILD FOUND\n");
	}
	hwnd = targethwnd;
}

void E_Window::setIconInvisible(HWND hwnd)
{
	ITaskbarList *pTaskList = NULL;
	CoCreateInstance(CLSID_TaskbarList, NULL,
		CLSCTX_SERVER, IID_ITaskbarList,
		(LPVOID *)&pTaskList);
	if (pTaskList != NULL) {
		// use pTaskList as needed...
		pTaskList->HrInit();
		HRESULT r = NULL;// = pTaskList->AddTab(GetDesktop());
		r = pTaskList->DeleteTab(hwnd);
		if (SUCCEEDED(r)) {
			OutputDebugStringA("ADD SUCCESS");
		}
		else {

			OutputDebugStringA("ADD FAIL");
		}
		pTaskList->Release();
	}
	else {
		OutputDebugStringA("INTERFACE FAIL");
	}
}

void E_Window::setIconVisible(HWND hwnd)
{
	ITaskbarList *pTaskList = NULL;
	CoCreateInstance(CLSID_TaskbarList, NULL,
		CLSCTX_SERVER, IID_ITaskbarList,
		(LPVOID *)&pTaskList);
	if (pTaskList != NULL) {
		// use pTaskList as needed...
		pTaskList->HrInit();
		HRESULT r = NULL;// = pTaskList->AddTab(GetDesktop());
		r = pTaskList->AddTab(hwnd);
		if (SUCCEEDED(r)) {
			OutputDebugStringA("ADD SUCCESS");
		}
		else {
			OutputDebugStringA("ADD FAIL");
		}
		pTaskList->Release();
	}
	else {
		OutputDebugStringA("INTERFACE FAIL");
	}
}

void E_Window::setTransparent()
{
	SetWindowLongW(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA); //창투명
}
void E_Window::setOpaque()
{
	SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA); //투명해제
	SetWindowLongW(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) && ~WS_EX_LAYERED);
}
HWND E_Window::getHWND()
{
	return hwnd;
}