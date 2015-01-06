#include "stdafx.h"
#include "E_Window.h"

E_Window::E_Window(HWND window)
{
	this->window = window;
}
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
	window = tmp.window;
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
	window = tmp.window;
}

bool E_Window::operator==(const E_Window &tmp)
{
	if (window == tmp.window)
		return true;
	else
		false;
}

//void E_Window::setHWND(string mainWindowStr, string exWindowStr)
//{
//	std::wstring l_temp = std::wstring(mainWindowStr.begin(), mainWindowStr.end());
//	LPCWSTR l_str = l_temp.c_str();
//	std::wstring t_temp = std::wstring(exWindowStr.begin(), exWindowStr.end());
//	LPCWSTR t_str = t_temp.c_str();
//	HRESULT hr = S_OK;
//
//	// Register the thumbnail
//	HTHUMBNAIL thumbnail = NULL;
//	HWND child = NULL;
//	HWND targethwnd = FindWindowW(NULL, l_str);
//
//	if (NULL != targethwnd) {
//		child = FindWindowExW(targethwnd, NULL, t_str, NULL);
//		if (NULL != child)
//			OutputDebugString(L"CHILD FOUND\n");
//		else
//			OutputDebugString(L"NO CHILD FOUND\n");
//	}
//	hwnd = targethwnd;
//}

bool E_Window::setIconInvisible(HWND hwnd)
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
			pTaskList->Release();
			return true;
		}
		else {
			OutputDebugStringA("ADD FAIL");
			pTaskList->Release();
			return false;
		}
	}
	else {
		OutputDebugStringA("INTERFACE FAIL");
		return false;
	}
}

bool E_Window::setIconVisible(HWND hwnd)
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
			pTaskList->Release();
			OutputDebugStringA("ADD SUCCESS");
			return true;
		}
		else {
			pTaskList->Release();
			OutputDebugStringA("ADD FAIL");
			return false;
		}
	}
	else {
		OutputDebugStringA("INTERFACE FAIL");
	}
	return false;
}

void E_Window::setTransparent()
{
	SetWindowLongW(window, GWL_EXSTYLE, GetWindowLong(window, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(window, 0, 0, LWA_ALPHA); //창투명
}

void E_Window::setOpaque()
{
	SetLayeredWindowAttributes(window, 0, 255, LWA_ALPHA); //투명해제
	SetWindowLongW(window, GWL_EXSTYLE, GetWindowLong(window, GWL_EXSTYLE) && ~WS_EX_LAYERED);
}

//HWND E_Window::getHWND()
//{
//	return hwnd;
//}

double E_Window::getStartX()
{
	return start_x;
}

void E_Window::setStartX(double param_x)
{
	start_x = param_x;
}

double E_Window::getStartY()
{
	return start_y;
}

void E_Window::setStartY(double param_y)
{
	start_y = param_y;
}

double E_Window::getWidth()
{
	return width;
}

void E_Window::setWidth(double param_w)
{
	width = param_w;
}

double E_Window::getHeight()
{
	return height;
}

void E_Window::setHeight(double param_h)
{
	height = param_h;
}

bool E_Window::taskScreenshot()
{
	return false;
}


void E_Window::setHide()
{
	ShowWindow(window,SW_HIDE);
}


// 윈도우 세팅
void E_Window::setWindow()
{
}


CWnd* E_Window::getWindow()
{
	return NULL;
}


void E_Window::setIcon()
{
}


CBitmap E_Window::getIcon()
{
	return CBitmap();
}


void E_Window::setShow()
{
	ShowWindow(window, SW_SHOW);
}
