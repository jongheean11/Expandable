#include "stdafx.h"
#include "E_DesktopSwitcher.h"
#include "E_AeroPeekController.h"
#include <Windows.h> 
#include <WinUser.h>
#include <sstream>
#include <string>
#include "E_WindowSwitcher.h"

void E_DesktopSwitcher::updateSelectedDesktop()
{
	//업데이트가 발생한 경우 자동으로 호출됨
	TRACE_WIN32A("[E_DesktopSwitcher::updateSelectedDesktop()]");
}


void getWindowText(HWND hwnd)
{
	LPWSTR a = new WCHAR[1000];
	GetWindowText(hwnd, a, 1000);
	OutputDebugStringA("\n");
	OutputDebugString(a);
	OutputDebugStringA("\n");
	delete a;
}

double E_DesktopSwitcher::getMainDesktopSize()
{
	return main_desktop_size;
}

void E_DesktopSwitcher::setMainDesktopSize(double size)
{
	main_desktop_size = size;
}

void eraseWindowS()
{
	//deSwitcher->window_RECT_hthumbnail_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(windowRECT, pushThumbnail));
	//deSwitcher->window_area_list_rect.push_front(windowRECT);
	//deSwitcher->window_area_map_RECT_EWindow.insert(hash_map<RECT*, E_Window*>::value_type(windowRECT, *itr_window));
	E_DesktopSwitcher* deSwitcher = E_DesktopSwitcher::getSingleton();
	E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();

	for (list<RECT*>::iterator itr_rect = deSwitcher->window_area_list_rect.begin(); itr_rect != deSwitcher->window_area_list_rect.end(); itr_rect++)
	{
		aeController->unregisterAero(deSwitcher->window_RECT_hthumbnail_map.find(*itr_rect)->second);
	}
	
	deSwitcher->window_RECT_hthumbnail_map.clear();
	deSwitcher->window_area_list_rect.clear();
	deSwitcher->window_area_map_RECT_EWindow.clear();
}

