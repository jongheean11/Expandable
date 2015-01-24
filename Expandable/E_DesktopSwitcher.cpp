#include "stdafx.h"
#include "E_DesktopSwitcher.h"
#include "E_AeroPeekController.h"
#include <Windows.h> 
#include <WinUser.h>
#include <sstream>
#include <string>
#include "E_WindowSwitcher.h"
#define WM_TRAY_EVENT (WM_USER + 3)

void stealFocus(HWND hwnd)
{
	DWORD dwCurrentThread = GetCurrentThreadId();
	DWORD dwFGThread = GetWindowThreadProcessId(GetForegroundWindow(), NULL);

	AttachThreadInput(dwCurrentThread, dwFGThread, TRUE);

	// Possible actions you may wan to bring the window into focus.
	SetForegroundWindow(hwnd);
	SetCapture(hwnd);
	SetFocus(hwnd);
	SetActiveWindow(hwnd);
	EnableWindow(hwnd, TRUE);

	AttachThreadInput(dwCurrentThread, dwFGThread, FALSE);
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

void eraseDesktopList()
{
	E_Global* e_global = E_Global::getSingleton();
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_DesktopSwitcher* deSwitcher = E_DesktopSwitcher::getSingleton();
	E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();
	
	for (std::hash_map<RECT*, HTHUMBNAIL>::iterator itr_ = deSwitcher->desktop_RECT_hthumbnail_map.begin(); itr_ != deSwitcher->desktop_RECT_hthumbnail_map.end(); itr_++)
	{
		aeController->unregisterAero(itr_->second);
		delete itr_->first;
	}
	deSwitcher->desktop_RECT_hthumbnail_map.clear();

	for (std::hash_map<int, hash_map<RECT*, HTHUMBNAIL>>::iterator itr_ = deSwitcher->window_desktop_RECT_hthumbnail_map.begin(); itr_ != deSwitcher->window_desktop_RECT_hthumbnail_map.end(); itr_++)
	{
		for (std::hash_map<RECT*, HTHUMBNAIL>::iterator itr__ = itr_->second.begin(); itr__ != itr_->second.end(); itr__++)
		{
			aeController->unregisterAero(itr__->second);
			delete itr__->first;
		}
		itr_->second.clear();
	}
	deSwitcher->window_desktop_RECT_hthumbnail_map.clear();
	
	/*for (list<RECT*>::iterator itr_ = deSwitcher->desktop_area_list_rect.begin(); itr_ != deSwitcher->desktop_area_list_rect.end(); itr_++)
	{
		delete *itr_;
	}*/
	deSwitcher->desktop_area_list_rect.clear();
	
	for (hash_map<int, list<RECT*>>::iterator itr_ = deSwitcher->window_desktop_rect_map.begin(); itr_ != deSwitcher->window_desktop_rect_map.end(); itr_++)
	{
		/*for (list<RECT*>::iterator itr__ = itr_->second.begin(); itr__ != itr_->second.end(); itr__++)
		{
			delete *itr__;
		}*/
		itr_->second.clear();
	}
	deSwitcher->window_desktop_rect_map.clear();

	/*for (list<CWnd*>::iterator itr_ = deSwitcher->desktop_CWnd_list.begin(); itr_ != deSwitcher->desktop_CWnd_list.end(); itr_++)
	{
		::DestroyWindow((*itr_)->m_hWnd);
		delete *itr_;
	}
	deSwitcher->desktop_CWnd_list.clear();*/
}

void drawDesktopList()
{
	E_Global* e_global = E_Global::getSingleton();
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_DesktopSwitcher* deSwitcher = E_DesktopSwitcher::getSingleton();
	E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();

	HTHUMBNAIL pushThumbnail;

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

	deSwitcher->background_left = 0;
	deSwitcher->background_top = deSwitcher->initial_padding_height; // top은 변하지 않음.
	deSwitcher->background_right = 0;
	deSwitcher->background_bottom = deSwitcher->initial_padding_height + deSwitcher->switch_height; // bottom은 변하지 않음
	deSwitcher->taskbar_top = deSwitcher->background_bottom - deSwitcher->sizeRect_taskbar.Height() * deSwitcher->ratio_hh; // bottom, left, right는 background와 동일

	if (deSwitcher->leftarrow == NULL)
	{
		deSwitcher->leftarrow = new CRect(11 * enManager->getWidth() / 1280, 93 * enManager->getHeight() / 1024, 67 * enManager->getWidth() / 1280, 149 * enManager->getHeight() / 1024);
		deSwitcher->rightarrow = new CRect(1213 * enManager->getWidth() / 1280, 93 * enManager->getHeight() / 1024, 1269 * enManager->getWidth() / 1280, 149 * enManager->getHeight() / 1024);
	}

	int i = 0,
		desktop_i = 0,
		desktop_listnum = e_global->desktopList.size() >= 4 ? 4 : e_global->desktopList.size(),
		__max = e_global->desktopList.size() >= 4 ? 0 : 4 - e_global->desktopList.size();

	//Desktop 리스트 부분
	std::list<E_Desktop*>::iterator itr_desktop = e_global->desktopList.begin();
	if (e_global->desktopList.size() > 4)
	{
		for (; itr_desktop != e_global->desktopList.end(); itr_desktop++)
		{
			if (desktop_i == deSwitcher->desktoplist_startindex)
				break;
			desktop_i++;
		}
	}

	UINT nClassStyle_window = 0;
	CString szClassName_window = AfxRegisterWndClass(nClassStyle_window, 0, CreateSolidBrush(NULL_BRUSH), 0);
	
	bool list_exist = deSwitcher->desktop_CWnd_list.size() > 0 ? true : false;
	list<CWnd*>::iterator itr_CWnd;
	if (list_exist)
	{
		itr_CWnd = deSwitcher->desktop_CWnd_list.begin();
	}

	for (; i != desktop_listnum; i++)
	{		
		deSwitcher->background_left = deSwitcher->initial_padding_width + deSwitcher->padding_width * (2 * (i + 1) - 1) + deSwitcher->switch_width * i + __max*((deSwitcher->switch_width + deSwitcher->padding_width * 2) / 2);
		deSwitcher->background_right = deSwitcher->background_left + deSwitcher->switch_width;
		
		RECT *backgroundRECT = new RECT
		{
			deSwitcher->background_left,
			deSwitcher->background_top,
			deSwitcher->background_right,
			deSwitcher->background_bottom,
		};

		CWnd* pushCWnd;
		if (!list_exist)
		{
			pushCWnd = new CWnd;
			pushCWnd->CreateEx(WS_EX_TOPMOST, szClassName_window, L"E_DesktopSwitcher", WS_VISIBLE | WS_POPUP, *backgroundRECT, deSwitcher, 0);
		}
		else
		{
			pushCWnd = *itr_CWnd;
			itr_CWnd++;
		}

		//aeController->registerAero(deSwitcher->hShellWnd, deSwitcher->m_hWnd, *backgroundRECT, pushThumbnail);
		//aeController->registerAero(deSwitcher->hShellWnd, pushCWnd->m_hWnd, *backgroundRECT, pushThumbnail);
		aeController->registerAero(deSwitcher->hShellWnd, pushCWnd->m_hWnd, CRect(0, 0, backgroundRECT->right - backgroundRECT->left, backgroundRECT->bottom - backgroundRECT->top), pushThumbnail);
		deSwitcher->desktop_RECT_hthumbnail_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(backgroundRECT, pushThumbnail));
		deSwitcher->desktop_area_list_rect.push_back(backgroundRECT);

		for (list<HWND>::iterator itr_window_docked = e_global->dockedWindowList.begin(); itr_window_docked != e_global->dockedWindowList.end(); itr_window_docked++)
		{
			UINT state;
			WINDOWPLACEMENT windowinfo;
			GetWindowPlacement(*itr_window_docked, &windowinfo);
			UINT winstate = windowinfo.showCmd;
			BOOL isVisible = IsWindowVisible(*itr_window_docked);
			BOOL isMinimized = IsIconic(*itr_window_docked);
			if (!((state = winstate) == SW_FORCEMINIMIZE
				|| (state = winstate) == SW_HIDE		//HIDE는 사실 처리 안됨 (invisible)
				|| (state = winstate) == SW_MINIMIZE
				|| (state = winstate) == SW_SHOWMINIMIZED
				|| (state = winstate) == SW_SHOWMINNOACTIVE
				|| isVisible == FALSE
				|| isMinimized == TRUE)
				 && IsWindow(*itr_window_docked))
			{
				CRect getSize;
				GetWindowRect(*itr_window_docked, &getSize);
				double window_left = getSize.left * deSwitcher->ratio_ww,// + deSwitcher->background_left,
					window_top = getSize.top * deSwitcher->ratio_hh;// +deSwitcher->background_top;
				double window_right = getSize.Width() * deSwitcher->ratio_ww  + window_left,
					window_bottom = getSize.Height() * deSwitcher->ratio_hh + window_top;
				RECT *windowRECT = new RECT
				{
					window_left,
					window_top,
					window_right,
					window_bottom
				};
				//aeController->registerAero(*itr_window_docked, deSwitcher->m_hWnd, *windowRECT, pushThumbnail);
				aeController->registerAero(*itr_window_docked, pushCWnd->m_hWnd, *windowRECT, pushThumbnail);
			}
		}

		(*itr_desktop)->setAllIconInvisible();
		(*itr_desktop)->setAllShow();
		list<RECT*> windowRECT_List;
		hash_map<RECT*, HTHUMBNAIL> pushThumbnail_Map;
		std::list<E_Window*> window_list_topmost;
		for (std::list<E_Window*>::iterator itr_window = (*itr_desktop)->windowList.begin(); itr_window != ((*itr_desktop)->windowList.end()); itr_window++) // iterating backward
		{
			//(*itr_window)->setShow();
			if (((*itr_window)->isAeroPossible()) && IsWindow((*itr_window)->getWindow()) && (!(*itr_window)->dock))
			{
				DWORD dwExStyle = ::GetWindowLong((*itr_window)->getWindow(), GWL_EXSTYLE);
				if ((dwExStyle & WS_EX_TOPMOST) != 0)
				{
					window_list_topmost.push_front(*itr_window);
				}
				else
				{
					CRect getSize;
					GetWindowRect((*itr_window)->getWindow(), &getSize);
					double window_left = getSize.left * deSwitcher->ratio_ww,// + deSwitcher->background_left,
						window_top = getSize.top * deSwitcher->ratio_hh;// +deSwitcher->background_top;
					double window_right = getSize.Width() * deSwitcher->ratio_ww + window_left,
						window_bottom = getSize.Height() * deSwitcher->ratio_hh +window_top;
					RECT *windowRECT = new RECT
					{
						window_left,
						window_top,
						window_right,
						window_bottom
					};
					//aeController->registerAero((*itr_window)->getWindow(), deSwitcher->m_hWnd, *windowRECT, pushThumbnail);
					aeController->registerAero((*itr_window)->getWindow(), pushCWnd->m_hWnd, *windowRECT, pushThumbnail);

					windowRECT_List.push_front(windowRECT);
					pushThumbnail_Map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(windowRECT, pushThumbnail));
				}
			}
		}
		for (std::list<E_Window*>::iterator itr_window = window_list_topmost.begin(); itr_window != window_list_topmost.end(); itr_window++) // iterating backward
		{
			CRect getSize;
			GetWindowRect((*itr_window)->getWindow(), &getSize);
			double window_left = getSize.left * deSwitcher->ratio_ww,// + deSwitcher->background_left,
				window_top = getSize.top * deSwitcher->ratio_hh;// +deSwitcher->background_top;
			double window_right = getSize.Width() * deSwitcher->ratio_ww + window_left,
				window_bottom = getSize.Height() * deSwitcher->ratio_hh +window_top;
			RECT *windowRECT = new RECT
			{
				window_left,
				window_top,
				window_right,
				window_bottom
			};
			//aeController->registerAero((*itr_window)->getWindow(), deSwitcher->m_hWnd, *windowRECT, pushThumbnail);
			aeController->registerAero((*itr_window)->getWindow(), pushCWnd->m_hWnd, *windowRECT, pushThumbnail);

			windowRECT_List.push_front(windowRECT);
			pushThumbnail_Map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(windowRECT, pushThumbnail));
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

		RECT *taskbarRECT = new RECT
		{
			deSwitcher->background_left,
			deSwitcher->taskbar_top,
			deSwitcher->background_right,
			deSwitcher->background_bottom
		};
		//aeController->registerAero(deSwitcher->hTaskbarWnd, deSwitcher->m_hWnd, *taskbarRECT, pushThumbnail);
		//aeController->registerAero(deSwitcher->hTaskbarWnd, pushCWnd->m_hWnd, *taskbarRECT, pushThumbnail);
		aeController->registerAero(deSwitcher->hTaskbarWnd, pushCWnd->m_hWnd, CRect(0, taskbarRECT->top - backgroundRECT->top, taskbarRECT->right - taskbarRECT->left, taskbarRECT->bottom - backgroundRECT->top), pushThumbnail);
		deSwitcher->desktop_RECT_hthumbnail_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(taskbarRECT, pushThumbnail));

		deSwitcher->desktop_CWnd_list.push_back(pushCWnd);
	}
}

