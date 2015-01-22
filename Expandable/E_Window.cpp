#include "stdafx.h"
#include "E_Window.h"

E_Window::E_Window(HWND window) : tpmode(false)
{
	this->window = window;
	//스크린샷
	SetMinimizeMaximizeAnimation(false);
	takeScreenshot();
	SetMinimizeMaximizeAnimation(true);
	dock = false;
	//아이콘
	int width = E_Util::getSystemSmallIconSize();
	HICON hicon = E_Util::getIconHandle(this->window);
	HBITMAP bitmap = E_Util::ConvertIconToBitmap(hicon, width, width);
	icon.Attach(bitmap);

	//윈도우 이름
	GetWindowTextA(window, windowName, 255);
	savedState = 0xffffffff;
}

E_Window::~E_Window()
{
	//스크린샷/ 아이콘 자원 해제
	if (icon.DeleteObject()){
		TRACE_WIN32A("[Window::~E_Window()] icon 소멸");
	}
	else{
		TRACE_WIN32A("[Window::~E_Window()] icon 소멸실패");
	}
	if (screenshot.DeleteObject()){
		TRACE_WIN32A("[Window::~E_Window()] screenshot 소멸");
	}
	else{
		TRACE_WIN32A("[Window::~E_Window()] screenshot 소멸실패");
	}
}

bool E_Window::operator==(const E_Window &tmp)
{
	if (window == tmp.window)
		return true;
	else
		false;
}

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
	tpmode = true;
	SetWindowLongW(window, GWL_EXSTYLE, GetWindowLong(window, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(window, 0, 0, LWA_ALPHA); //창투명
}

void E_Window::setOpaque()
{
	tpmode = false;
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
		//ShowWindow(window, SW_SHOW);
	}

	HWND hTargetWnd = window;

	//LPCTSTR lpszFilePath;

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

	screenshot.DeleteObject();
	screenshot.Attach(hBitmap);

	if (notShowing == true){
		ShowWindow(window, state);
		this->setOpaque();
	}
	
	return bSuccess;
	
}

void E_Window::setMinimize()
{
	ShowWindow(window, SW_MINIMIZE);
}

void E_Window::setHide()
{
	ShowWindow(window,SW_HIDE);
}


void E_Window::setShow()
{
	ShowWindow(window, SW_SHOW);
}

void E_Window::setNormal()
{
	ShowWindow(window, SW_NORMAL);
}


CBitmap* E_Window::getScreenshot()
{
	return &screenshot;
}

/*

*/
CBitmap* E_Window::getIcon()
{
	return &icon;
}


// 윈도우 Show 상태를 가져옴 //정상적이지 않을 수 있음 (실제 윈도우 상태를 알고싶을땐 IsVisible API가 더 정확하다)
UINT E_Window::getShowState()
{
	WINDOWPLACEMENT windowinfo;
	GetWindowPlacement(window, &windowinfo);
	
	return windowinfo.showCmd;
}


// 에어로가 가능한지 아는 함수.
bool E_Window::isAeroPossible()
{
	UINT state;
	UINT winstate = getShowState();
	BOOL isVisible = IsWindowVisible(this->getWindow());
	BOOL isMinimized = IsIconic(this->getWindow());
	if ((state = winstate) == SW_FORCEMINIMIZE
		|| (state = winstate) == SW_HIDE		//HIDE는 사실 처리 안됨 (invisible)
		|| (state = winstate) == SW_MINIMIZE
		|| (state = winstate) == SW_SHOWMINIMIZED
		|| (state = winstate) == SW_SHOWMINNOACTIVE
		|| isVisible == FALSE
		|| isMinimized == TRUE
		//|| (state = winstate) == SW_SHOWNA
		){
		//스크린샷이 안되는 상황
		return false;
	}

	return true;
}


char* E_Window::getWindowName()
{
	return windowName;
}
bool E_Window::getTPMode()
{
	return tpmode;
}

// 윈도우 쇼 상태
void E_Window::saveShowState()
{
	if (isMaximized())
	{
		savedState = SW_MAXIMIZE;
	}
	else if (isAeroPossible())
	{
		savedState = SW_SHOWNORMAL;
	}
	else{
		savedState = SW_MINIMIZE;
	}
}


// 저장된 상태 복구
void E_Window::restoreShowState()
{
	if (savedState != 0xffffffff)
		ShowWindow(window, savedState);
}


void E_Window::setRestore()
{
	ShowWindow(window, SW_RESTORE);
}


// 상태 검사시 사용 (일반적으로 최대화 여부 검사)
bool E_Window::isMaximized()
{
	return IsZoomed(window);
}
	