void drawWindowS()
{
	HTHUMBNAIL pushThumbnail;
	E_Global* e_global = E_Global::getSingleton();
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_DesktopSwitcher* deSwitcher = E_DesktopSwitcher::getSingleton();
	E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();

	CRect sizeRect_background, sizeRect_taskbar;
	HWND hShellWnd = GetShellWindow(); // 바탕화면 hwnd
	GetWindowRect(hShellWnd, &sizeRect_background); // 바탕화면 크기 얻기
	HWND hTaskbarWnd = FindWindowW(_T("Shell_TrayWnd"), NULL); // 작업표시줄 hwnd
	GetWindowRect(hTaskbarWnd, &sizeRect_taskbar); // 작업표시줄 크기 얻기

	//Selected Desktop 부분
	double background_bottom = 1080 - 0.15 * (1080 - (2 * deSwitcher->initial_padding_height + deSwitcher->switch_height)),
		background_top = (2 * deSwitcher->initial_padding_height + deSwitcher->switch_height) + 0.05 * (1080 - (2 * deSwitcher->initial_padding_height + deSwitcher->switch_height));
	double background_left = (enManager->getWidth() - (background_bottom - background_top) * deSwitcher->ratio_wh) / 2;
	double background_right = enManager->getWidth() - background_left;

	deSwitcher->main_desktop_width = background_right - background_left;
	deSwitcher->main_desktop_height = background_bottom - background_top;

	deSwitcher->ratio_ww = (background_right - background_left) / (double)enManager->getWidth();
	deSwitcher->ratio_hh = (background_bottom - background_top) / (double)enManager->getHeight();
	double taskbar_top = background_bottom - sizeRect_taskbar.Height() * deSwitcher->ratio_hh; // bottom, left, right는 background와 동일

	deSwitcher->setMainDesktopSize((background_right - background_left) * (background_bottom - background_top));

	RECT main_backgroundRECT
	{
		background_left,
		background_top,
		background_right,
		background_bottom,
	};
	aeController->registerAero(hShellWnd, deSwitcher->m_hWnd, main_backgroundRECT, pushThumbnail);

	RECT taskbarRECT =
	{
		background_left,
		taskbar_top,
		background_right,
		background_bottom
	};
	aeController->registerAero(hTaskbarWnd, deSwitcher->m_hWnd, taskbarRECT, pushThumbnail);

	std::list<E_Desktop*>::iterator desktop_current = e_global->desktopList.begin();
	int desktop_i = 0;
	for (; desktop_current != e_global->desktopList.end(); desktop_current++)
	{
		if (desktop_i == e_global->getSelectedIndex())
			break;
		desktop_i++;
	}
	std::list<E_Window*> window_list = (*desktop_current)->windowList;
	for (std::list<E_Window*>::iterator itr_window = window_list.begin(); itr_window != window_list.end(); itr_window++)
	{
		if ((*itr_window)->isAeroPossible())
		{
			getWindowText((*itr_window)->getWindow());
			CRect getSize;
			GetWindowRect((*itr_window)->getWindow(), &getSize);
			double window_left = getSize.left * deSwitcher->ratio_ww + background_left,
				window_top = getSize.top * deSwitcher->ratio_hh + background_top;
			double window_right = getSize.Width() * deSwitcher->ratio_ww + window_left,
				window_bottom = getSize.Height() * deSwitcher->ratio_hh + window_top;
			RECT *windowRECT = new RECT
			{
				window_left,
				window_top,
				window_right,
				window_bottom
			};
			aeController->registerAero((*itr_window)->getWindow(), deSwitcher->m_hWnd, *windowRECT, pushThumbnail);
			deSwitcher->window_RECT_hthumbnail_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(windowRECT, pushThumbnail));

			deSwitcher->window_area_list_rect.push_front(windowRECT);
			deSwitcher->window_area_map_RECT_EWindow.insert(hash_map<RECT*, E_Window*>::value_type(windowRECT, *itr_window));
		}
	}
}

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

	deSwitcher->ratio_wh = (double)enManager->getWidth() / (double)enManager->getHeight();
	deSwitcher->ratio_hw = (double)enManager->getHeight() / (double)enManager->getWidth();
	deSwitcher->initial_padding_width = 50;
	deSwitcher->initial_padding_height = 50 * deSwitcher->ratio_hw;
	deSwitcher->switch_width = ((double)enManager->getWidth() - deSwitcher->initial_padding_width * 2) * (double)0.175; // 0.7/4 = 0.175, 100=50*2
	deSwitcher->padding_width = ((double)enManager->getWidth() - deSwitcher->initial_padding_width * 2) * (double)0.0375; // 0.15/4 = 0.0375
	deSwitcher->switch_height = deSwitcher->switch_width * deSwitcher->ratio_hw,
		deSwitcher->padding_height = deSwitcher->padding_width * deSwitcher->ratio_hw;
	deSwitcher->ratio_ww = deSwitcher->switch_width / (double)enManager->getWidth();
	deSwitcher->ratio_hh = deSwitcher->switch_height / (double)enManager->getHeight();
	
	double background_left = 0, background_top = deSwitcher->initial_padding_height, // top은 변하지 않음.
		background_right = 0, background_bottom = deSwitcher->initial_padding_height + deSwitcher->switch_height, // bottom은 변하지 않음
		taskbar_top = background_bottom - sizeRect_taskbar.Height() * deSwitcher->ratio_hh; // bottom, left, right는 background와 동일
	
	int i = 0,
		desktop_i = 0,
		desktop_listnum = e_global->desktopList.size() >= 4 ? 4 : e_global->desktopList.size(),
		__max = e_global->desktopList.size() >= 4 ? 0 : 4 - e_global->desktopList.size();

	std::list<HWND> all_windows = e_global->getAllWindows();

	//Desktop 리스트 부분
	std::list<E_Desktop*>::iterator itr_desktop = e_global->desktopList.begin();
	if (e_global->desktopList.size() > 4)
	{
		for (; itr_desktop != e_global->desktopList.end(); itr_desktop++)
		{
			if (desktop_i == ((e_global->desktopList.size() + e_global->getSelectedIndex() - 1) % e_global->desktopList.size()))
				break;
			desktop_i++;
		}
	}

	for (; i != desktop_listnum; i++)
	{
		background_left = deSwitcher->initial_padding_width + deSwitcher->padding_width * (2 * (i + 1) - 1) + deSwitcher->switch_width * i + __max*((deSwitcher->switch_width + deSwitcher->padding_width * 2) / 2);
		background_right = background_left + deSwitcher->switch_width;
		RECT *backgroundRECT = new RECT
		{
			background_left,
			background_top,
			background_right,
			background_bottom,
		};;
		aeController->registerAero(hShellWnd, deSwitcher->m_hWnd, *backgroundRECT, pushThumbnail);
		deSwitcher->desktop_RECT_hthumbnail_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(backgroundRECT, pushThumbnail));
		deSwitcher->desktop_area_list_rect.push_back(backgroundRECT);
		
		RECT taskbarRECT =
		{
			background_left,
			taskbar_top,
			background_right,
			background_bottom
		};
		aeController->registerAero(hTaskbarWnd, deSwitcher->m_hWnd, taskbarRECT, pushThumbnail);


		list<RECT*> windowRECT_List;
		hash_map<RECT*, HTHUMBNAIL> pushThumbnail_Map;
		for (std::list<E_Window*>::iterator itr_window = (*itr_desktop)->windowList.begin(); itr_window != ((*itr_desktop)->windowList.end()); itr_window++) // iterating backward
		//for (std::list<HWND>::iterator itr_window = all_windows.begin(); itr_window != all_windows.end(); itr_window++)
		{
			if ((*itr_window)->isAeroPossible())
			{
				//double window_left = itr_window->getStartX() * ratio_ww + background_left,
				//window_top = itr_window->getStartY() * ratio_hh + background_top;
				//double window_right = itr_window->getWidth() * ratio_ww + window_left,
				//window_bottom = itr_window->getHeight() * ratio_hh + window_top;
				CRect getSize;
				GetWindowRect((*itr_window)->getWindow(), &getSize);
				//GetWindowRect(*itr_window, &getSize);
				double window_left = getSize.left * deSwitcher->ratio_ww + background_left,
					window_top = getSize.top * deSwitcher->ratio_hh + background_top;
				double window_right = getSize.Width() * deSwitcher->ratio_ww + window_left,
					window_bottom = getSize.Height() * deSwitcher->ratio_hh + window_top;
				RECT *windowRECT = new RECT
				{
					window_left,
					window_top,
					window_right,
					window_bottom
				};
				aeController->registerAero((*itr_window)->getWindow(), deSwitcher->m_hWnd, *windowRECT, pushThumbnail);

				windowRECT_List.push_front(windowRECT);
				pushThumbnail_Map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(windowRECT, pushThumbnail));
			}
		}
		deSwitcher->window_desktop_rect_map.insert(hash_map<int, list<RECT*>>::value_type(desktop_i, windowRECT_List));
		deSwitcher->window_desktop_RECT_hthumbnail_map.insert(hash_map<int, hash_map<RECT*, HTHUMBNAIL>>::value_type(desktop_i, pushThumbnail_Map));

		desktop_i = (desktop_i + 1) % e_global->desktopList.size();
		if (e_global->desktopList.size() <= 4)
		{
			itr_desktop++;
		}
		else
		{
			if (desktop_i == 0)
				itr_desktop = e_global->desktopList.begin();
			else
				itr_desktop++;
		}
	}
	
	drawWindowS();

	hdc = BeginPaint(*deSwitcher, &ps);
	EndPaint(*deSwitcher, &ps);
}

