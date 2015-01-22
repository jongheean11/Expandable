#include "stdafx.h"
#include "E_Window.h"

E_Window::E_Window(HWND window) : tpmode(false)
{
	this->window = window;
	//��ũ����
	SetMinimizeMaximizeAnimation(false);
	takeScreenshot();
	SetMinimizeMaximizeAnimation(true);
	dock = false;
	//������
	int width = E_Util::getSystemSmallIconSize();
	HICON hicon = E_Util::getIconHandle(this->window);
	HBITMAP bitmap = E_Util::ConvertIconToBitmap(hicon, width, width);
	icon.Attach(bitmap);

	//������ �̸�
	GetWindowTextA(window, windowName, 255);
	savedState = 0xffffffff;
}

E_Window::~E_Window()
{
	//��ũ����/ ������ �ڿ� ����
	if (icon.DeleteObject()){
		TRACE_WIN32A("[Window::~E_Window()] icon �Ҹ�");
	}
	else{
		TRACE_WIN32A("[Window::~E_Window()] icon �Ҹ����");
	}
	if (screenshot.DeleteObject()){
		TRACE_WIN32A("[Window::~E_Window()] screenshot �Ҹ�");
	}
	else{
		TRACE_WIN32A("[Window::~E_Window()] screenshot �Ҹ����");
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
	SetLayeredWindowAttributes(window, 0, 0, LWA_ALPHA); //â����
}

void E_Window::setOpaque()
{
	tpmode = false;
	SetLayeredWindowAttributes(window, 0, 255, LWA_ALPHA); //��������
	SetWindowLongW(window, GWL_EXSTYLE, GetWindowLong(window, GWL_EXSTYLE) && ~WS_EX_LAYERED);
}

// ������ ����
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
	//ShowWindow(window,SW_FORCEMINIMIZE); // ��ũ���� �ȵ�
	//ShowWindow(window, SW_HIDE);	//��ũ���� �ȵ�
	//ShowWindow(window, SW_MAXIMIZE);
	//ShowWindow(window, SW_MINIMIZE);	//��ũ���� �ȵ�
	//ShowWindow(window, SW_RESTORE);
	//ShowWindow(window, SW_SHOW);
	//ShowWindow(window, SW_SHOWDEFAULT);
	//ShowWindow(window, SW_SHOWMAXIMIZED);
	//ShowWindow(window, SW_SHOWMINIMIZED);	//��ũ���� �ȵ�
	//ShowWindow(window, SW_SHOWMINNOACTIVE);	//��ũ���� �ȵ�
	//ShowWindow(window, SW_SHOWNA);	//�ɼ��� �ȵɼ�������
	//ShowWindow(window, SW_SHOWNOACTIVATE);
	//ShowWindow(window, SW_SHOWNORMAL);

	bool notShowing = false;
	UINT state;
	WINDOWPLACEMENT windowinfo;
	GetWindowPlacement(window, &windowinfo);

	//��� ���´� �ƿ� �ʿ���� ������ �� ������ ��ũ������ ������ �ʴ� ��츦 ��� ã��
	if ((state = (windowinfo.showCmd )) == SW_FORCEMINIMIZE
		|| (state = (windowinfo.showCmd )) == SW_HIDE		//HIDE�� ��� ó�� �ȵ� (invisible)
		|| (state = (windowinfo.showCmd )) == SW_MINIMIZE
		|| (state = (windowinfo.showCmd )) == SW_SHOWMINIMIZED
		|| (state = (windowinfo.showCmd )) == SW_SHOWMINNOACTIVE
		|| (state = (windowinfo.showCmd )) == SW_SHOWNA
		){
		//��ũ������ �ȵǴ� ��Ȳ
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


// ������ Show ���¸� ������ //���������� ���� �� ���� (���� ������ ���¸� �˰������ IsVisible API�� �� ��Ȯ�ϴ�)
UINT E_Window::getShowState()
{
	WINDOWPLACEMENT windowinfo;
	GetWindowPlacement(window, &windowinfo);
	
	return windowinfo.showCmd;
}


// ����ΰ� �������� �ƴ� �Լ�.
bool E_Window::isAeroPossible()
{
	UINT state;
	UINT winstate = getShowState();
	BOOL isVisible = IsWindowVisible(this->getWindow());
	BOOL isMinimized = IsIconic(this->getWindow());
	if ((state = winstate) == SW_FORCEMINIMIZE
		|| (state = winstate) == SW_HIDE		//HIDE�� ��� ó�� �ȵ� (invisible)
		|| (state = winstate) == SW_MINIMIZE
		|| (state = winstate) == SW_SHOWMINIMIZED
		|| (state = winstate) == SW_SHOWMINNOACTIVE
		|| isVisible == FALSE
		|| isMinimized == TRUE
		//|| (state = winstate) == SW_SHOWNA
		){
		//��ũ������ �ȵǴ� ��Ȳ
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

// ������ �� ����
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


// ����� ���� ����
void E_Window::restoreShowState()
{
	if (savedState != 0xffffffff)
		ShowWindow(window, savedState);
}


void E_Window::setRestore()
{
	ShowWindow(window, SW_RESTORE);
}


// ���� �˻�� ��� (�Ϲ������� �ִ�ȭ ���� �˻�)
bool E_Window::isMaximized()
{
	return IsZoomed(window);
}
	