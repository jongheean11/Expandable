#include "stdafx.h"
#include "E_Window.h"

E_Window::E_Window(HWND window)
{
	this->window = window;
	SetMinimizeMaximizeAnimation(false);
	takeScreenshot();
	SetMinimizeMaximizeAnimation(true);
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


// 윈도우 세팅
void E_Window::setWindow(HWND window)
{
	this->window = window;
}


HWND E_Window::getWindow()
{
	return window;
}

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

void E_Window::SetMinimizeMaximizeAnimation(bool status)
{
	ANIMATIONINFO animationInfo;
	if (status)
		animationInfo.iMinAnimate = 1;
	else
		animationInfo.iMinAnimate = 0;
	animationInfo.cbSize = sizeof(ANIMATIONINFO);
	//SystemParametersInfo(SPI_GETANIMATION, sizeof(ANIMATIONINFO),
	//	&animationInfo,SPIF_UPDATEINIFILE);

	//if (animationInfo.iMinAnimate != status)
	//{
		//animationInfo.iMinAnimate = status;
		SystemParametersInfo(SPI_SETANIMATION, sizeof(ANIMATIONINFO),
			&animationInfo, SPIF_SENDCHANGE);
	//}
}

bool E_Window::takeScreenshot()
{
	//ShowWindow(window,SW_FORCEMINIMIZE); // 스크린샷 안됨
	//ShowWindow(window, SW_HIDE);	//스크린샷 안됨
	//ShowWindow(window, SW_MAXIMIZE);
	//ShowWindow(window, SW_MINIMIZE);	//스크린샷 안됨
	//ShowWindow(window, SW_RESTORE);
	//ShowWindow(window, SW_SHOW);
	//ShowWindow(window, SW_SHOWDEFAULT);
	//ShowWindow(window, SW_SHOWMAXIMIZED);
	//ShowWindow(window, SW_SHOWMINIMIZED);	//스크린샷 안됨
	//ShowWindow(window, SW_SHOWMINNOACTIVE);	//스크린샷 안됨
	//ShowWindow(window, SW_SHOWNA);	//될수도 안될수도있음
	//ShowWindow(window, SW_SHOWNOACTIVATE);
	//ShowWindow(window, SW_SHOWNORMAL);

	bool notShowing = false;
	UINT state;
	WINDOWPLACEMENT windowinfo;
	GetWindowPlacement(window, &windowinfo);

	//몇몇 상태는 아예 필요없는 조건일 수 있으나 스크린샷이 찍히지 않는 경우를 모두 찾음
	if ((state = (windowinfo.showCmd )) == SW_FORCEMINIMIZE
		|| (state = (windowinfo.showCmd )) == SW_HIDE		//HIDE는 사실 처리 안됨 (invisible)
		|| (state = (windowinfo.showCmd )) == SW_MINIMIZE
		|| (state = (windowinfo.showCmd )) == SW_SHOWMINIMIZED
		|| (state = (windowinfo.showCmd )) == SW_SHOWMINNOACTIVE
		|| (state = (windowinfo.showCmd )) == SW_SHOWNA
		){
		//스크린샷이 안되는 상황
		notShowing = true;
		this->setTransparent();
		ShowWindow(window, SW_RESTORE);
	}

	HWND hTargetWnd = window;

	LPCTSTR lpszFilePath;

	CRect rct;
	if (hTargetWnd)
		::GetWindowRect(hTargetWnd, &rct);
	else
		return FALSE;

	HBITMAP hBitmap = NULL;
	HBITMAP hOldBitmap = NULL;
	BOOL bSuccess = FALSE;

	HDC hDC = ::GetWindowDC(hTargetWnd);
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	hBitmap = ::CreateCompatibleBitmap(hDC, rct.Width(), rct.Height());

	if (!hBitmap)
		return FALSE;

	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

	if (!::PrintWindow(hTargetWnd, hMemDC, 0))
		bSuccess = FALSE;
	else
		bSuccess = TRUE;


	SelectObject(hMemDC, hOldBitmap);

	//DeleteObject(hBitmap);

	::DeleteDC(hMemDC);
	::ReleaseDC(hTargetWnd, hDC);

	screenshot.Attach(hBitmap);

	if (notShowing == true){
		ShowWindow(window, state);
		this->setOpaque();
	}
	
	return bSuccess;
	
}


void E_Window::setHide()
{
	ShowWindow(window,SW_HIDE);
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


CBitmap* E_Window::getScreenshot()
{
	return &screenshot;
}