E_DesktopSwitcher::E_DesktopSwitcher()
{	
	ison = false;

	window_selected = false;
	window_squeezed = false;
	desktop_selected = false;
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
	
	if (!ison)
	{	
		E_Global* e_global = E_Global::getSingleton();
		if (e_global->desktopList.size() <= 4)
			desktoplist_startindex = 0;
		else
			desktoplist_startindex = (e_global->desktopList.size() + e_global->getSelectedIndex() - 1) % e_global->desktopList.size();
		
		/*CBrush m_oBkgndBrush;
		m_oBkgndBrush.CreateSolidBrush(RGB(255, 255, 255));
		UINT nClassStyle = CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		CString szClassName = AfxRegisterWndClass(nClassStyle, 0, (HBRUSH)m_oBkgndBrush.GetSafeHandle(), 0);
		Create(szClassName, _T(""), WS_VISIBLE || WS_EX_TOPMOST, CRect(0, 0, enManager->getWidth(), enManager->getHeight()), CWnd::GetDesktopWindow(), 1234);*/
		CBrush brush_window;
		UINT nClassStyle_window = 0;// CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		CString szClassName_window = AfxRegisterWndClass(nClassStyle_window, 0, (HBRUSH)CreateSolidBrush(E_WindowSwitcher::backgroundColor), 0);
		CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_window, L"DesktopSwitcher", WS_VISIBLE | WS_POPUP, CRect(0, 0, enManager->getWidth(), enManager->getHeight()), CWnd::GetDesktopWindow(), 0);
		E_Window::setIconInvisible(this->m_hWnd);
		
		drawDesktopSwitcher();
		UpdateWindow();
		ison = true;
		
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
	/*for (std::list<HTHUMBNAIL>::iterator itr = window_hthumbnail_list.begin(); itr != window_hthumbnail_list.end(); itr++)
	{
		e_aeropeekcontroller->unregisterAero(*itr);
	}*/
	//window_hthumbnail_list.clear();
	
	E_Window::setIconVisible(this->m_hWnd);
	DestroyWindow();
	ison = false;
}

