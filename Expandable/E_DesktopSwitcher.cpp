#include "stdafx.h"
#include "E_DesktopSwitcher.h"
#include "E_AeroPeekController.h"
#include <Windows.h> 
#include <WinUser.h>
#include <sstream>
#include <string>

HRESULT UpdateDesktop_Background(HWND hwnd, double left, double top, double right, double bottom);
HRESULT UpdateDesktop_Taskbar(HWND hwnd, double left, double top, double right, double bottom);
HRESULT UpdateDesktop3(HWND hwnd, double left, double top, double right, double bottom);

void drawDesktopSwitcher()
{
	E_Global* e_global = E_Global::getSingleton();
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_DesktopSwitcher* deSwitcher = E_DesktopSwitcher::getSingleton();
	E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();
	
	PAINTSTRUCT ps;
	HDC hdc;
	HTHUMBNAIL pushThumbnail;
	CRect sizeRect_background, sizeRect_taskbar;
	HWND hShellWnd = GetShellWindow(); // 바탕화면 hwnd
	GetWindowRect(hShellWnd, &sizeRect_background); // 바탕화면 크기 얻기
	HWND hTaskbarWnd = FindWindowW(_T("Shell_TrayWnd"), NULL); // 작업표시줄 hwnd
	GetWindowRect(hTaskbarWnd, &sizeRect_taskbar); // 작업표시줄 크기 얻기

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


	//Desktop 및 Window Manage 된 후 적용 가능한 코드.
	
	//for (std::list<E_Desktop*>::iterator itr_desktop = e_global->desktopList.begin(); itr_desktop != e_global->desktopList.end(); itr_desktop++) //iterating list
	for (;;)
	{
		//background_left = initial_padding_width + padding_width * (2 * i - 1) + switch_width * (i - 1) + __max*((switch_width + padding_width * 2) / 2);
		background_left = initial_padding_width + padding_width * (2 * i - 1) + switch_width * (i - 1) + (__max-3)*((switch_width + padding_width * 2) / 2);
		background_right = background_left + switch_width;
		RECT *backgroundRECT = new RECT
		{
			background_left,
			background_top,
			background_right,
			background_bottom,
		};
		aeController->registerAero(hShellWnd, deSwitcher->m_hWnd, *backgroundRECT, pushThumbnail);
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
		std::list<HWND> all_windows = e_global->getAllWindows();

		//for (std::list<E_Window*>::iterator itr_window = ((*itr_desktop)->onWindowList.end()); itr_window != ((*itr_desktop)->onWindowList.begin()); itr_window--) // iterating backward
		for (std::list<HWND>::iterator itr_window = all_windows.begin(); itr_window != all_windows.end(); itr_window++)
		{
			//double window_left = itr_window->getStartX() * ratio_ww + background_left,
				//window_top = itr_window->getStartY() * ratio_hh + background_top;
			//double window_right = itr_window->getWidth() * ratio_ww + window_left,
				//window_bottom = itr_window->getHeight() * ratio_hh + window_top;
			CRect getSize;
			//GetWindowRect((*itr_window)->getWindow(), &getSize);
			GetWindowRect(*itr_window, &getSize);
			double window_left = getSize.left * ratio_ww + background_left,
				window_top = getSize.top * ratio_hh + background_top;
			double window_right = getSize.Width() * ratio_ww + window_left,
				window_bottom = getSize.Height() * ratio_hh + window_top;
			RECT *windowRECT = new RECT
			{
				window_left,
				window_top,
				window_right,
				window_bottom
			};
			//aeController->registerAero((*itr_window)->getWindow(), deSwitcher->m_hWnd, *windowRECT, pushThumbnail);
			aeController->registerAero(*itr_window, deSwitcher->m_hWnd, *windowRECT, pushThumbnail);
			deSwitcher->handle_list.push_back(pushThumbnail);
			
			deSwitcher->window_area_list_rect.push_back(windowRECT);
			//deSwitcher->window_area_list_hwnd.push_back((*itr_window)->getWindow());
		}

		deSwitcher->desktop_area_list_rect.push_back(backgroundRECT);

		i++;
		//if (i == _max)
		if (i == _max-3)
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

	window_selected = false;
	winThumbProps.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY;
	// Use the window frame and client area
	winThumbProps.fSourceClientAreaOnly = FALSE;
	winThumbProps.fVisible = TRUE;
	winThumbProps.opacity = (255 * 70) / 100;
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
		UpdateWindow();
		E_Window::setIconInvisible(this->m_hWnd);
		ison = true;
		
		drawDesktopSwitcher();
		ShowWindow(SW_SHOWMAXIMIZED);
	}
	else
	{
		terminateSwitcher();
	}
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

BEGIN_MESSAGE_MAP(E_DesktopSwitcher, CWnd)
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


void E_DesktopSwitcher::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (!window_selected)
	{
		desktop_i = 0;
		for (std::list<RECT*>::iterator itr_desktop_area = desktop_area_list_rect.begin(); itr_desktop_area != desktop_area_list_rect.end(); itr_desktop_area++)
		{
			if (((CRect)(*itr_desktop_area)).PtInRect(point))
			{
				window_selected = true;
				window_leftdown_point = point;
				window_RECT = *itr_desktop_area;
				OutputDebugStringA("desktop\n");

				int i = 0;
				for (std::list<HTHUMBNAIL>::iterator itr_desktop_thumbnail = handle_list.begin(); itr_desktop_thumbnail != handle_list.end(); itr_desktop_thumbnail++)
				{
					if (i != desktop_i)
						i++;
					else
					{
						window_hthumbnail = *itr_desktop_thumbnail;
						return;
					}
				}
			}
			desktop_i++;
		}
	}
	
	// window 공간들 iterating
	int window_i = 0;
	for (std::list<RECT*>::iterator itr_window_area = window_area_list_rect.begin(); itr_window_area != window_area_list_rect.end(); itr_window_area++)
	{
		double areasize = ((*itr_window_area)->right - (*itr_window_area)->left)*((*itr_window_area)->bottom - (*itr_window_area)->top);
		if ((((CRect)(*itr_window_area)).PtInRect(point)) && !(areasize == 2073600))
		{
			OutputDebugStringA("window\n");
			return;
		}
		
		window_i++;
	}
	

	CWnd::OnLButtonDown(nFlags, point);
}


