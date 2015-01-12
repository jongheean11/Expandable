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

void drawDesktopSwitcher()
{
	E_Global* e_global = E_Global::getSingleton();
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_DesktopSwitcher* deSwitcher = E_DesktopSwitcher::getSingleton();
	E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();
	
	deSwitcher->target_desktop_index = 0;

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
	__max = e_global->desktopList.size() >= 4 ? 0 : 4 - e_global->desktopList.size();

	std::list<HWND> all_windows = e_global->getAllWindows();

	//아래부턴 Desktop 및 Window Manage 된 후 적용 가능한 코드.
	
	//Desktop 리스트 부분
	for (std::list<E_Desktop*>::iterator itr_desktop = e_global->desktopList.begin(); itr_desktop != e_global->desktopList.end(); itr_desktop++) //iterating list
	//for (;;)
	{
		background_left = initial_padding_width + padding_width * (2 * i - 1) + switch_width * (i - 1) + __max*((switch_width + padding_width * 2) / 2);
		background_right = background_left + switch_width;
		RECT *backgroundRECT = new RECT
		{
			background_left,
			background_top,
			background_right,
			background_bottom,
		};;
		aeController->registerAero(hShellWnd, deSwitcher->m_hWnd, *backgroundRECT, pushThumbnail);
		//deSwitcher->handle_list.push_back(pushThumbnail);
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
		//deSwitcher->handle_list.push_back(pushThumbnail);

		for (std::list<E_Window*>::iterator itr_window = (*itr_desktop)->windowList.begin(); itr_window != ((*itr_desktop)->windowList.end()); itr_window++) // iterating backward
		//for (std::list<HWND>::iterator itr_window = all_windows.begin(); itr_window != all_windows.end(); itr_window++)
		{
			//double window_left = itr_window->getStartX() * ratio_ww + background_left,
				//window_top = itr_window->getStartY() * ratio_hh + background_top;
			//double window_right = itr_window->getWidth() * ratio_ww + window_left,
				//window_bottom = itr_window->getHeight() * ratio_hh + window_top;
			CRect getSize;
			GetWindowRect((*itr_window)->getWindow(), &getSize);
			//GetWindowRect(*itr_window, &getSize);
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
			aeController->registerAero((*itr_window)->getWindow(), deSwitcher->m_hWnd, *windowRECT, pushThumbnail);
			//aeController->registerAero(*itr_window, deSwitcher->m_hWnd, *windowRECT, pushThumbnail);
			
			if (i - 1 == deSwitcher->target_desktop_index) // 선택된 Desktop section에 그려질 window 영역만 RECT 및 HTHUMBNAIL 저장
			{
				deSwitcher->window_desktop_rect_list.push_front(windowRECT);
				//deSwitcher->window_desktop_hthumbnail_list.push_front(pushThumbnail);
				deSwitcher->window_desktop_RECT_hthumbnail_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(windowRECT, pushThumbnail));
			}
		}

		i++;
		if (i == _max)
			break;
	}
	
	//Selected Desktop 부분
	background_bottom = 1080 - 0.15 * (1080 - (2 * initial_padding_height + switch_height));
	background_top = (2 * initial_padding_height + switch_height) + 0.05 * (1080 - (2 * initial_padding_height + switch_height));
	background_left = (enManager->getWidth() - (background_bottom-background_top) * ratio_wh) / 2;
	background_right = enManager->getWidth() - background_left;

	deSwitcher->main_desktop_width = background_right - background_left;
	deSwitcher->main_desktop_height = background_bottom - background_top;

	ratio_ww = (background_right - background_left) / (double)enManager->getWidth();
	ratio_hh = (background_bottom - background_top) / (double)enManager->getHeight();
	taskbar_top = background_bottom - sizeRect_taskbar.Height() * ratio_hh; // bottom, left, right는 background와 동일

	deSwitcher->setMainDesktopSize((background_right - background_left) * (background_bottom - background_top));

	RECT main_backgroundRECT
	{
		background_left,
		background_top,
		background_right,
		background_bottom,
	};
	aeController->registerAero(hShellWnd, deSwitcher->m_hWnd, main_backgroundRECT, pushThumbnail);
	//deSwitcher->handle_list.push_back(pushThumbnail);
	
	RECT taskbarRECT =
	{
		background_left,
		taskbar_top,
		background_right,
		background_bottom
	};
	aeController->registerAero(hTaskbarWnd, deSwitcher->m_hWnd, taskbarRECT, pushThumbnail);
	//deSwitcher->handle_list.push_back(pushThumbnail);


	//for (std::list<HWND>::iterator itr_window = all_windows.begin(); itr_window != all_windows.end(); itr_window++)
	std::list<E_Desktop*>::iterator desktop_temp = e_global->desktopList.begin();
	std::list<E_Window*> window_list = (*desktop_temp)->windowList;
	for (std::list<E_Window*>::iterator itr_window = window_list.begin(); itr_window != window_list.end(); itr_window++)
	{
		if ((*itr_window)->isAeroPossible())
		{
			getWindowText((*itr_window)->getWindow());
			//double window_left = itr_window->getStartX() * ratio_ww + background_left,
			//window_top = itr_window->getStartY() * ratio_hh + background_top;
			//double window_right = itr_window->getWidth() * ratio_ww + window_left,
			//window_bottom = itr_window->getHeight() * ratio_hh + window_top;
			CRect getSize;
			GetWindowRect((*itr_window)->getWindow(), &getSize);
			//GetWindowRect(*itr_window, &getSize);
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
			aeController->registerAero((*itr_window)->getWindow(), deSwitcher->m_hWnd, *windowRECT, pushThumbnail);
			//aeController->registerAero(*itr_window, deSwitcher->m_hWnd, *windowRECT, pushThumbnail);
			//deSwitcher->window_hthumbnail_list.push_front(pushThumbnail);
			deSwitcher->window_RECT_hthumbnail_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(windowRECT, pushThumbnail));

			deSwitcher->window_area_list_rect.push_front(windowRECT);
			//deSwitcher->window_area_list_hwnd.push_front(deSwitcher->m_hWnd);
			deSwitcher->window_area_map_RECT_EWindow.insert(hash_map<RECT*, E_Window*>::value_type(windowRECT, *itr_window));
		}
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
	window_squeezed = false;
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

	/*CBrush m_oBkgndBrush;
	m_oBkgndBrush.CreateSolidBrush(RGB(255, 255, 255));
	UINT nClassStyle = CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	CString szClassName = AfxRegisterWndClass(nClassStyle, 0, (HBRUSH)m_oBkgndBrush.GetSafeHandle(), 0);*/
	if (!ison)
	{	
		//Create(szClassName, _T(""), WS_VISIBLE || WS_EX_TOPMOST, CRect(0, 0, enManager->getWidth(), enManager->getHeight()), CWnd::GetDesktopWindow(), 1234); // nid 뭔지 꼭 찾기.
		CBrush brush_window;
		UINT nClassStyle_window = 0;// CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		CString szClassName_window = AfxRegisterWndClass(nClassStyle_window, 0, (HBRUSH)CreateSolidBrush(E_WindowSwitcher::backgroundColor), 0);
		CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_window, L"DesktopSwitcher", WS_VISIBLE | WS_POPUP, CRect(0, 0, enManager->getWidth(), enManager->getHeight()), CWnd::GetDesktopWindow(), 0);
		
		drawDesktopSwitcher();
		UpdateWindow();
		E_Window::setIconInvisible(this->m_hWnd);
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
/*
void E_DesktopSwitcher::drawOverview(E_Desktop* targetDesktop, E_Window* targetWindow)
{

}

void E_DesktopSwitcher::updateComponent(E_Desktop* targetDesktop, E_Window* targetWindow)
{

}
*/
	

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