void E_DesktopSwitcher::switchDesktop(E_Desktop* selection)
{

}

BEGIN_MESSAGE_MAP(E_DesktopSwitcher, CWnd)
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


void moveWindow(HWND target, int x, int y, int width, int height)
{
	MoveWindow(target, 0, 0, width, height, TRUE);
	
	ShowWindow(target, SW_NORMAL);
	UpdateWindow(target);
}

void bringWindowToTop(HWND hwnd)
{
	BringWindowToTop(hwnd);
}


int testflag = 0;
void E_DesktopSwitcher::OnLButtonDown(UINT nFlags, CPoint point)
{
	E_Global* e_global = E_Global::getSingleton();

	int i = desktoplist_startindex;
	for (std::list<RECT*>::iterator itr_desktop_area = desktop_area_list_rect.begin(); itr_desktop_area != desktop_area_list_rect.end(); itr_desktop_area++)
	{
		if (((CRect)(*itr_desktop_area)).PtInRect(point) && (i != e_global->getSelectedIndex()))
		{
			desktop_selected = true;
			target_desktop_index = i;
			return;
		}
		i = (i + 1) % e_global->desktopList.size();
	}
	
	window_squeezed = false;
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();

	//initialize swappoint_h
	swappoint_h = (((enManager->getWidth() - 100)*0.175) / enManager->getWidth()) * enManager->getHeight() + 100 * ((double)enManager->getHeight() / (double)enManager->getWidth());
	if (!window_selected)
	{
		std::list<RECT*> window_desktop_rect_list = window_desktop_rect_map.find(e_global->getSelectedIndex())->second;
		std::list<RECT*>::iterator itr_window_desktop_area = window_desktop_rect_list.begin();
		// window 공간들 iterating
		for (std::list<RECT*>::iterator itr_window_area = window_area_list_rect.begin(); itr_window_area != window_area_list_rect.end(); itr_window_area++)
		{
			double areasize = ((*itr_window_area)->right - (*itr_window_area)->left)*((*itr_window_area)->bottom - (*itr_window_area)->top);
			if ((((CRect)(*itr_window_area)).PtInRect(point)) && !(areasize == this->getMainDesktopSize()))
			{
				window_selected = true;
				window_leftdown_point = point;
				
				hash_map<RECT*, E_Window*>::iterator itr_window_EWindow = window_area_map_RECT_EWindow.find(*itr_window_area);
				hash_map<RECT*, HTHUMBNAIL>::iterator itr_window_hthumbnail = window_RECT_hthumbnail_map.find(*itr_window_area);
				hash_map<RECT*, HTHUMBNAIL> window_desktop_RECT_hthumbnail_map__ = window_desktop_RECT_hthumbnail_map.find(e_global->getSelectedIndex())->second;
				hash_map<RECT*, HTHUMBNAIL>::iterator itr_window_desktop_hthumbnail = window_desktop_RECT_hthumbnail_map__.find(*itr_window_desktop_area);

				bringWindowToTop(itr_window_EWindow->second->getWindow());
				BringWindowToTop();

				window_ptr = itr_window_EWindow->second;
				window_RECT = *itr_window_area;
				window_RECT_copy = new RECT(*window_RECT);
				window_area_list_rect.remove(*itr_window_area);
				window_area_list_rect.push_front(window_RECT_copy);
				window_area_map_RECT_EWindow.erase(itr_window_EWindow);
				window_area_map_RECT_EWindow.insert(hash_map<RECT*, E_Window*>::value_type(window_RECT_copy,window_ptr));

				aeController->unregisterAero(itr_window_hthumbnail->second);
				window_RECT_hthumbnail_map.erase(itr_window_hthumbnail);
				aeController->registerAero(window_ptr->getWindow(), this->m_hWnd, *window_RECT_copy, window_hthumbnail);
				window_RECT_hthumbnail_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(window_RECT_copy, window_hthumbnail));

				window_RECT_from_desktop = *itr_window_desktop_area;
				window_desktop_rect_list.remove(*itr_window_desktop_area);
				window_desktop_rect_list.push_front(window_RECT_from_desktop);
				window_desktop_rect_map.erase(window_desktop_rect_map.find(e_global->getSelectedIndex()));
				window_desktop_rect_map.insert(hash_map<int, list<RECT*>>::value_type(e_global->getSelectedIndex(), window_desktop_rect_list));

				aeController->unregisterAero(itr_window_desktop_hthumbnail->second);
				window_desktop_RECT_hthumbnail_map__.erase(itr_window_desktop_hthumbnail);
				aeController->registerAero(window_ptr->getWindow(), this->m_hWnd, *window_RECT_from_desktop, window_hthumbnail_from_desktop);
				window_desktop_RECT_hthumbnail_map__.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(window_RECT_from_desktop, window_hthumbnail_from_desktop));
				window_desktop_RECT_hthumbnail_map.erase(window_desktop_RECT_hthumbnail_map.find(e_global->getSelectedIndex()));
				window_desktop_RECT_hthumbnail_map.insert(hash_map<int, hash_map<RECT*, HTHUMBNAIL>>::value_type(e_global->getSelectedIndex(), window_desktop_RECT_hthumbnail_map__));

				return;
			}
			itr_window_desktop_area++;
		}
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}