void eraseWindowS()
{
	E_DesktopSwitcher* deSwitcher = E_DesktopSwitcher::getSingleton();
	E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();

	for (list<RECT*>::iterator itr_rect = deSwitcher->window_area_list_rect.begin(); itr_rect != deSwitcher->window_area_list_rect.end(); itr_rect++)
	{
		aeController->unregisterAero(deSwitcher->window_RECT_hthumbnail_map.find(*itr_rect)->second);
		delete *itr_rect;
	}

	aeController->unregisterAero(deSwitcher->main_backgroundHTHUMBNAIL);
	aeController->unregisterAero(deSwitcher->main_taskbarHTHUMBNAIL);
	
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

	UINT nClassStyle_window = 0;
	CString szClassName_window = AfxRegisterWndClass(nClassStyle_window, 0, CreateSolidBrush(NULL_BRUSH), 0);

	e_global->getSelectedDesktop()->setAllIconVisible();

	//Selected Desktop 부분
	double background_bottom = enManager->getHeight() - 0.15 * (enManager->getHeight() - (2 * deSwitcher->initial_padding_height + deSwitcher->switch_height)),
		background_top = (2 * deSwitcher->initial_padding_height + deSwitcher->switch_height) + 0.05 * (enManager->getHeight() - (2 * deSwitcher->initial_padding_height + deSwitcher->switch_height));
	double background_left = (enManager->getWidth() - (background_bottom - background_top) * deSwitcher->ratio_wh) / 2;
	double background_right = enManager->getWidth() - background_left;

	deSwitcher->main_desktop_width = background_right - background_left;
	deSwitcher->main_desktop_height = background_bottom - background_top;

	deSwitcher->ratio_ww = (background_right - background_left) / (double)enManager->getWidth();
	deSwitcher->ratio_hh = (background_bottom - background_top) / (double)enManager->getHeight();
	double taskbar_top = background_bottom - deSwitcher->sizeRect_taskbar.Height() * deSwitcher->ratio_hh; // bottom, left, right는 background와 동일

	deSwitcher->setMainDesktopSize((background_right - background_left) * (background_bottom - background_top));

	deSwitcher->main_backgroundRECT = new RECT
	{
		background_left,
		background_top,
		background_right,
		background_bottom
	};

	if (deSwitcher->mainCWnd == NULL)
	{
		deSwitcher->mainCWnd = new CWnd;
		deSwitcher->mainCWnd->CreateEx(WS_EX_TOPMOST, szClassName_window, L"E_DesktopSwitcher", WS_VISIBLE | WS_POPUP, *deSwitcher->main_backgroundRECT, deSwitcher, 0);
	}

	aeController->registerAero(deSwitcher->hShellWnd, deSwitcher->mainCWnd->m_hWnd, CRect(0, 0, deSwitcher->main_backgroundRECT->right - deSwitcher->main_backgroundRECT->left, deSwitcher->main_backgroundRECT->bottom - deSwitcher->main_backgroundRECT->top), deSwitcher->main_backgroundHTHUMBNAIL);

	std::list<E_Desktop*>::iterator desktop_current = e_global->desktopList.begin();
	int desktop_i = 0;
	for (; desktop_current != e_global->desktopList.end(); desktop_current++)
	{
		if (desktop_i == e_global->getSelectedIndex())
			break;
		desktop_i++;
	}

	for (list<HWND>::iterator itr_window_docked = e_global->dockedWindowList.begin(); itr_window_docked != e_global->dockedWindowList.end(); itr_window_docked++)
	{
		UINT state;
		WINDOWPLACEMENT windowinfo;
		GetWindowPlacement(*itr_window_docked, &windowinfo);
		UINT winstate = windowinfo.showCmd;
		BOOL isVisible = IsWindowVisible(*itr_window_docked);
		BOOL isMinimized = IsIconic(*itr_window_docked);
		if (!((state = winstate) == SW_FORCEMINIMIZE
			|| (state = winstate) == SW_MINIMIZE
			|| (state = winstate) == SW_SHOWMINIMIZED
			|| (state = winstate) == SW_SHOWMINNOACTIVE
			|| isVisible == FALSE
			|| isMinimized == TRUE)
			&& IsWindow(*itr_window_docked))
		{
			CRect getSize;
			GetWindowRect(*itr_window_docked, &getSize);
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
			//aeController->registerAero(*itr_window_docked, deSwitcher->mainCWnd->m_hWnd, *windowRECT, pushThumbnail);
			aeController->registerAero(*itr_window_docked, deSwitcher->mainCWnd->m_hWnd, CRect(windowRECT->left - background_left, windowRECT->top - background_top, windowRECT->right - window_left, windowRECT->bottom - window_top), pushThumbnail);
		}
	}

	std::list<E_Window*> window_list = (*desktop_current)->windowList;
	std::list<E_Window*> window_list_topmost;
	for (std::list<E_Window*>::iterator itr_window = window_list.begin(); itr_window != window_list.end(); itr_window++)
	{
		if (((*itr_window)->isAeroPossible()) && IsWindow((*itr_window)->getWindow()) && (!(*itr_window)->dock))
		{
			DWORD dwExStyle = ::GetWindowLong((*itr_window)->getWindow(), GWL_EXSTYLE);
			if ((dwExStyle & WS_EX_TOPMOST) != 0)
			{
				window_list_topmost.push_front(*itr_window);
			}
			else
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
				//aeController->registerAero((*itr_window)->getWindow(), deSwitcher->mainCWnd->m_hWnd, CRect(windowRECT->left - background_left, windowRECT->top - background_top, windowRECT->right - window_left, windowRECT->bottom - window_top), pushThumbnail);
				aeController->registerAero((*itr_window)->getWindow(), deSwitcher->mainCWnd->m_hWnd, CRect(windowRECT->left - background_left, windowRECT->top - background_top, windowRECT->right - background_left, windowRECT->bottom - background_top), pushThumbnail);
				deSwitcher->window_RECT_hthumbnail_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(windowRECT, pushThumbnail));

				deSwitcher->window_area_list_rect.push_front(windowRECT);
				deSwitcher->window_area_map_RECT_EWindow.insert(hash_map<RECT*, E_Window*>::value_type(windowRECT, *itr_window));
			}
		}
	}
	for (std::list<E_Window*>::iterator itr_window = window_list_topmost.begin(); itr_window != window_list_topmost.end(); itr_window++)
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
		//aeController->registerAero((*itr_window)->getWindow(), deSwitcher->mainCWnd->m_hWnd, *windowRECT, pushThumbnail);
		aeController->registerAero((*itr_window)->getWindow(), deSwitcher->mainCWnd->m_hWnd, CRect(windowRECT->left - background_left, windowRECT->top - background_top, windowRECT->right - background_left, windowRECT->bottom - background_top), pushThumbnail);
		deSwitcher->window_RECT_hthumbnail_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(windowRECT, pushThumbnail));

		deSwitcher->window_area_list_rect.push_front(windowRECT);
		deSwitcher->window_area_map_RECT_EWindow.insert(hash_map<RECT*, E_Window*>::value_type(windowRECT, *itr_window));
	}

	deSwitcher->main_taskbarRECT = new RECT
	{
		background_left,
		taskbar_top,
		background_right,
		background_bottom
	};
	//aeController->registerAero(deSwitcher->hTaskbarWnd, deSwitcher->m_hWnd, *(deSwitcher->main_taskbarRECT), deSwitcher->main_taskbarHTHUMBNAIL);
	aeController->registerAero(deSwitcher->hTaskbarWnd, deSwitcher->mainCWnd->m_hWnd, CRect(0, deSwitcher->main_taskbarRECT->top - deSwitcher->main_backgroundRECT->top, deSwitcher->main_taskbarRECT->right - deSwitcher->main_taskbarRECT->left, deSwitcher->main_taskbarRECT->bottom - deSwitcher->main_backgroundRECT->top), deSwitcher->main_taskbarHTHUMBNAIL);
}