bool testflag = false;
void E_DesktopSwitcher::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	/*desktop_i = 0;
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
		}*/
	
	window_squeezed = false;

	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_Global* e_global = E_Global::getSingleton();
	E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();

	int i = 0;
	
	//initialize swappoint_h
	swappoint_h = (((enManager->getWidth() - 100)*0.175) / enManager->getWidth()) * enManager->getHeight() + 100 * ((double)enManager->getHeight() / (double)enManager->getWidth());
	/*
	if (!window_selected)
	{
		// window 공간들 iterating
		int window_i = 0;
		std::list<E_Desktop*>::iterator desktop_current = e_global->desktopList.begin(); //TO-DO
		std::list<E_Window*>::reverse_iterator itr_window = (*desktop_current)->windowList.rbegin(); //TO-DO
		for (std::list<RECT*>::iterator itr_window_area = window_area_list_rect.begin(); itr_window_area != window_area_list_rect.end(); itr_window_area++)
		{
			while (!((*itr_window)->isAeroPossible()))
			{
				itr_window++;
			}
			double areasize = ((*itr_window_area)->right - (*itr_window_area)->left)*((*itr_window_area)->bottom - (*itr_window_area)->top);
			if ((((CRect)(*itr_window_area)).PtInRect(point)) && !(areasize == this->getMainDesktopSize()))
			{
				window_selected = true;
				window_leftdown_point = point;

				std::list<HTHUMBNAIL>::iterator itr_window_thumbnail = window_hthumbnail_list.begin();
				std::list<HWND>::iterator itr_window_hwnd = window_area_list_hwnd.begin();

				std::list<RECT*>::iterator itr_rect_from_desktop = window_desktop_rect_list.begin(); 
				std::list<HTHUMBNAIL>::iterator itr_hthumbnail_from_desktop = window_desktop_hthumbnail_list.begin(); //TO-DO
				
				for (int i = 0;; i++)
				{
					if (i == window_i)
					{
						bringWindowToTop((*itr_window)->getWindow());
//						BringWindowToTop();

						//E_Desktop* & _current(*desktop_current);
						window_ptr = *itr_window;
						(*desktop_current)->windowList.remove(*itr_window);
						(*desktop_current)->windowList.push_back(window_ptr);
						E_Desktop* new_desktop_current = *desktop_current;
						//_current = *desktop_current;
						
						window_RECT = *itr_window_area;
						window_RECT_copy = new RECT(*window_RECT);
						window_area_list_rect.remove(*itr_window_area);
						window_area_list_rect.push_front(window_RECT);

						aeController->unregisterAero(*itr_window_thumbnail);
						window_hthumbnail_list.remove(*itr_window_thumbnail);
						aeController->registerAero(window_ptr->getWindow(), this->m_hWnd, *window_RECT, window_hthumbnail);
						window_hthumbnail_list.push_front(window_hthumbnail);
						
						window_RECT_from_desktop = *itr_rect_from_desktop;
						window_desktop_rect_list.remove(*itr_rect_from_desktop);
						window_desktop_rect_list.push_front(window_RECT_from_desktop);
						
						aeController->unregisterAero(*itr_hthumbnail_from_desktop);
						window_desktop_hthumbnail_list.remove(*itr_hthumbnail_from_desktop);
						aeController->registerAero(window_ptr->getWindow(), this->m_hWnd, *window_RECT_from_desktop, window_hthumbnail_from_desktop);
						window_desktop_hthumbnail_list.push_front(window_hthumbnail_from_desktop);
						
						BringWindowToTop();
						return;
					}

					itr_window_thumbnail++;
					itr_window_hwnd++;
					itr_rect_from_desktop++;
					itr_hthumbnail_from_desktop++;
				}
			}
			window_i++;
		}
	}*/
	if (!window_selected)
	{
		// window 공간들 iterating
		
		std::list<RECT*>::iterator itr_window_desktop_area = window_desktop_rect_list.begin();
		for (std::list<RECT*>::iterator itr_window_area = window_area_list_rect.begin(); itr_window_area != window_area_list_rect.end(); itr_window_area++)
		{
			double areasize = ((*itr_window_area)->right - (*itr_window_area)->left)*((*itr_window_area)->bottom - (*itr_window_area)->top);
			if ((((CRect)(*itr_window_area)).PtInRect(point)) && !(areasize == this->getMainDesktopSize()))
			{
				window_selected = true;
				window_leftdown_point = point;
				
				hash_map<RECT*, E_Window*>::iterator itr_window_EWindow = window_area_map_RECT_EWindow.find(*itr_window_area);
				hash_map<RECT*, HTHUMBNAIL>::iterator itr_window_hthumbnail = window_RECT_hthumbnail_map.find(*itr_window_area);
				hash_map<RECT*, HTHUMBNAIL>::iterator itr_window_desktop_hthumbnail = window_desktop_RECT_hthumbnail_map.find(*itr_window_desktop_area);

				bringWindowToTop(itr_window_EWindow->second->getWindow());
				//BringWindowToTop();

				window_ptr = itr_window_EWindow->second;
				window_RECT = *itr_window_area;
				window_RECT_copy = new RECT(*window_RECT);
				window_area_list_rect.remove(*itr_window_area);
				window_area_list_rect.push_front(window_RECT_copy);
				window_area_map_RECT_EWindow.insert(itr_window_EWindow, hash_map<RECT*, E_Window*>::value_type(window_RECT_copy, itr_window_EWindow->second));

				aeController->unregisterAero(itr_window_hthumbnail->second);
				aeController->registerAero(window_ptr->getWindow(), this->m_hWnd, *window_RECT_copy, window_hthumbnail);
				window_RECT_hthumbnail_map.insert(itr_window_hthumbnail, hash_map<RECT*, HTHUMBNAIL>::value_type(window_RECT_copy, window_hthumbnail));

				window_RECT_from_desktop = *itr_window_desktop_area;
				window_desktop_rect_list.remove(*itr_window_desktop_area);
				window_desktop_rect_list.push_front(window_RECT_from_desktop);

				aeController->unregisterAero(itr_window_desktop_hthumbnail->second);
				aeController->registerAero(window_ptr->getWindow(), this->m_hWnd, *window_RECT_from_desktop, window_hthumbnail_from_desktop);
				window_desktop_RECT_hthumbnail_map.insert(itr_window_desktop_hthumbnail, hash_map<RECT*, HTHUMBNAIL>::value_type(window_RECT_from_desktop, window_hthumbnail_from_desktop));

				//BringWindowToTop();
				return;
			}
			itr_window_desktop_area++;
		}
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}