void E_DesktopSwitcher::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	window_selected = false;
	
	CWnd::OnLButtonUp(nFlags, point);
}


void E_DesktopSwitcher::OnMouseMove(UINT nFlags, CPoint point)
{
	// desktop 공간들 iterating
	if (((GetKeyState(VK_LBUTTON) & 0x80) != 0) && window_selected)
	{
		LONG diff_x = point.x - window_leftdown_point.x,
			diff_y = point.y - window_leftdown_point.y;

		(*window_RECT).left = (*window_RECT).left + diff_x;
		(*window_RECT).right = (*window_RECT).right + diff_x;
		(*window_RECT).top = (*window_RECT).top + diff_y;
		(*window_RECT).bottom = (*window_RECT).bottom + diff_y;

		// The destination rectangle size

		// Set the thumbnail properties for use
		
		winThumbProps.rcDestination = *window_RECT;
		DwmUpdateThumbnailProperties(window_hthumbnail, &winThumbProps);
		
		window_leftdown_point = point;

		long x = diff_x, y = diff_y;
		std::ostringstream ss_x, ss_y;
		ss_x << x; ss_y << y;
		string result = "desktop x: " + ss_x.str() + ", y: " + ss_y.str() + "\n";
		std::wstring result_wstring = std::wstring(result.begin(), result.end());
		LPCWSTR result_lpcwstr = result_wstring.c_str();

		OutputDebugString(result_lpcwstr);
		return;
	}


	CWnd::OnMouseMove(nFlags, point);
}