void drawDesktopSwitcher()
{
	E_Global* e_global = E_Global::getSingleton();
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_DesktopSwitcher* deSwitcher = E_DesktopSwitcher::getSingleton();
	E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();
	
	PAINTSTRUCT ps;
	HDC hdc;
	
	deSwitcher->hShellWnd = GetShellWindow(); // 바탕화면 hwnd
	GetWindowRect(deSwitcher->hShellWnd, &(deSwitcher->sizeRect_background)); // 바탕화면 크기 얻기
	deSwitcher->hTaskbarWnd = FindWindowW(_T("Shell_TrayWnd"), NULL); // 작업표시줄 hwnd
	GetWindowRect(deSwitcher->hTaskbarWnd, &(deSwitcher->sizeRect_taskbar)); // 작업표시줄 크기 얻기
	
	drawDesktopList();
	
	drawWindowS();

	hdc = BeginPaint(*deSwitcher, &ps);
	EndPaint(*deSwitcher, &ps);

}

void E_DesktopSwitcher::updateSelectedDesktop()
{
	//업데이트가 발생한 경우 자동으로 호출됨
	TRACE_WIN32A("[E_DesktopSwitcher::updateSelectedDesktop()]");
	if (ison)
	{
		eraseDesktopList();
		eraseWindowS();
		drawDesktopList();
		drawWindowS();
	}
}

E_DesktopSwitcher::E_DesktopSwitcher()
{	
	initial = true;

	leftkey_pressed = false;
	rightkey_pressed = false;
	enterkey_pressed = false;
	esckey_pressed = false;
	leftarrow = NULL; 
	rightarrow = NULL;
	otherkey = false;

	mainCWnd = NULL;
	window_squeezed_inlist = false;

	restore = false;
	ison = false;
	doubleclick_first = false;
	doubleclick_second = false;
	desktop_inrange = true;
	leftarrow_pressed = false;
	rightarrow_pressed = false;
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
	initial = false;
}

E_DesktopSwitcher* E_DesktopSwitcher::singleton = NULL;