void E_DesktopSwitcher::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	/*
	if (window_selected)
	{
		if (window_squeezed)
		{
			E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
			E_Global* e_global = E_Global::getSingleton();
			
			int i = 0;
			double bottom_edge = (50 * (double)enManager->getHeight() / (double)enManager->getWidth()) + (((double)enManager->getWidth() - 100) * (double)0.175 * (double)enManager->getHeight() / (double)enManager->getWidth());
			if (bottom_edge > ((*window_RECT_copy).top + 20))
			{
				std::list<E_Desktop*>::iterator itr_desktop = e_global->desktopList.begin();
				std::list<E_Desktop*>::iterator itr_desktop_from = e_global->desktopList.begin();

				i = 0;
				while (i != target_desktop_index)
				{
					itr_desktop_from++;
					i++;
				}
				
				i = 0;
				for (std::list<RECT*>::iterator itr_desktop_rect = desktop_area_list_rect.begin(); itr_desktop_rect != desktop_area_list_rect.end(); itr_desktop_rect++)
				{
					//bool noOverlap = (*itr_desktop_rect)->left > (*window_RECT_copy).right ||
					//	(*window_RECT_copy).left > (*itr_desktop_rect)->right ||
					//	(*itr_desktop_rect)->top > (*window_RECT_copy).bottom ||
					//	(*window_RECT_copy).top > (*itr_desktop_rect)->bottom;
					
					bool yesOverlap = ((CRect)*itr_desktop_rect).PtInRect(point);
					if (yesOverlap)
					{
						if (i != target_desktop_index)
						{
							//E_Desktop* & _from(*itr_desktop_from);
							//E_Desktop* & _target(*itr_desktop);

							(*itr_desktop_from)->windowList.remove(window_ptr);
							(*itr_desktop)->windowList.push_front(window_ptr);
							int a = e_global->desktopList.size();
							//_from = *itr_desktop_from;

							a = e_global->desktopList.size();
							//_target = *itr_desktop;

							a = e_global->desktopList.size();
							window_hthumbnail_list.remove(window_hthumbnail);
							window_area_list_rect.remove(window_RECT);
							
							// 메모리 구조 제대로 안잡힘...
							(*window_RECT_from_desktop).left += ((enManager->getWidth() - 100) / 4) * (i - target_desktop_index);
							(*window_RECT_from_desktop).right += ((enManager->getWidth() - 100) / 4) * (i - target_desktop_index);
							winThumbProps.rcDestination = *window_RECT_from_desktop;
							DwmUpdateThumbnailProperties(window_hthumbnail, &winThumbProps);
							
							E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();
							aeController->unregisterAero(window_hthumbnail_from_desktop);
							//DwmUpdateThumbnailProperties(window_hthumbnail_from_desktop, &winThumbProps);

							window_desktop_rect_list.remove(window_RECT_from_desktop);
							window_desktop_hthumbnail_list.remove(window_hthumbnail_from_desktop);

							window_selected = false;
							window_squeezed = false;
							return;
						}
						break;
					}

					i++;
					itr_desktop++;
				}
				winThumbProps.rcDestination = *window_RECT;
				DwmUpdateThumbnailProperties(window_hthumbnail, &winThumbProps);
			}
			else
			{
				winThumbProps.rcDestination = *window_RECT;
				DwmUpdateThumbnailProperties(window_hthumbnail, &winThumbProps);
			}
		}
		else
		{
			winThumbProps.rcDestination = *window_RECT;
			DwmUpdateThumbnailProperties(window_hthumbnail, &winThumbProps);
		}
		
		window_selected = false;
		window_squeezed = false;
	}*/

	if (window_selected)
	{
		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		E_Global* e_global = E_Global::getSingleton();
		E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();

		int i = 0;
		
		hash_map<RECT*, E_Window*>::iterator itr_window_EWindow = window_area_map_RECT_EWindow.find(window_RECT_copy);
		hash_map<RECT*, HTHUMBNAIL>::iterator itr_window_hthumbnail = window_RECT_hthumbnail_map.find(window_RECT_copy);
		hash_map<RECT*, HTHUMBNAIL>::iterator itr_window_desktop_hthumbnail = window_desktop_RECT_hthumbnail_map.find(window_RECT_from_desktop);
		std::list<E_Desktop*>::iterator itr_desktop = e_global->desktopList.begin();
		std::list<E_Desktop*>::iterator itr_desktop_from = e_global->desktopList.begin();
		for (std::list<RECT*>::iterator itr_desktop_rect = desktop_area_list_rect.begin(); itr_desktop_rect != desktop_area_list_rect.end(); itr_desktop_rect++)
		{
			bool yesOverlap = ((CRect)*itr_desktop_rect).PtInRect(point);
			if (yesOverlap)
			{
				if (i != target_desktop_index)
				{
					(*itr_desktop_from)->windowList.remove(window_ptr);
					(*itr_desktop)->windowList.push_front(window_ptr);

					window_RECT_hthumbnail_map.erase(itr_window_hthumbnail);
					window_area_list_rect.remove(window_RECT_copy);
					// 메모리 구조 제대로 안잡힘...
					(*window_RECT_from_desktop).left += ((enManager->getWidth() - 100) / 4) * (i - target_desktop_index);
					(*window_RECT_from_desktop).right += ((enManager->getWidth() - 100) / 4) * (i - target_desktop_index);
					winThumbProps.rcDestination = *window_RECT_from_desktop;
					DwmUpdateThumbnailProperties(window_hthumbnail, &winThumbProps);
					
					aeController->unregisterAero(window_hthumbnail_from_desktop);
					
					window_desktop_rect_list.remove(window_RECT_from_desktop);
					window_desktop_RECT_hthumbnail_map.erase(itr_window_desktop_hthumbnail);

					window_selected = false;
					window_squeezed = false;
					return;
				}
				break;
			}
			i++;
			itr_desktop++;
		}
		winThumbProps.rcDestination = *window_RECT;
		aeController->unregisterAero(window_hthumbnail);
		aeController->registerAero(window_ptr->getWindow(), this->m_hWnd, *window_RECT, window_hthumbnail);
		window_RECT_hthumbnail_map.insert(itr_window_hthumbnail, hash_map<RECT*, HTHUMBNAIL>::value_type(window_RECT, window_hthumbnail));
		window_area_map_RECT_EWindow.insert(itr_window_EWindow, hash_map<RECT*, E_Window*>::value_type(window_RECT, itr_window_EWindow->second));
		window_area_list_rect.remove(window_RECT_copy);
		window_area_list_rect.push_front(window_RECT);
		
		window_selected = false;
		window_squeezed = false;
	}
	
	CWnd::OnLButtonUp(nFlags, point);
}


void E_DesktopSwitcher::OnMouseMove(UINT nFlags, CPoint point)
{
	// desktop 공간들 iterating
	
	if (((GetKeyState(VK_LBUTTON) & 0x80) != 0) && window_selected)
	{
		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		LONG diff_x = point.x - window_leftdown_point.x,
			diff_y = point.y - window_leftdown_point.y;

		(*window_RECT_copy).left = (*window_RECT_copy).left + diff_x;
		(*window_RECT_copy).right = (*window_RECT_copy).right + diff_x;
		(*window_RECT_copy).top = (*window_RECT_copy).top + diff_y;
		(*window_RECT_copy).bottom = (*window_RECT_copy).bottom + diff_y;

		// The destination rectangle size

		// Set the thumbnail properties for use
		
		winThumbProps.rcDestination = *window_RECT_copy;
		DwmUpdateThumbnailProperties(window_hthumbnail, &winThumbProps);

		window_leftdown_point = point;

		// TO-DO-LIST
		if (window_squeezed)
		{

		}
		else
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
	

	CWnd::OnMouseMove(nFlags, point);
}
