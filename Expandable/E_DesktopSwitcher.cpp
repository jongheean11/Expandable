#include "stdafx.h"
#include "E_DesktopSwitcher.h"
#include "E_Global.h"
#include <Windows.h>

HRESULT UpdateDesktop(HWND hwnd);


E_DesktopSwitcher::E_DesktopSwitcher()
{	
	//hwnd_cwnd->Create(_T("STATIC"), _T("DesktopSwitcher"), WS_CHILD | WS_VISIBLE, CRect(0, 0, (int)E_Global::getresolutionWidth(), (int)E_Global::getresolutionHeight()), AfxGetApp()->m_pMainWnd, 1234);
	hwnd_cwnd = NULL;
	hwnd_cwnd = new CWnd;
	ison = false;
}

E_DesktopSwitcher::~E_DesktopSwitcher()
{
	//DestroyWindow(hwnd);
	hwnd_cwnd->DestroyWindow();
}

void E_DesktopSwitcher::drawWindowSwitcher()
{
	/*hwnd = CreateWindow(
		L"DesktopSwithcer",
		L"DesktopSwitcher",
		WS_OVERLAPPEDWINDOW,
		0, 0,
		(int)E_Global::resolutionWidth, (int)E_Global::resolutionHeight,
		NULL,
		NULL,
		NULL,
		NULL);
	ShowWindow(hwnd, SW_SHOWMAXIMIZED);*/
	CBrush m_oBkgndBrush;
	m_oBkgndBrush.CreateSolidBrush(RGB(255, 255, 255));
	UINT nClassStyle = CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	CString szClassName = AfxRegisterWndClass(nClassStyle, 0, (HBRUSH)m_oBkgndBrush.GetSafeHandle(), 0);
	if (!ison)
	{	
		hwnd_cwnd->Create(szClassName, _T(""), WS_VISIBLE, CRect(0, 0, (int)E_Global::resolutionWidth, (int)E_Global::resolutionHeight), CWnd::GetDesktopWindow() , 1234);
		//SetClassLong(hwnd_cwnd->)
		// nID : ID of the Window -> 고려안된점 : 해당 ID가 affordable한지 체크 안 되 있음.
		hwnd_cwnd->ShowWindow(SW_SHOWMAXIMIZED);
		hwnd_cwnd->UpdateWindow();
		E_Window::setIconInvisible(hwnd_cwnd->m_hWnd);
		ison = true;
	}
	else
	{
		E_Window::setIconVisible(hwnd_cwnd->m_hWnd);
		hwnd_cwnd->CloseWindow();
		hwnd_cwnd->DestroyWindow();
		ison = false;
		
		//hwnd_cwnd->~CWnd();
		/*hwnd_cwnd = new CWnd;
		hwnd_cwnd->Create(szClassName, _T(""), WS_VISIBLE, CRect(0, 0, (int)E_Global::resolutionWidth, (int)E_Global::resolutionHeight), CWnd::GetDesktopWindow(), 1234);*/
	}

	drawPreview();
	/*E_Window::setIconInvisible(hwnd_cwnd->m_hWnd);
	hwnd_cwnd->ShowWindow(SW_SHOWMAXIMIZED);
	hwnd_cwnd->UpdateWindow();*/
	
	//hwnd_cwnd->SetLayeredWindowAttributes(RGB(255, 0, 0), 0, LWA_COLORKEY);
	
}

void E_DesktopSwitcher::destroyWindowSwitcher()
{
	if (hwnd_cwnd != NULL)
	{
		hwnd_cwnd->DestroyWindow();
		hwnd_cwnd = NULL;
	}
}

void E_DesktopSwitcher::drawPreview()
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	HWND other = NULL;
	UpdateDesktop(hwnd_cwnd->m_hWnd);
	hdc = BeginPaint(hwnd_cwnd->m_hWnd, &ps);
	// TODO: 여기에 그리기 코드를 추가합니다.
	EndPaint(hwnd_cwnd->m_hWnd, &ps);
}

void E_DesktopSwitcher::onMouseClick()
{

}

void E_DesktopSwitcher::switchDesktop(int direction)
{

}

void E_DesktopSwitcher::drawOverview(E_Desktop* targetDesktop, E_Window* targetWindow)
{

}

void E_DesktopSwitcher::updateComponent(E_Desktop* targetDesktop, E_Window* targetWindow)
{

}

HRESULT UpdateDesktop(HWND hwnd)
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
		RECT dest = { 0, 50, 500, 550 };

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