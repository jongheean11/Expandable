#include "stdafx.h"
#include "E_DesktopSwitcher.h"
#include "E_AeroPeekController.h"
#include <Windows.h> 
#include <WinUser.h>

HRESULT UpdateDesktop_Background(HWND hwnd, double left, double top, double right, double bottom);
HRESULT UpdateDesktop_Taskbar(HWND hwnd, double left, double top, double right, double bottom);
HRESULT UpdateDesktop3(HWND hwnd, double left, double top, double right, double bottom);


E_DesktopSwitcher::E_DesktopSwitcher()
{	
	/*
	hwnd_cwnd = NULL;
	hwnd_cwnd = new CWnd;
	*/
	
	ison = false;
}

E_DesktopSwitcher::~E_DesktopSwitcher()
{
}
E_DesktopSwitcher* E_DesktopSwitcher::singleton = NULL;

void E_DesktopSwitcher::startSwitcher()
{	
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();

	CBrush m_oBkgndBrush;
	m_oBkgndBrush.CreateSolidBrush(RGB(255, 255, 255));
	UINT nClassStyle = CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	CString szClassName = AfxRegisterWndClass(nClassStyle, 0, (HBRUSH)m_oBkgndBrush.GetSafeHandle(), 0);
	if (!ison)
	{	
		Create(szClassName, _T(""), WS_VISIBLE, CRect(0, 0, enManager->getWidth(), enManager->getHeight()), CWnd::GetDesktopWindow(), 1234); // nid 뭔지 꼭 찾기.
		ShowWindow(SW_SHOWMAXIMIZED);
		UpdateWindow();
		E_Window::setIconInvisible(this->m_hWnd);
		ison = true;
		

		int wmId, wmEvent;
		PAINTSTRUCT ps;
		CDC* cdc;
		HDC hdc;
		HWND other = NULL;

		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		E_AeroPeekController::UpdateDesktop_Background(*this, 50, 50, 500, 303.125);
		E_AeroPeekController::UpdateDesktop_Taskbar(*this, 50, 293.593, 500, 281 + 23);
		UpdateDesktop3(*this, 100, 100, 300, 180);
		cdc = BeginPaint(&ps);
		EndPaint(&ps);

		/*
		// HTHUMBNAIL list 등록
		for () //iterating list
		{
		PAINTSTRUCT ps;
		HDC hdc;
		HTHUMBNAIL pushThumbnail;
		CRect sizeRect;
		GetWindowRect(targetHWND, &sizeRect);
		RECT backgroundRECT = iterating // { , 50, 500, (500 - 50)*(enManager->getHeight()) / enManager->getWidth() };
		E_AeroPeekController::getSingleton()->registerAero(, hwnd_cwnd->m_hWnd, backgroundRECT, pushThumbnail);
		handle_list.push_back(pushThumbnail);

		hdc = BeginPaint(hwnd_cwnd->m_hWnd, &ps);
		// TODO: 여기에 그리기 코드를 추가합니다.
		EndPaint(hwnd_cwnd->m_hWnd, &ps);
		}*/
	}
	else
	{
		E_Window::setIconVisible(this->m_hWnd);
		DestroyWindow();
		ison = false;
	}

	/*E_Window::setIconInvisible(hwnd_cwnd->m_hWnd);
	hwnd_cwnd->ShowWindow(SW_SHOWMAXIMIZED);
	hwnd_cwnd->UpdateWindow();*/
	
	//hwnd_cwnd->SetLayeredWindowAttributes(RGB(255, 0, 0), 0, LWA_COLORKEY);
}

void E_DesktopSwitcher::terminateSwitcher()
{
}

void E_DesktopSwitcher::switchDesktop(E_Desktop* selection)
{

}

void E_DesktopSwitcher::drawOverview(E_Desktop* targetDesktop, E_Window* targetWindow)
{

}

void E_DesktopSwitcher::updateComponent(E_Desktop* targetDesktop, E_Window* targetWindow)
{

}

HRESULT UpdateDesktop3(HWND hwnd, double left, double top, double right, double bottom)
{
	string mainWindowStr = "Facebook - Chrome",
		exWindowStr = "Chrome_RenderWidgetHostHWND";
	std::wstring l_temp = std::wstring(mainWindowStr.begin(), mainWindowStr.end());
	LPCWSTR l_str = l_temp.c_str();
	std::wstring t_temp = std::wstring(exWindowStr.begin(), exWindowStr.end());
	LPCWSTR t_str = t_temp.c_str();
	HRESULT hr = S_OK;
	// Register the thumbnail
	HTHUMBNAIL thumbnail = NULL;
	
	HWND chrome = FindWindowW(NULL, l_str);
	if (NULL != chrome) {
		HWND child = FindWindowExW(chrome, NULL, t_str, NULL);
		if (NULL != child) {
			hr = DwmRegisterThumbnail(hwnd, chrome, &thumbnail);
			if (SUCCEEDED(hr))
			{
				OutputDebugString(L"DRAW SUCCESS\n");
			}
			else {
				OutputDebugString(L"DRAW FAIL\n");
			}
		}
		else
			OutputDebugString(L"NO CHILD FOUND\n");
	}
	else {
		OutputDebugString(L"NO Chrome FOUND\n");
	}
	if (SUCCEEDED(hr))
	{
		// The destination rectangle size
		RECT dest = { left, top, right, bottom };

		// Set the thumbnail properties for use
		DWM_THUMBNAIL_PROPERTIES dskThumbProps;
		dskThumbProps.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY;

		// Use the window frame and client area
		dskThumbProps.fSourceClientAreaOnly = FALSE;
		dskThumbProps.fVisible = TRUE;
		dskThumbProps.opacity = (255 * 70) / 100;
		dskThumbProps.rcDestination = dest;

		// Display the thumbnail
		hr = DwmUpdateThumbnailProperties(thumbnail, &dskThumbProps);
		if (SUCCEEDED(hr))
		{
			// ...
		}
	}
	return hr;
}