void E_DesktopSwitcher::OnLButtonUp(UINT nFlags, CPoint point)
{
	E_Global* e_global = E_Global::getSingleton();
	if (desktop_selected)
	{
		desktop_selected = false;
		int i = desktoplist_startindex;
		for (std::list<RECT*>::iterator itr_desktop_area = desktop_area_list_rect.begin(); itr_desktop_area != desktop_area_list_rect.end(); itr_desktop_area++)
		{
			if (((CRect)(*itr_desktop_area)).PtInRect(point) && (i == target_desktop_index))
			{
				eraseWindowS();
				e_global->setSelectedIndex(i);
				drawWindowS();
				return;
			}
			i = (i + 1) % e_global->desktopList.size();
		}
	}
				
	if (window_selected)
	{
		window_selected = false;
		window_squeezed = false;
							
		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();

		hash_map<RECT*, E_Window*>::iterator itr_window_EWindow = window_area_map_RECT_EWindow.find(window_RECT_copy);
		hash_map<RECT*, HTHUMBNAIL>::iterator itr_window_hthumbnail = window_RECT_hthumbnail_map.find(window_RECT_copy);
		hash_map<RECT*, HTHUMBNAIL> window_desktop_RECT_hthumbnail_map__ = window_desktop_RECT_hthumbnail_map.find(e_global->getSelectedIndex())->second;
		hash_map<RECT*, HTHUMBNAIL>::iterator itr_window_desktop_hthumbnail = window_desktop_RECT_hthumbnail_map__.find(window_RECT_from_desktop);
		list<RECT*> window_desktop_rect_list = window_desktop_rect_map.find(e_global->getSelectedIndex())->second;
		
		std::list<E_Desktop*>::iterator itr_desktop_from = e_global->desktopList.begin();
		int j = 0;
		for (; itr_desktop_from!= e_global->desktopList.end(); itr_desktop_from++)
		{
			if (j == e_global->getSelectedIndex())
				break;
			j++;
		}
		
		int i = desktoplist_startindex;

		for (std::list<RECT*>::iterator itr_desktop_rect = desktop_area_list_rect.begin(); itr_desktop_rect != desktop_area_list_rect.end(); itr_desktop_rect++)
		{
			bool yesOverlap = ((CRect)*itr_desktop_rect).PtInRect(point);
			if (yesOverlap)
			{
				if (i != e_global->getSelectedIndex())
				{
					std::list<E_Desktop*>::iterator itr_desktop = e_global->desktopList.begin();
					for (j = 0; itr_desktop != e_global->desktopList.end(); itr_desktop++)
					{
						if (j == i)
							break;
						j++;
					}

					(*itr_desktop_from)->windowList.remove(window_ptr);
					(*itr_desktop)->windowList.push_back(window_ptr);

					if (abs(i - e_global->getSelectedIndex()) >= 4)
					{
						(*window_RECT_from_desktop).left += ((enManager->getWidth() - 100) / 4) * (i - e_global->desktopList.size() - e_global->getSelectedIndex());
						(*window_RECT_from_desktop).right += ((enManager->getWidth() - 100) / 4) * (i - e_global->desktopList.size() - e_global->getSelectedIndex());
					}
					else
					{
						(*window_RECT_from_desktop).left += ((enManager->getWidth() - 100) / 4) * (i - e_global->getSelectedIndex());
						(*window_RECT_from_desktop).right += ((enManager->getWidth() - 100) / 4) * (i - e_global->getSelectedIndex());
					}
					winThumbProps.rcDestination = *window_RECT_from_desktop;
					DwmUpdateThumbnailProperties(window_hthumbnail, &winThumbProps);
					
					aeController->unregisterAero(window_hthumbnail_from_desktop);
					//aeController->unregisterAero(itr_window_desktop_hthumbnail->second);
					
					window_RECT_hthumbnail_map.erase(itr_window_hthumbnail);
					
					hash_map<RECT*, HTHUMBNAIL> window_desktop_RECT_hthumbnail_map_ = window_desktop_RECT_hthumbnail_map.find(i)->second;
					window_desktop_RECT_hthumbnail_map_.insert(*itr_window_desktop_hthumbnail);
					window_desktop_RECT_hthumbnail_map.erase(window_desktop_RECT_hthumbnail_map.find(i));
					window_desktop_RECT_hthumbnail_map.insert(hash_map<int, hash_map<RECT*, HTHUMBNAIL>>::value_type(i, window_desktop_RECT_hthumbnail_map_));
					window_desktop_RECT_hthumbnail_map__.erase(itr_window_desktop_hthumbnail);
					window_desktop_RECT_hthumbnail_map.erase(window_desktop_RECT_hthumbnail_map.find(e_global->getSelectedIndex()));
					window_desktop_RECT_hthumbnail_map.insert(hash_map<int, hash_map<RECT*, HTHUMBNAIL>>::value_type(e_global->getSelectedIndex(), window_desktop_RECT_hthumbnail_map__));
					
					window_area_map_RECT_EWindow.erase(itr_window_EWindow);

					window_area_list_rect.remove(*window_area_list_rect.begin());

					list<RECT*> window_desktop_rect_list__ = window_desktop_rect_map.find(i)->second;
					window_desktop_rect_list__.push_back(*window_desktop_rect_list.begin());
					window_desktop_rect_map.erase(window_desktop_rect_map.find(i));
					window_desktop_rect_map.insert(hash_map<int, list<RECT*>>::value_type(i, window_desktop_rect_list__));
					window_desktop_rect_list.remove(*window_desktop_rect_list.begin());
					window_desktop_rect_map.erase(window_desktop_rect_map.find(e_global->getSelectedIndex()));
					window_desktop_rect_map.insert(hash_map<int, list<RECT*>>::value_type(e_global->getSelectedIndex(), window_desktop_rect_list));

					return;
				}
				break;
			}
			i = (i + 1) % e_global->desktopList.size();
		}
		winThumbProps.rcDestination = *window_RECT;
		DwmUpdateThumbnailProperties(window_hthumbnail, &winThumbProps);
		
		window_area_list_rect.remove(*window_area_list_rect.begin());
		window_area_list_rect.push_front(window_RECT);
		E_Window* RECT_EWindow_copy = itr_window_EWindow->second;
		HTHUMBNAIL RECT_HTHUMBNAIL_copy = itr_window_hthumbnail->second;
		window_area_map_RECT_EWindow.erase(itr_window_EWindow);
		window_area_map_RECT_EWindow.insert(hash_map<RECT*, E_Window*>::value_type(window_RECT, RECT_EWindow_copy));
		window_RECT_hthumbnail_map.erase(itr_window_hthumbnail);
		window_RECT_hthumbnail_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(window_RECT, RECT_HTHUMBNAIL_copy));
	}
	
	CWnd::OnLButtonUp(nFlags, point);
}