void E_DesktopSwitcher::startSwitcher()
{	
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	
	if (!ison)
	{
		E_Global* e_global = E_Global::getSingleton();

		initindex = e_global->getSelectedIndex();

		e_global->onUpdate();

		if (e_global->desktopList.size() <= 4)
			desktoplist_startindex = 0;
		else
			desktoplist_startindex = (e_global->desktopList.size() + e_global->getSelectedIndex() - 1) % e_global->desktopList.size();

		UINT nClassStyle_window = 0;// CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		if (initial)
		{
			initial = false;
			HBITMAP hbmOrig;
			if (enManager->getWidth() == 1920 && enManager->getHeight() == 1080)
			{
				if (e_global->desktopList.size() > 4)
					hbmOrig = (HBITMAP)LoadImage(IMAGE_BITMAP, __T("res\\DesktopSwitcher_background19201080_witharrow.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
				else
					hbmOrig = (HBITMAP)LoadImage(IMAGE_BITMAP, __T("res\\DesktopSwitcher_background19201080.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			}
			else if (enManager->getWidth() == 1600 && enManager->getHeight() == 900)
			{
				if (e_global->desktopList.size() > 4)
					hbmOrig = (HBITMAP)LoadImage(IMAGE_BITMAP, __T("res\\DesktopSwitcher_background1600900_witharrow.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
				else
					hbmOrig = (HBITMAP)LoadImage(IMAGE_BITMAP, __T("res\\DesktopSwitcher_background1600900.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			}
			else if (enManager->getWidth() == 1024 && enManager->getHeight() == 768)
			{
				if (e_global->desktopList.size() > 4)
					hbmOrig = (HBITMAP)LoadImage(IMAGE_BITMAP, __T("res\\DesktopSwitcher_background1024768_witharrow.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
				else
					hbmOrig = (HBITMAP)LoadImage(IMAGE_BITMAP, __T("res\\DesktopSwitcher_background1024768.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			}
			else if (enManager->getWidth() == 1280 && enManager->getHeight() == 1024)
			{
				if (e_global->desktopList.size() > 4)
					hbmOrig = (HBITMAP)LoadImage(IMAGE_BITMAP, __T("res\\DesktopSwitcher_background12801024_witharrow.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
				else
					hbmOrig = (HBITMAP)LoadImage(IMAGE_BITMAP, __T("res\\DesktopSwitcher_background12801024.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			}
			else // 1280x720
			{
				if (e_global->desktopList.size() > 4)
					hbmOrig = (HBITMAP)LoadImage(IMAGE_BITMAP, __T("res\\DesktopSwitcher_background1280720_witharrow.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
				else
					hbmOrig = (HBITMAP)LoadImage(IMAGE_BITMAP, __T("res\\DesktopSwitcher_background1280720.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			}

			BITMAP bm = { 0 };
			GetObject(hbmOrig, sizeof(BITMAP), &bm);

			HDC dc = ::GetDC(::GetDesktopWindow());
			HDC memdc = CreateCompatibleDC(dc);
			HDC memdc2 = CreateCompatibleDC(dc);
			HBITMAP hbm = CreateCompatibleBitmap(dc, enManager->getWidth(), enManager->getHeight());
			HGDIOBJ hOld = SelectObject(memdc, hbm);
			SelectObject(memdc2, hbmOrig);
			StretchBlt(memdc, 0, 0, enManager->getWidth(), enManager->getHeight(), memdc2, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
			::ReleaseDC(::GetDesktopWindow(), dc);


			SelectObject(memdc, hOld);
			DeleteObject(hbmOrig);
			DeleteDC(memdc2);
			DeleteObject(hOld);
			DeleteDC(memdc);
			DeleteDC(dc);

			HBRUSH fillBrush;
			fillBrush = ::CreatePatternBrush(hbm);
			DeleteObject(hbm);

			CString szClassName_window = AfxRegisterWndClass(nClassStyle_window, 0, fillBrush, 0);
			CreateEx(WS_EX_TOPMOST, szClassName_window, L"E_DesktopSwitcher", WS_VISIBLE | WS_POPUP, CRect(0, 0, enManager->getWidth(), enManager->getHeight()), CWnd::GetDesktopWindow(), 0);
		}
		UpdateWindow();
		//WS_EX_TOPMOST
		DwmEnableComposition(DWM_EC_ENABLECOMPOSITION);

		E_Window::setIconInvisible(this->m_hWnd);
		
		//e_global->stopUpdate();
		drawDesktopSwitcher();

		::SetWindowLongW(hTaskbarWnd, GWL_EXSTYLE, GetWindowLong(hTaskbarWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		::SetLayeredWindowAttributes(hTaskbarWnd, 0, 0, LWA_ALPHA);
		
		SetLayeredWindowAttributes(0, 255, LWA_ALPHA); //투명해제
		SetWindowLongW(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) && ~WS_EX_LAYERED);
		Invalidate(TRUE);
		UpdateWindow();
		ison = true;
		
		ShowWindow(SW_SHOWMAXIMIZED);
		Invalidate(FALSE);

		SetCapture();

		stealFocus(this->m_hWnd);

		restore = true;
		//fillBrush.DeleteObject();
		//cb->DeleteObject();

	}
	else
	{
		terminateSwitcher();
	}
}

void E_DesktopSwitcher::terminateSwitcher()
{
	if (restore)
	{
		if (initindex != E_Global::getSingleton()->getSelectedIndex())
		{
			E_Global::getSingleton()->getSelectedDesktop()->setAllIconInvisible();
			E_Global::getSingleton()->setSelectedIndex(initindex);
		}
		restore = false;
	}

	if (ison)
	{
		E_Global* e_global = E_Global::getSingleton();
		E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();

		for (list<E_Desktop*>::iterator itr_desktop = e_global->desktopList.begin(); itr_desktop != e_global->desktopList.end(); itr_desktop++)
		{
			(*itr_desktop)->setAllIconVisible();
			(*itr_desktop)->setAllHide();
		}
		for (list<HWND>::iterator itr_window_docked = e_global->dockedWindowList.begin(); itr_window_docked != e_global->dockedWindowList.end(); itr_window_docked++)
		{
			::ShowWindow(*itr_window_docked, SW_SHOW);
		}
		for (list<E_Window*>::reverse_iterator itr = e_global->getSelectedDesktop()->windowList.rbegin(); itr != e_global->getSelectedDesktop()->windowList.rend(); itr++)
		{
			if (!(*itr)->dock)
				(*itr)->setShow();
		}

		::SetLayeredWindowAttributes(hTaskbarWnd, 0, 255, LWA_ALPHA); //투명해제
		::SetWindowLongW(hTaskbarWnd, GWL_EXSTYLE, GetWindowLong(hTaskbarWnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);

		delete leftarrow;
		delete rightarrow;

		leftarrow = NULL;
		rightarrow = NULL;

		eraseDesktopList();
		eraseWindowS();

		window_squeezed_inlist = false;
		
		doubleclick_first = false;
		doubleclick_second = false;
		desktop_inrange = true;
		leftarrow_pressed = false;
		rightarrow_pressed = false;
		window_selected = false;
		window_squeezed = false;
		desktop_selected = false;

		aeController->unregisterAllAreo();

		window_area_list_rect.clear();
		window_area_map_RECT_EWindow.clear();
		window_desktop_RECT_hthumbnail_map.clear();
//		window_desktop_rect_map.clear();
		window_RECT_hthumbnail_map.clear();
		desktop_area_list_rect.clear();
		desktop_RECT_hthumbnail_map.clear();
		
		/*for (hash_map<int, list<RECT*>>::iterator itr_ = window_desktop_rect_map.begin(); itr_ != window_desktop_rect_map.end(); itr_++)
		{
			for (list<RECT*>::iterator itr__ = itr_->second.begin(); itr__ != itr_->second.end(); itr__++)
			{
				delete *itr__;
			}
			itr_->second.clear();
		}*/
		window_desktop_rect_map.clear();

		for (list<CWnd*>::iterator itr_ = desktop_CWnd_list.begin(); itr_ != desktop_CWnd_list.end(); itr_++)
		{
			::DestroyWindow((*itr_)->m_hWnd);
		}
		desktop_CWnd_list.clear();

		delete mainCWnd;
		mainCWnd = NULL;
		
		ReleaseCapture();

		leftkey_pressed = false;
		rightkey_pressed = false;
		enterkey_pressed = false;
		esckey_pressed = false;

		E_Window::setIconVisible(this->m_hWnd);
		ison = false;
		//DestroyWindow();
		SetWindowLongW(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(0, 0, LWA_ALPHA);
		::SendMessage(e_global->hwnd_frame, WM_TRAY_EVENT, e_global->getSelectedIndex(), 0);

		otherkey = false;
	}
}

void E_DesktopSwitcher::switchDesktop(E_Desktop* selection)
{

}

BEGIN_MESSAGE_MAP(E_DesktopSwitcher, CWnd)
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
ON_WM_PAINT()
ON_WM_DESTROY()
ON_WM_CREATE()
ON_WM_KEYDOWN()
ON_WM_KEYUP()
ON_WM_KILLFOCUS()
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
	restore = false;

	E_Global* e_global = E_Global::getSingleton();

	//e_global->stopUpdate();

	GetLocalTime(&doubleClickTimer);

	if (doubleclick_first)
	{
		WORD ms = doubleClickTimer.wMilliseconds - __doubleClickTimer.wMilliseconds;
		WORD s = doubleClickTimer.wSecond - __doubleClickTimer.wSecond;
		if (s*(WORD)1000 + doubleClickTimer.wMilliseconds - __doubleClickTimer.wMilliseconds <= (WORD)100)
		{
			doubleclick_second = true;
		}
		else
		{
			doubleclick_first = false;
			doubleclick_second = false;
		}
	}
	else
	{
		doubleclick_first = false;
		doubleclick_second = false;
	}
	
	if (e_global->desktopList.size() > 4)
	{
		if (leftarrow->PtInRect(point))
		{
			leftarrow_pressed = true;
			
			restore = true;
			return;
		}
		else if (rightarrow->PtInRect(point))
		{
			rightarrow_pressed = true;

			restore = true;
			return;
		}
	}

	int i = desktoplist_startindex;
	for (std::list<RECT*>::iterator itr_desktop_area = desktop_area_list_rect.begin(); itr_desktop_area != desktop_area_list_rect.end(); itr_desktop_area++)
	{
		if (((CRect)(*itr_desktop_area)).PtInRect(point) && (i != e_global->getSelectedIndex()))
		{
			desktop_selected = true;
			target_desktop_index = i;

			stealFocus(this->m_hWnd);

			restore = true;
			return;
		}
		i = (i + 1) % e_global->desktopList.size();
	}
	
	window_squeezed = false;
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();

	//initialize swappoint_h
	//swappoint_h = (((enManager->getWidth() - 100)*0.175) / enManager->getWidth()) * enManager->getHeight() + 100 * ((double)enManager->getHeight() / (double)enManager->getWidth());
	//swappoint_h = main_backgroundRECT->top;
	if (!window_selected)
	{
		if (desktop_inrange)
		{
			list<CWnd*>::iterator itr_desktop_CWnd;
			int p = desktoplist_startindex;
			for (itr_desktop_CWnd = desktop_CWnd_list.begin(); itr_desktop_CWnd != desktop_CWnd_list.end(); itr_desktop_CWnd++)
			{
				if (p == e_global->getSelectedIndex())
					break;

				p = (p + 1) % e_global->desktopList.size();
			}

			std::list<RECT*> window_desktop_rect_list = window_desktop_rect_map.find(e_global->getSelectedIndex())->second;
			std::list<RECT*>::iterator itr_window_desktop_area = window_desktop_rect_list.begin();
			// window 공간들 iterating
			for (std::list<RECT*>::iterator itr_window_area = window_area_list_rect.begin(); itr_window_area != window_area_list_rect.end(); itr_window_area++)
			{
				double areasize = ((*itr_window_area)->right - (*itr_window_area)->left)*((*itr_window_area)->bottom - (*itr_window_area)->top);
				if ((((CRect)(*itr_window_area)).PtInRect(point)) && !(areasize == this->getMainDesktopSize()) && ((CRect)main_backgroundRECT).PtInRect(point))
				{
					hash_map<RECT*, E_Window*>::iterator itr_window_EWindow = window_area_map_RECT_EWindow.find(*itr_window_area);
					hash_map<RECT*, HTHUMBNAIL>::iterator itr_window_hthumbnail = window_RECT_hthumbnail_map.find(*itr_window_area);
					hash_map<RECT*, HTHUMBNAIL> window_desktop_RECT_hthumbnail_map__ = window_desktop_RECT_hthumbnail_map.find(e_global->getSelectedIndex())->second;
					hash_map<RECT*, HTHUMBNAIL>::iterator itr_window_desktop_hthumbnail = window_desktop_RECT_hthumbnail_map__.find(*itr_window_desktop_area);

					window_selected = true;
					window_leftdown_point = point;

					bringWindowToTop(itr_window_EWindow->second->getWindow());
					BringWindowToTop();

					window_ptr = itr_window_EWindow->second;
					window_RECT = *itr_window_area;
					window_RECT_copy = new RECT(*window_RECT);
					window_area_list_rect.remove(*itr_window_area);
					window_area_list_rect.push_front(window_RECT_copy);
					window_area_map_RECT_EWindow.erase(itr_window_EWindow);
					window_area_map_RECT_EWindow.insert(hash_map<RECT*, E_Window*>::value_type(window_RECT_copy, window_ptr));

					aeController->unregisterAero(itr_window_hthumbnail->second);
					window_RECT_hthumbnail_map.erase(itr_window_hthumbnail);
					//aeController->registerAero(window_ptr->getWindow(), this->m_hWnd, *window_RECT_copy, window_hthumbnail);
					aeController->registerAero(window_ptr->getWindow(), mainCWnd->m_hWnd, 
						CRect(window_RECT_copy->left - main_backgroundRECT->left, window_RECT_copy->top - main_backgroundRECT->top, window_RECT_copy->right - (window_RECT_copy->left - main_backgroundRECT->left), window_RECT_copy->bottom - (window_RECT_copy->top - main_backgroundRECT->top)),
						window_hthumbnail);
					window_RECT_hthumbnail_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(window_RECT_copy, window_hthumbnail));

					window_RECT_from_desktop = *itr_window_desktop_area;
					window_desktop_rect_list.remove(*itr_window_desktop_area);
					window_desktop_rect_list.push_front(window_RECT_from_desktop);
					window_desktop_rect_map.erase(window_desktop_rect_map.find(e_global->getSelectedIndex()));
					window_desktop_rect_map.insert(hash_map<int, list<RECT*>>::value_type(e_global->getSelectedIndex(), window_desktop_rect_list));

					aeController->unregisterAero(itr_window_desktop_hthumbnail->second);
					window_desktop_RECT_hthumbnail_map__.erase(itr_window_desktop_hthumbnail);
					aeController->registerAero(window_ptr->getWindow(), (*itr_desktop_CWnd)->m_hWnd, *window_RECT_from_desktop, window_hthumbnail_from_desktop);
					//aeController->registerAero(window_ptr->getWindow(), mainCWnd->m_hWnd, CRect(window_RECT_from_desktop->left - main_backgroundRECT->left, window_RECT_from_desktop->top - main_backgroundRECT->top, window_RECT_from_desktop->right - (window_RECT_from_desktop->left - main_backgroundRECT->left), window_RECT_from_desktop->bottom - (window_RECT_from_desktop->top - main_backgroundRECT->top)), window_hthumbnail_from_desktop);
					window_desktop_RECT_hthumbnail_map__.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(window_RECT_from_desktop, window_hthumbnail_from_desktop));
					window_desktop_RECT_hthumbnail_map.erase(window_desktop_RECT_hthumbnail_map.find(e_global->getSelectedIndex()));
					window_desktop_RECT_hthumbnail_map.insert(hash_map<int, hash_map<RECT*, HTHUMBNAIL>>::value_type(e_global->getSelectedIndex(), window_desktop_RECT_hthumbnail_map__));

					stealFocus(this->m_hWnd);

					restore = true;
					return;
				}
				itr_window_desktop_area++;
			}
		}
		else
		{
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

					bringWindowToTop(itr_window_EWindow->second->getWindow());
					BringWindowToTop();

					window_ptr = itr_window_EWindow->second;
					window_RECT = *itr_window_area;
					window_RECT_copy = new RECT(*window_RECT);
					window_area_list_rect.remove(*itr_window_area);
					window_area_list_rect.push_front(window_RECT_copy);
					window_area_map_RECT_EWindow.erase(itr_window_EWindow);
					window_area_map_RECT_EWindow.insert(hash_map<RECT*, E_Window*>::value_type(window_RECT_copy, window_ptr));

					aeController->unregisterAero(itr_window_hthumbnail->second);
					window_RECT_hthumbnail_map.erase(itr_window_hthumbnail);
					aeController->registerAero(window_ptr->getWindow(), mainCWnd->m_hWnd, 
						CRect(window_RECT_copy->left - main_backgroundRECT->left, window_RECT_copy->top - main_backgroundRECT->top, window_RECT_copy->right - (window_RECT_copy->left - main_backgroundRECT->left), window_RECT_copy->bottom - (window_RECT_copy->top - main_backgroundRECT->top)), 
						window_hthumbnail);
					//aeController->registerAero(window_ptr->getWindow(), mainCWnd->m_hWnd, CRect(window_RECT_copy->left - main_backgroundRECT->left, window_RECT_copy->top - main_backgroundRECT->top, window_RECT_copy->right - (window_RECT_copy->left - main_backgroundRECT->left), window_RECT_copy->bottom - (window_RECT_copy->top - main_backgroundRECT->top)),window_hthumbnail); ///
					window_RECT_hthumbnail_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(window_RECT_copy, window_hthumbnail));

					stealFocus(this->m_hWnd);

					restore = true;
					return;
				}
			}
		}
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}


void E_DesktopSwitcher::OnLButtonUp(UINT nFlags, CPoint point)
{
	E_Global* e_global = E_Global::getSingleton();

	GetLocalTime(&__doubleClickTimer);
	WORD ms = __doubleClickTimer.wMilliseconds - doubleClickTimer.wMilliseconds;
	WORD s = __doubleClickTimer.wSecond - doubleClickTimer.wSecond;
	if (s*(WORD)1000 + __doubleClickTimer.wMilliseconds - doubleClickTimer.wMilliseconds <= (WORD)500 )
	{
		if (!doubleclick_first)
		{
			doubleclick_first = true;
		}
		else if (doubleclick_second)
		{
			int i = 0;
			list<E_Desktop*>::iterator itr_desktop = e_global->desktopList.begin();
			for (; i != desktoplist_startindex; i++)
			{
				if (i == desktoplist_startindex)
					break;
				itr_desktop++;
			}

			bool desktop_dbclicked = false;
			for (std::list<RECT*>::iterator itr_desktop_area = desktop_area_list_rect.begin(); itr_desktop_area != desktop_area_list_rect.end(); itr_desktop_area++)
			{
				if (((CRect)(*itr_desktop_area)).PtInRect(point))
				{
					desktop_dbclicked = true;
				}
				
				i = (i + 1) % e_global->desktopList.size();
				if (i == 0)
				{
					itr_desktop = e_global->desktopList.begin();
				}
				else
				{
					itr_desktop++;
				}
			}
			if (desktop_dbclicked)
			{

				terminateSwitcher();
				//e_global->startUpdate();
				return;
			}

			doubleclick_first = false;
			doubleclick_second = false;
		}
		else
		{
			doubleclick_first = false;
			doubleclick_second = false;
		}
	}
	else
	{
		doubleclick_first = false;
		doubleclick_second = false;
	}

	if (leftarrow->PtInRect(point) && leftarrow_pressed)
	{
		restore = false;
		leftarrow_pressed = false;
		eraseDesktopList();
		desktoplist_startindex = (e_global->desktopList.size() + desktoplist_startindex - 1) % e_global->desktopList.size();
		drawDesktopList();
		desktop_inrange = false;
		for (int j = 0; j < 4; j++)
		{
			if (((desktoplist_startindex + j) % e_global->desktopList.size()) == e_global->getSelectedIndex())
			{
				desktop_inrange = true;
				break;
			}
		}

		Invalidate(TRUE);
		//e_global->startUpdate();
		restore = true;
		return;
	}
	if (rightarrow->PtInRect(point) && rightarrow_pressed)
	{
		restore = false;
		rightarrow_pressed = false;
		eraseDesktopList();
		desktoplist_startindex = (desktoplist_startindex + 1) % e_global->desktopList.size();
		drawDesktopList();
		desktop_inrange = false;
		for (int j = 0; j < 4; j++)
		{
			if (((desktoplist_startindex + j) % e_global->desktopList.size()) == e_global->getSelectedIndex())
			{
				desktop_inrange = true;
				break;
			}
		}

		Invalidate(TRUE);
		//e_global->startUpdate();
		restore = true;
		return;
	}


	if (desktop_selected)
	{
		desktop_selected = false;
		int i = desktoplist_startindex;
		for (std::list<RECT*>::iterator itr_desktop_area = desktop_area_list_rect.begin(); itr_desktop_area != desktop_area_list_rect.end(); itr_desktop_area++)
		{
			if (((CRect)(*itr_desktop_area)).PtInRect(point) && (i == target_desktop_index))
			{
				restore = false;
				eraseWindowS();
				e_global->getSelectedDesktop()->setAllIconInvisible();
				e_global->setSelectedIndex(i);
				drawWindowS();
				desktop_inrange = true;

				Invalidate(TRUE);
				restore = true;
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

		if (desktop_inrange)
		{
			hash_map<RECT*, E_Window*>::iterator itr_window_EWindow = window_area_map_RECT_EWindow.find(window_RECT_copy);
			hash_map<RECT*, HTHUMBNAIL>::iterator itr_window_hthumbnail = window_RECT_hthumbnail_map.find(window_RECT_copy);
			hash_map<RECT*, HTHUMBNAIL> window_desktop_RECT_hthumbnail_map__ = window_desktop_RECT_hthumbnail_map.find(e_global->getSelectedIndex())->second;
			list<RECT*> window_desktop_rect_list = window_desktop_rect_map.find(e_global->getSelectedIndex())->second;
			hash_map<RECT*, HTHUMBNAIL>::iterator itr_window_desktop_hthumbnail = window_desktop_RECT_hthumbnail_map__.find(*window_desktop_rect_list.begin());

			std::list<E_Desktop*>::iterator itr_desktop_from = e_global->desktopList.begin();
			int j = 0;
			for (; itr_desktop_from != e_global->desktopList.end(); itr_desktop_from++)
			{
				if (j == e_global->getSelectedIndex())
					break;
				j++;
			}

			int i = desktoplist_startindex;

			if (window_squeezed_inlist)
			{
				int i_CWnd = 1;
				list<CWnd*>::iterator itr_CWnd = desktop_CWnd_list.begin();

				for (std::list<RECT*>::iterator itr_desktop_rect = desktop_area_list_rect.begin(); itr_desktop_rect != desktop_area_list_rect.end(); itr_desktop_rect++)
				{
					BOOL yesOverlap = ((CRect)*itr_desktop_rect).PtInRect(point);
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

							aeController->unregisterAero(itr_window_desktop_hthumbnail->second);
							window_RECT_hthumbnail_map.erase(itr_window_hthumbnail);

							double ratio_wW = (((double)enManager->getWidth() - 100) * (double)0.175) / main_desktop_width,
								ratio_hH = (((double)enManager->getWidth() - 100) * (double)0.175 * (double)enManager->getHeight() / (double)enManager->getWidth()) / main_desktop_height;
							(*window_RECT_from_desktop).left = ((*window_RECT).left - main_backgroundRECT->left) * ratio_wW;
							(*window_RECT_from_desktop).right = ((*window_RECT).right - main_backgroundRECT->left) * ratio_wW;
							(*window_RECT_from_desktop).top = ((*window_RECT).top - main_backgroundRECT->top) * ratio_hH;
							(*window_RECT_from_desktop).bottom = ((*window_RECT).bottom - main_backgroundRECT->top) * ratio_hH;

							winThumbProps.rcDestination = *window_RECT_from_desktop;
							DwmUpdateThumbnailProperties(window_hthumbnail, &winThumbProps);

							hash_map<RECT*, HTHUMBNAIL> window_desktop_RECT_hthumbnail_map_ = window_desktop_RECT_hthumbnail_map.find(i)->second;
							window_desktop_RECT_hthumbnail_map_.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(window_RECT_from_desktop, window_hthumbnail));
							window_desktop_RECT_hthumbnail_map.erase(window_desktop_RECT_hthumbnail_map.find(i));
							window_desktop_RECT_hthumbnail_map.insert(hash_map<int, hash_map<RECT*, HTHUMBNAIL>>::value_type(i, window_desktop_RECT_hthumbnail_map_));
							window_desktop_RECT_hthumbnail_map__.erase(itr_window_desktop_hthumbnail);
							window_desktop_RECT_hthumbnail_map.erase(window_desktop_RECT_hthumbnail_map.find(e_global->getSelectedIndex()));
							window_desktop_RECT_hthumbnail_map.insert(hash_map<int, hash_map<RECT*, HTHUMBNAIL>>::value_type(e_global->getSelectedIndex(), window_desktop_RECT_hthumbnail_map__));

							window_area_map_RECT_EWindow.erase(itr_window_EWindow);

							window_area_list_rect.remove(*window_area_list_rect.begin());

							list<RECT*> window_desktop_rect_list__ = window_desktop_rect_map.find(i)->second;
							window_desktop_rect_list__.push_front(*window_desktop_rect_list.begin());
							window_desktop_rect_map.erase(window_desktop_rect_map.find(i));
							window_desktop_rect_map.insert(hash_map<int, list<RECT*>>::value_type(i, window_desktop_rect_list__));
							window_desktop_rect_list.remove(*window_desktop_rect_list.begin());
							window_desktop_rect_map.erase(window_desktop_rect_map.find(e_global->getSelectedIndex()));
							window_desktop_rect_map.insert(hash_map<int, list<RECT*>>::value_type(e_global->getSelectedIndex(), window_desktop_rect_list));
						}
						break;
					}
					i = (i + 1) % e_global->desktopList.size();
					itr_CWnd++;
					i_CWnd++;
				}
				window_squeezed_inlist = false;
			}
			else
			{
				aeController->unregisterAero(window_hthumbnail);
				aeController->registerAero(window_ptr->getWindow(), mainCWnd->m_hWnd, CRect(window_RECT->left - main_backgroundRECT->left, window_RECT->top - main_backgroundRECT->top, window_RECT->right - main_backgroundRECT->left, window_RECT->bottom - main_backgroundRECT->top), window_hthumbnail);

				window_area_list_rect.remove(*window_area_list_rect.begin());
				window_area_list_rect.push_front(window_RECT);
				E_Window* RECT_EWindow_copy = itr_window_EWindow->second;
				HTHUMBNAIL RECT_HTHUMBNAIL_copy = itr_window_hthumbnail->second;
				window_area_map_RECT_EWindow.erase(itr_window_EWindow);
				window_area_map_RECT_EWindow.insert(hash_map<RECT*, E_Window*>::value_type(window_RECT, window_ptr));
				//window_area_map_RECT_EWindow.insert(hash_map<RECT*, E_Window*>::value_type(window_RECT, RECT_EWindow_copy));
				window_RECT_hthumbnail_map.erase(itr_window_hthumbnail);
				window_RECT_hthumbnail_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(window_RECT, window_hthumbnail));
				//window_RECT_hthumbnail_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(window_RECT, RECT_HTHUMBNAIL_copy));
			}
		}
		else
		{
			hash_map<RECT*, E_Window*>::iterator itr_window_EWindow = window_area_map_RECT_EWindow.find(window_RECT_copy);
			hash_map<RECT*, HTHUMBNAIL>::iterator itr_window_hthumbnail = window_RECT_hthumbnail_map.find(window_RECT_copy);

			std::list<E_Desktop*>::iterator itr_desktop_from = e_global->desktopList.begin();
			int j = 0;
			for (; itr_desktop_from != e_global->desktopList.end(); itr_desktop_from++)
			{
				if (j == e_global->getSelectedIndex())
					break;
				j++;
			}

			if (window_squeezed_inlist)
			{
				int i = desktoplist_startindex;
				int p = 0;
				for (std::list<RECT*>::iterator itr_desktop_rect = desktop_area_list_rect.begin(); itr_desktop_rect != desktop_area_list_rect.end(); itr_desktop_rect++)
				{
					BOOL yesOverlap = ((CRect)*itr_desktop_rect).PtInRect(point);
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

							window_RECT_hthumbnail_map.erase(itr_window_hthumbnail);

							
							double ratio_wW = (((double)enManager->getWidth() - 100) * (double)0.175) / main_desktop_width,
								ratio_hH = (((double)enManager->getWidth() - 100) * (double)0.175 * (double)enManager->getHeight() / (double)enManager->getWidth()) / main_desktop_height;
							(*window_RECT_from_desktop).left = ((*window_RECT).left - main_backgroundRECT->left) * ratio_wW;
							(*window_RECT_from_desktop).right = ((*window_RECT).right - main_backgroundRECT->left) * ratio_wW;
							(*window_RECT_from_desktop).top = ((*window_RECT).top - main_backgroundRECT->top) * ratio_hH;
							(*window_RECT_from_desktop).bottom = ((*window_RECT).bottom - main_backgroundRECT->top) * ratio_hH;

							winThumbProps.rcDestination = *window_RECT_from_desktop;
							DwmUpdateThumbnailProperties(window_hthumbnail, &winThumbProps); 
							
							hash_map<RECT*, HTHUMBNAIL> window_desktop_RECT_hthumbnail_map_ = window_desktop_RECT_hthumbnail_map.find(i)->second;
							window_desktop_RECT_hthumbnail_map_.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(window_RECT_from_desktop, window_hthumbnail));
							window_desktop_RECT_hthumbnail_map.erase(window_desktop_RECT_hthumbnail_map.find(i));
							window_desktop_RECT_hthumbnail_map.insert(hash_map<int, hash_map<RECT*, HTHUMBNAIL>>::value_type(i, window_desktop_RECT_hthumbnail_map_));

							window_area_map_RECT_EWindow.erase(itr_window_EWindow);

							window_area_list_rect.remove(*window_area_list_rect.begin());

							list<RECT*> window_desktop_rect_list__ = window_desktop_rect_map.find(i)->second;
							window_desktop_rect_list__.push_front(window_RECT_from_desktop);
							window_desktop_rect_map.erase(window_desktop_rect_map.find(i));
							window_desktop_rect_map.insert(hash_map<int, list<RECT*>>::value_type(i, window_desktop_rect_list__));
						}
						break;
					}
					i = (i + 1) % e_global->desktopList.size();
					p++;
				}
				window_squeezed_inlist = false;
			}
			else
			{
				aeController->unregisterAero(window_hthumbnail);
				aeController->registerAero(window_ptr->getWindow(), mainCWnd->m_hWnd, CRect(window_RECT->left - main_backgroundRECT->left, window_RECT->top - main_backgroundRECT->top, window_RECT->right - main_backgroundRECT->left, window_RECT->bottom - main_backgroundRECT->top), window_hthumbnail);
				
				window_area_list_rect.remove(*window_area_list_rect.begin());
				window_area_list_rect.push_front(window_RECT);
				E_Window* RECT_EWindow_copy = itr_window_EWindow->second;
				HTHUMBNAIL RECT_HTHUMBNAIL_copy = itr_window_hthumbnail->second;
				window_area_map_RECT_EWindow.erase(itr_window_EWindow);
				window_area_map_RECT_EWindow.insert(hash_map<RECT*, E_Window*>::value_type(window_RECT, window_ptr));
				//window_area_map_RECT_EWindow.insert(hash_map<RECT*, E_Window*>::value_type(window_RECT, RECT_EWindow_copy));
				window_RECT_hthumbnail_map.erase(itr_window_hthumbnail);
				window_RECT_hthumbnail_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(window_RECT, window_hthumbnail));
				//window_RECT_hthumbnail_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(window_RECT, RECT_HTHUMBNAIL_copy));
			}
		}
	}

	E_Window::setIconInvisible(this->m_hWnd);
	CWnd::OnLButtonUp(nFlags, point);
}


void E_DesktopSwitcher::OnMouseMove(UINT nFlags, CPoint point)
{
	if (((GetKeyState(VK_LBUTTON) & 0x80) != 0) && window_selected)
	{
		//SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
		E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();
		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();

		LONG diff_x = point.x - window_leftdown_point.x,
			diff_y = point.y - window_leftdown_point.y;

		(*window_RECT_copy).left = (*window_RECT_copy).left + diff_x;
		(*window_RECT_copy).right = (*window_RECT_copy).right + diff_x;
		(*window_RECT_copy).top = (*window_RECT_copy).top + diff_y;
		(*window_RECT_copy).bottom = (*window_RECT_copy).bottom + diff_y;
		if (window_squeezed)
		{
			if (((CRect)main_backgroundRECT).PtInRect(point))
			{ // TODO
				double ratio_wW = (((double)enManager->getWidth() - 100) * (double)0.175) / main_desktop_width,
					ratio_hH = (((double)enManager->getWidth() - 100) * (double)0.175 * (double)enManager->getHeight() / (double)enManager->getWidth()) / main_desktop_height;

				(*window_RECT_copy).top = point.y - (point.y - (*window_RECT_copy).top) / ratio_hH;
				(*window_RECT_copy).bottom = point.y + ((*window_RECT_copy).bottom - point.y) / ratio_hH;
				(*window_RECT_copy).left = point.x - (point.x - (*window_RECT_copy).left) / ratio_wW;
				(*window_RECT_copy).right = point.x + ((*window_RECT_copy).right - point.x) / ratio_wW;
				window_squeezed = false;

				aeController->unregisterAero(window_hthumbnail);
				aeController->registerAero(window_ptr->getWindow(), mainCWnd->m_hWnd, CRect(window_RECT_copy->left - main_backgroundRECT->left, window_RECT_copy->top - main_backgroundRECT->top, window_RECT_copy->right - main_backgroundRECT->left, window_RECT_copy->bottom - main_backgroundRECT->top), window_hthumbnail);
			}
			else
			{
				if (!window_squeezed_inlist)
				{
					int i = desktoplist_startindex;
					list<CWnd*>::iterator itr_CWnd = desktop_CWnd_list.begin();
					for (list<RECT*>::iterator itr_rect = desktop_area_list_rect.begin(); itr_rect != desktop_area_list_rect.end(); itr_rect++)
					{
						if ((((CRect)(*itr_rect)).PtInRect(point)) && (i != E_Global::getSingleton()->getSelectedIndex()))
						{
							(*window_RECT_copy).top -= (*itr_rect)->top;
							(*window_RECT_copy).bottom -= (*itr_rect)->top;
							(*window_RECT_copy).left -= (*itr_rect)->left;
							(*window_RECT_copy).right -= (*itr_rect)->left;
							window_squeezed_top = (*itr_rect)->top;
							window_squeezed_left = (*itr_rect)->left;
							aeController->unregisterAero(window_hthumbnail);
							aeController->registerAero(window_ptr->getWindow(), (*itr_CWnd)->m_hWnd, *window_RECT_copy, window_hthumbnail);

							window_squeezed_inlist = true;
							break;
						}
						i = (i + 1) % E_Global::getSingleton()->desktopList.size();
						itr_CWnd++;
					}

					if (!window_squeezed_inlist)
					{
						winThumbProps.rcDestination = *window_RECT_copy;
						DwmUpdateThumbnailProperties(window_hthumbnail, &winThumbProps);
					}
				}
				else
				{
					window_squeezed_inlist = false;
					for (list<RECT*>::iterator itr_rect = desktop_area_list_rect.begin(); itr_rect != desktop_area_list_rect.end(); itr_rect++)
					{
						if (((CRect)(*itr_rect)).PtInRect(point))
						{
							window_squeezed_inlist = true;
							break;
						}
					}

					if (window_squeezed_inlist)
					{
						winThumbProps.rcDestination = *window_RECT_copy;
						DwmUpdateThumbnailProperties(window_hthumbnail, &winThumbProps);
						window_squeezed_inlist = true;
					}
					else
					{
						(*window_RECT_copy).top += window_squeezed_top;
						(*window_RECT_copy).bottom += window_squeezed_top;
						(*window_RECT_copy).left += window_squeezed_left;
						(*window_RECT_copy).right += window_squeezed_left;

						aeController->unregisterAero(window_hthumbnail);
						aeController->registerAero(window_ptr->getWindow(), this->m_hWnd, *window_RECT_copy, window_hthumbnail);
						window_squeezed_inlist = false;
					}
				}
			}
		}
		else
		{
			//if ((double)(*window_RECT_copy).top < swappoint_h - 50)
			if (!((CRect)main_backgroundRECT).PtInRect(point))
			{
				window_squeezed = true;

				double ratio_wW = (((double)enManager->getWidth() - 100) * (double)0.175) / main_desktop_width,
					ratio_hH = (((double)enManager->getWidth() - 100) * (double)0.175 * (double)enManager->getHeight() / (double)enManager->getWidth()) / main_desktop_height;

				(*window_RECT_copy).top = point.y - (point.y - (*window_RECT_copy).top) * ratio_hH;
				(*window_RECT_copy).bottom = point.y + ((*window_RECT_copy).bottom - point.y) * ratio_hH;
				(*window_RECT_copy).left = point.x - (point.x - (*window_RECT_copy).left) * ratio_wW;
				(*window_RECT_copy).right = point.x + ((*window_RECT_copy).right - point.x) * ratio_wW;

				aeController->unregisterAero(window_hthumbnail);
				aeController->registerAero(window_ptr->getWindow(), this->m_hWnd, *window_RECT_copy, window_hthumbnail);

				//winThumbProps.rcDestination = *window_RECT_copy;
				//DwmUpdateThumbnailProperties(window_hthumbnail, &winThumbProps);
			}
			else
			{
				winThumbProps.rcDestination = CRect(window_RECT_copy->left - main_backgroundRECT->left, window_RECT_copy->top - main_backgroundRECT->top, window_RECT_copy->right - main_backgroundRECT->left, window_RECT_copy->bottom - main_backgroundRECT->top);
				DwmUpdateThumbnailProperties(window_hthumbnail, &winThumbProps);
			}
		}
		window_leftdown_point = point;
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
	
	else
	{
		if (E_Global::getSingleton()->desktopList.size() > 4)
		{
			if ((leftarrow->PtInRect(point)) || (rightarrow->PtInRect(point)))
			{
				SetCursor(LoadCursor(NULL, IDC_HAND));
			}
			else
			{
				SetCursor(LoadCursor(NULL, IDC_ARROW));
			}
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void E_DesktopSwitcher::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 __super::OnPaint()을(를) 호출하지 마십시오.
	if (ison)
	{
		CPen pen;
		pen.CreatePen(PS_SOLID, 4, RGB(255, 255, 255));
		dc.SelectObject(pen);

		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		E_Global* e_global = E_Global::getSingleton();
		int i = desktoplist_startindex;
		for (list<RECT*>::iterator itr_rect = desktop_area_list_rect.begin(); itr_rect != desktop_area_list_rect.end(); itr_rect++)
		{
			if (e_global->getSelectedIndex() == i)
			{
				dc.MoveTo((*itr_rect)->left - (9 * enManager->getWidth() / 1920), (*itr_rect)->top - (6 * enManager->getHeight() / 1080));
				dc.LineTo((*itr_rect)->right + (9 * enManager->getWidth() / 1920), (*itr_rect)->top - (6 * enManager->getHeight() / 1080));
				dc.MoveTo((*itr_rect)->right + (9 * enManager->getWidth() / 1920), (*itr_rect)->top - (6 * enManager->getHeight() / 1080));
				dc.LineTo((*itr_rect)->right + (9 * enManager->getWidth() / 1920), (*itr_rect)->bottom + (6 * enManager->getHeight() / 1080));
				dc.MoveTo((*itr_rect)->left - (9 * enManager->getWidth() / 1920), (*itr_rect)->top - (6 * enManager->getHeight() / 1080));
				dc.LineTo((*itr_rect)->left - (9 * enManager->getWidth() / 1920), (*itr_rect)->bottom + (6 * enManager->getHeight() / 1080));
				dc.MoveTo((*itr_rect)->left - (9 * enManager->getWidth() / 1920), (*itr_rect)->bottom + (6 * enManager->getHeight() / 1080));
				dc.LineTo((*itr_rect)->right + (9 * enManager->getWidth() / 1920), (*itr_rect)->bottom + (6 * enManager->getHeight() / 1080));
				pen.DeleteObject();
				return;
			}
			i = (i + 1) % e_global->getDesktopCount();
		}

		
	}
}


void E_DesktopSwitcher::OnDestroy()
{
	__super::OnDestroy();

	terminateSwitcher();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


int E_DesktopSwitcher::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}

void E_DesktopSwitcher::keyArrowPress(int direction)
{
	if (ison)
	{
		E_Global* e_global = E_Global::getSingleton();

		int index = (e_global->desktopList.size() + e_global->getSelectedIndex() + direction) % e_global->desktopList.size();

		if (e_global->desktopList.size() == 1)
			return;

		else if ((e_global->desktopList.size() >= 2) && (e_global->desktopList.size() <= 4))
		{			
			eraseWindowS();
			e_global->getSelectedDesktop()->setAllIconInvisible();
			e_global->setSelectedIndex(index);
			drawWindowS();
			desktop_inrange = true;

			Invalidate(TRUE);
			return;
		}
		else
		{	
			int i = desktoplist_startindex, p = 0;
			desktop_inrange = false;
			while (p < 4)
			{
				if (e_global->getSelectedIndex() == i)
				{
					desktop_inrange = true;
					break;
				}
				i = (i + 1) % e_global->desktopList.size();
				p++;
			}
			restore = false;
			if (desktop_inrange)
			{
				if ((p == 0) && (direction == -1))
				{
					eraseDesktopList();
					desktoplist_startindex = (e_global->desktopList.size() + desktoplist_startindex - 1) % e_global->desktopList.size();
					drawDesktopList();

					eraseWindowS();
					e_global->getSelectedDesktop()->setAllIconInvisible();
					e_global->setSelectedIndex(index);
					drawWindowS();
					desktop_inrange = true;

					Invalidate(TRUE);
				}
				else if ((p == 3) && (direction == 1))
				{
					eraseDesktopList();
					desktoplist_startindex = (e_global->desktopList.size() + desktoplist_startindex + 1) % e_global->desktopList.size();
					drawDesktopList();

					eraseWindowS();
					e_global->getSelectedDesktop()->setAllIconInvisible();
					e_global->setSelectedIndex(index);
					drawWindowS();
					desktop_inrange = true;

					Invalidate(TRUE);
				}
				else
				{
					eraseWindowS();
					e_global->getSelectedDesktop()->setAllIconInvisible();
					e_global->setSelectedIndex(index);
					drawWindowS();
					desktop_inrange = true;

					Invalidate(TRUE);
				}
			}
			else
			{
				eraseWindowS();
				e_global->getSelectedDesktop()->setAllIconInvisible();
				e_global->setSelectedIndex(index);
				drawWindowS();
				desktop_inrange = true;

				Invalidate(TRUE);
			}
			restore = true;
		}
	}
}

void E_DesktopSwitcher::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_LEFT)
	{
		leftkey_pressed = true;
	}
	else if (nChar == VK_RIGHT)
	{
		rightkey_pressed = true;
	}
	else if (nChar == VK_RETURN)
	{
		enterkey_pressed = true;
	}
	else if (nChar == VK_ESCAPE)
	{
		esckey_pressed = true;
	}

	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}


void E_DesktopSwitcher::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	otherkey = false;
	if ((nChar == VK_LEFT) && leftkey_pressed)
	{
		if (!((0 == E_Global::getSingleton()->getSelectedIndex()) && (E_Global::getSingleton()->desktopList.size() <= 4)))
		{	
			keyArrowPress(-1);
			stealFocus(this->m_hWnd);
			E_Window::setIconInvisible(this->m_hWnd);
		}
	}
	else if ((nChar == VK_RIGHT) && rightkey_pressed)
	{
		if (!((E_Global::getSingleton()->getSelectedIndex() == (E_Global::getSingleton()->desktopList.size() - 1)) && (E_Global::getSingleton()->desktopList.size() <= 4)))
		{
			keyArrowPress(1);
			stealFocus(this->m_hWnd);
			E_Window::setIconInvisible(this->m_hWnd);
		}
	}
	else if ((nChar == VK_RETURN) && enterkey_pressed)
	{
		restore = false;
		terminateSwitcher();
	}
	else if ((nChar == VK_ESCAPE) && esckey_pressed)
	{
		if (initindex != E_Global::getSingleton()->getSelectedIndex())
		{
			E_Global::getSingleton()->getSelectedDesktop()->setAllIconInvisible();
			E_Global::getSingleton()->setSelectedIndex(initindex);
		}
		restore = false;
		terminateSwitcher();
	}
	else
	{
		otherkey = true;
	}

	leftkey_pressed = false;
	rightkey_pressed = false;
	enterkey_pressed = false;
	esckey_pressed = false;

	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}


void E_DesktopSwitcher::OnKillFocus(CWnd* pNewWnd)
{
	if (otherkey)
	{
		otherkey = false;
		return;
	}
	if (restore)
	{
		terminateSwitcher();
	}

	__super::OnKillFocus(pNewWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
