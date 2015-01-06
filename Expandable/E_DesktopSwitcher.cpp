#include "stdafx.h"
#include "E_DesktopSwitcher.h"
#include "E_AeroPeekController.h"
#include <Windows.h> 
#include <WinUser.h>

HRESULT UpdateDesktop_Background(HWND hwnd, double left, double top, double right, double bottom);
HRESULT UpdateDesktop_Taskbar(HWND hwnd, double left, double top, double right, double bottom);
HRESULT UpdateDesktop3(HWND hwnd, double left, double top, double right, double bottom);

void drawDesktopSwitcher()
{
	E_Global *e_global = E_Global::getSingleton();
	PAINTSTRUCT ps;
	HDC hdc;
	HTHUMBNAIL pushThumbnail;
	CRect sizeRect_background, sizeRect_taskbar;
	HWND hShellWnd = GetShellWindow(); // 바탕화면 hwnd
	GetWindowRect(hShellWnd, &sizeRect_background); // 바탕화면 크기 얻기
	HWND hTaskbarWnd = FindWindowW(_T("Shell_TrayWnd"), NULL); // 작업표시줄 hwnd
	GetWindowRect(hTaskbarWnd, &sizeRect_taskbar); // 작업표시줄 크기 얻기

	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_DesktopSwitcher* deSwitcher = E_DesktopSwitcher::getSingleton();
	E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();
	
	double ratio_wh = (double)enManager->getWidth() / (double)enManager->getHeight(),
		ratio_hw = (double)enManager->getHeight() / (double)enManager->getWidth();
	double initial_padding_width = 50,
		initial_padding_height = 50 * ratio_hw;	
	double switch_width = ((double)enManager->getWidth() - initial_padding_width * 2) * (double)0.175, // 0.7/4 = 0.175, 100=50*2
		padding_width = ((double)enManager->getWidth() - initial_padding_width * 2) * (double)0.0375; // 0.15/4 = 0.0375
	double switch_height = switch_width * ratio_hw,
		padding_height = padding_width * ratio_hw; 
	double ratio_ww = switch_width / (double)enManager->getWidth(),
		ratio_hh = switch_height / (double)enManager->getHeight();
	
	double background_left = 0, background_top = initial_padding_height, // top은 변하지 않음.
		background_right = 0, background_bottom = initial_padding_height + switch_height, // bottom은 변하지 않음
		taskbar_top = background_bottom - sizeRect_taskbar.Height() * ratio_hh; // bottom, left, right는 background와 동일
	
	int i = 1,
	_max = e_global->desktopList.size() >= 4 ? 5 : e_global->desktopList.size() + 1,
	__max = e_global->desktopList.size() >= 4 ? 5 : 4 - e_global->desktopList.size();
	for (std::list<E_Desktop>::iterator itr_desktop = e_global->desktopList.begin(); itr_desktop != e_global->desktopList.end(); itr_desktop++) //iterating list
	{
		background_left = initial_padding_width + padding_width * (2 * i - 1) + switch_width * (i - 1) + __max*((switch_width + padding_width * 2) / 2);
		background_right = background_left + switch_width;
		RECT backgroundRECT =
		{
			background_left,
			background_top,
			background_right,
			background_bottom,
		};
		aeController->registerAero(hShellWnd, deSwitcher->m_hWnd, backgroundRECT, pushThumbnail);
		deSwitcher->handle_list.push_back(pushThumbnail);

		RECT taskbarRECT =
		{
			background_left,
			taskbar_top,
			background_right,
			background_bottom
		};
		aeController->registerAero(hTaskbarWnd, deSwitcher->m_hWnd, taskbarRECT, pushThumbnail);
		deSwitcher->handle_list.push_back(pushThumbnail);

		for (std::list<E_Window*>::iterator itr_window = itr_desktop->onWindowList.end(); itr_window != itr_desktop->onWindowList.begin(); itr_window--) // iterating backward
		{
			double window_left = (*itr_window)->getStartX() * ratio_ww + background_left,
				window_top = (*itr_window)->getStartY() * ratio_hh + background_top;
			double window_right = (*itr_window)->getWidth() * ratio_ww + window_left,
				window_bottom = (*itr_window)->getHeight() * ratio_hh + window_top;
			RECT windowRECT =
			{
				window_left,
				window_top,
				window_right,
				window_bottom
			};
			aeController->registerAero((*itr_window)->getWindow()->GetSafeHwnd(), deSwitcher->m_hWnd, windowRECT, pushThumbnail);
			deSwitcher->handle_list.push_back(pushThumbnail);
		}

		i++;
		if (i == _max)
			break;
	}
	
	hdc = BeginPaint(*deSwitcher, &ps);
	EndPaint(*deSwitcher, &ps);
}

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
		
		
		/*
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
		*/

		drawDesktopSwitcher();
		// HTHUMBNAIL list 등록
		
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
	E_AeroPeekController *e_aeropeekcontroller = E_AeroPeekController::getSingleton();
	for (std::list<HTHUMBNAIL>::iterator itr = handle_list.begin(); itr != handle_list.end(); itr++)
	{
		e_aeropeekcontroller->unregisterAero(*itr);
	}
	handle_list.clear();
	
	E_Window::setIconVisible(this->m_hWnd);
	DestroyWindow();
	ison = false;
}

void E_DesktopSwitcher::switchDesktop(E_Desktop* selection)
{

}
/*
void E_DesktopSwitcher::drawOverview(E_Desktop* targetDesktop, E_Window* targetWindow)
{

}

void E_DesktopSwitcher::updateComponent(E_Desktop* targetDesktop, E_Window* targetWindow)
{

}
*/
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