void E_DesktopSwitcher::OnMouseMove(UINT nFlags, CPoint point)
{
	if (((GetKeyState(VK_LBUTTON) & 0x80) != 0) && window_selected)
	{
		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		LONG diff_x = point.x - window_leftdown_point.x,
			diff_y = point.y - window_leftdown_point.y;

		(*window_RECT_copy).left = (*window_RECT_copy).left + diff_x;
		(*window_RECT_copy).right = (*window_RECT_copy).right + diff_x;
		(*window_RECT_copy).top = (*window_RECT_copy).top + diff_y;
		(*window_RECT_copy).bottom = (*window_RECT_copy).bottom + diff_y;

		winThumbProps.rcDestination = *window_RECT_copy;
		DwmUpdateThumbnailProperties(window_hthumbnail, &winThumbProps);

		window_leftdown_point = point;

		if (!window_squeezed)
		{
			if ((double)(*window_RECT_copy).top < swappoint_h - 50)
			{
				window_squeezed = true;

				double ratio_wW = (((double)enManager->getWidth() - 100) * (double)0.175) / main_desktop_width,
					ratio_hH = (((double)enManager->getWidth() - 100) * (double)0.175 * (double)enManager->getHeight() / (double)enManager->getWidth()) / main_desktop_height;

				(*window_RECT_copy).top = point.y - (point.y - (*window_RECT_copy).top) * ratio_hH;
				(*window_RECT_copy).bottom = point.y + ((*window_RECT_copy).bottom - point.y) * ratio_hH;
				(*window_RECT_copy).left = point.x - (point.x - (*window_RECT_copy).left) * ratio_wW;
				(*window_RECT_copy).right = point.x + ((*window_RECT_copy).right - point.x) * ratio_wW;

				winThumbProps.rcDestination = *window_RECT_copy;
				DwmUpdateThumbnailProperties(window_hthumbnail, &winThumbProps);
			}
		}

		return;
	}
	
	else if (window_selected)
	{
		window_selected = false;
		window_squeezed = false;

		hash_map<RECT*, E_Window*>::iterator itr_window_EWindow = window_area_map_RECT_EWindow.find(window_RECT_copy);
		hash_map<RECT*, HTHUMBNAIL>::iterator itr_window_hthumbnail = window_RECT_hthumbnail_map.find(window_RECT_copy);

		winThumbProps.rcDestination = *window_RECT;
		DwmUpdateThumbnailProperties(window_hthumbnail, &winThumbProps);

		window_area_list_rect.remove(*window_area_list_rect.begin());
		window_area_list_rect.push_front(window_RECT);
		E_Window* RECT_EWindow_copy = itr_window_EWindow->second;
		HTHUMBNAIL RECT_HTHUMBNAIL_copy = itr_window_hthumbnail->second;
		window_area_map_RECT_EWindow.erase(itr_window_EWindow);
		window_area_map_RECT_EWindow.insert(hash_map<RECT*, E_Window*>::value_type(window_RECT, RECT_EWindow_copy));
		window_RECT_hthumbnail_map.erase(itr_window_hthumbnail);
		window_RECT_hthumbnail_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(window_RECT, RECT_HTHUMBNAIL_copy));
	}
	

	CWnd::OnMouseMove(nFlags, point);
}
