#include "stdafx.h"
#include "E_DragAndDropSwitcher.h"
#include <Windows.h> 
#include <WinUser.h>
#include "E_Global.h"
#include "E_EnvironmentManager.h"
#include "E_AeroPeekController.h"
#include "E_DesktopSwitcher.h"
#define WM_TRAY_EVENT (WM_USER + 3)

const COLORREF E_DragAndDropSwitcher::backgroundColor = RGB(0x37, 0xb6, 0xeb);

void E_DragAndDropSwitcher::updateSelectedDesktop()
{
	//업데이트가 발생한 경우 자동으로 호출됨
	TRACE_WIN32A("[E_DragAndDropSwitcher::updateSelectedDesktop()]");
}

void drawCurrentDesktop()
{
	E_Global* e_global = E_Global::getSingleton();
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_DragAndDropSwitcher* drSwitcher = E_DragAndDropSwitcher::getSingleton();
	E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();


	HTHUMBNAIL pushThumbnail;

	LONG plus_width = 0, plus_height = 0;
	if (drSwitcher->cursor_left)
	{
		plus_width = drSwitcher->sizeRect_background.right;
	}
	else if (drSwitcher->cursor_top)
	{
		plus_height = drSwitcher->sizeRect_background.bottom;
	}

	drSwitcher->currentDesktopRECT = new RECT
	{
		drSwitcher->sizeRect_background.left + plus_width,
		drSwitcher->sizeRect_background.top + plus_height,
		drSwitcher->sizeRect_background.right + plus_width,
		drSwitcher->sizeRect_background.bottom + plus_height
	};
	aeController->registerAero(drSwitcher->hShellWnd, drSwitcher->m_hWnd, *(drSwitcher->currentDesktopRECT), drSwitcher->currentDesktopThumbnail);


	for (list<HWND>::iterator itr_window_docked = e_global->dockedWindowList.begin(); itr_window_docked != e_global->dockedWindowList.end(); itr_window_docked++)
	{
		UINT state;
		WINDOWPLACEMENT windowinfo;
		GetWindowPlacement(*itr_window_docked, &windowinfo);
		UINT winstate = windowinfo.showCmd;
		BOOL isVisible = IsWindowVisible(*itr_window_docked);
		BOOL isMinimized = IsIconic(*itr_window_docked);

		if (winstate == SW_HIDE)
			::ShowWindow(*itr_window_docked, SW_SHOW);

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
			RECT *windowRECT = new RECT
			{
				getSize.left + plus_width,
				getSize.top + plus_height,
				getSize.right + plus_width,
				getSize.bottom + plus_height
			};
			aeController->registerAero(*itr_window_docked, drSwitcher->m_hWnd, *windowRECT, pushThumbnail);
		}
	}

	std::list<E_Window*> window_list = e_global->getSelectedDesktop()->windowList;
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
				CRect getSize;
				GetWindowRect((*itr_window)->getWindow(), &getSize);

				RECT *windowRECT = new RECT
				{
					getSize.left + plus_width,
					getSize.top + plus_height,
					getSize.right + plus_width,
					getSize.bottom + plus_height
				};
				aeController->registerAero((*itr_window)->getWindow(), drSwitcher->m_hWnd, *windowRECT, pushThumbnail);
				//drSwitcher->current_RECT_HTHUMBNAIL_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(windowRECT, pushThumbnail));
			}
		}
	}

	drSwitcher->currentTaskbarRECT = new RECT
	{
		drSwitcher->sizeRect_taskbar.left + plus_width,
		drSwitcher->sizeRect_taskbar.top + plus_height,
		drSwitcher->sizeRect_taskbar.right + plus_width,
		drSwitcher->sizeRect_taskbar.bottom + plus_height
	};
	aeController->registerAero(drSwitcher->hTaskbarWnd, drSwitcher->m_hWnd, *(drSwitcher->currentTaskbarRECT), drSwitcher->currentTaskbarThumbnail);

	for (std::list<E_Window*>::iterator itr_window = window_list_topmost.begin(); itr_window != window_list_topmost.end(); itr_window++)
	{
		CRect getSize;
		GetWindowRect((*itr_window)->getWindow(), &getSize);

		RECT *windowRECT = new RECT
		{
			getSize.left + plus_width,
			getSize.top + plus_height,
			getSize.right + plus_width,
			getSize.bottom + plus_height
		};
		aeController->registerAero((*itr_window)->getWindow(), drSwitcher->m_hWnd, *windowRECT, pushThumbnail);
		//drSwitcher->current_RECT_HTHUMBNAIL_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(windowRECT, pushThumbnail));
	}
}

void drawSwitchDesktop()
{
	E_Global* e_global = E_Global::getSingleton();
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_DragAndDropSwitcher* drSwitcher = E_DragAndDropSwitcher::getSingleton();
	E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();

	HTHUMBNAIL pushThumbnail;

	drSwitcher->switchDesktop;

	LONG width = 0, height = 0;

	if (drSwitcher->cursor_left)
	{
		width = 0;

		drSwitcher->switchDesktopRECT = new RECT
		{
			0,
			drSwitcher->sizeRect_background.top,
			drSwitcher->sizeRect_background.right,
			drSwitcher->sizeRect_background.bottom
		};
		aeController->registerAero(drSwitcher->hShellWnd, drSwitcher->m_hWnd, *(drSwitcher->switchDesktopRECT), drSwitcher->switchDesktopThumbnail);
		
		drSwitcher->switchTaskbarRECT = new RECT
		{
			0,
			drSwitcher->sizeRect_taskbar.top,
			drSwitcher->sizeRect_taskbar.right,
			drSwitcher->sizeRect_taskbar.bottom
		};
		}
	else if (drSwitcher->cursor_right)
	{
		width = drSwitcher->sizeRect_background.right; 

		drSwitcher->switchDesktopRECT = new RECT
		{
			drSwitcher->sizeRect_background.right + drSwitcher->sizeRect_background.left,
			drSwitcher->sizeRect_background.top,
			drSwitcher->sizeRect_background.right + drSwitcher->sizeRect_background.right,
			drSwitcher->sizeRect_background.bottom
		};
		aeController->registerAero(drSwitcher->hShellWnd, drSwitcher->m_hWnd, *(drSwitcher->switchDesktopRECT), drSwitcher->switchDesktopThumbnail);
		
		drSwitcher->switchTaskbarRECT = new RECT
		{
			drSwitcher->sizeRect_taskbar.right + drSwitcher->sizeRect_taskbar.left,
			drSwitcher->sizeRect_taskbar.top,
			drSwitcher->sizeRect_taskbar.right + drSwitcher->sizeRect_taskbar.right,
			drSwitcher->sizeRect_taskbar.bottom
		};
		}
	else if (drSwitcher->cursor_top)
	{
		height = 0;

		drSwitcher->switchDesktopRECT = new RECT
		{
			drSwitcher->sizeRect_background.left,
			0,
			drSwitcher->sizeRect_background.right,
			drSwitcher->sizeRect_background.bottom
		};
		aeController->registerAero(drSwitcher->hShellWnd, drSwitcher->m_hWnd, *(drSwitcher->switchDesktopRECT), drSwitcher->switchDesktopThumbnail);
		
		drSwitcher->switchTaskbarRECT = new RECT
		{
			drSwitcher->sizeRect_taskbar.left,
			drSwitcher->sizeRect_taskbar.top,
			drSwitcher->sizeRect_taskbar.right,
			drSwitcher->sizeRect_taskbar.bottom
		};
			}
		else
		{
		height = drSwitcher->sizeRect_background.bottom;

		drSwitcher->switchDesktopRECT = new RECT
		{
			drSwitcher->sizeRect_background.left,
			drSwitcher->sizeRect_background.bottom + drSwitcher->sizeRect_background.top,
			drSwitcher->sizeRect_background.right,
			drSwitcher->sizeRect_background.bottom + drSwitcher->sizeRect_background.bottom
		};
		aeController->registerAero(drSwitcher->hShellWnd, drSwitcher->m_hWnd, *(drSwitcher->switchDesktopRECT), drSwitcher->switchDesktopThumbnail);
		
		drSwitcher->switchTaskbarRECT = new RECT
		{
			drSwitcher->sizeRect_taskbar.left,
			drSwitcher->sizeRect_taskbar.bottom + drSwitcher->sizeRect_taskbar.top,
			drSwitcher->sizeRect_taskbar.right,
			drSwitcher->sizeRect_taskbar.bottom + drSwitcher->sizeRect_taskbar.bottom
		};
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
			RECT *windowRECT = new RECT
			{
				getSize.left + width,
				getSize.top + height,
				getSize.right + width,
				getSize.bottom + height
			};
			aeController->registerAero(*itr_window_docked, drSwitcher->m_hWnd, *windowRECT, pushThumbnail);	
		}
	}

	for (list<E_Window*>::iterator itr = drSwitcher->switchDesktop->windowList.begin(); itr != drSwitcher->switchDesktop->windowList.end(); itr++)
	{
		if (!(*itr)->dock)
			(*itr)->setShow();
	}

	//drSwitcher->switchDesktop->setAllShow();
	std::list<E_Window*> window_list = drSwitcher->switchDesktop->getWindowList();
	std::list<E_Window*> window_list_topmost;
	for (std::list<E_Window*>::iterator itr_window = window_list.begin(); itr_window != window_list.end(); itr_window++)
	{
		if (((*itr_window)->isAeroPossible()) && IsWindow((*itr_window)->getWindow()) && (!(*itr_window)->dock))
		{
			//(*itr_window)->setShow();
			DWORD dwExStyle = ::GetWindowLong((*itr_window)->getWindow(), GWL_EXSTYLE);
			if ((dwExStyle & WS_EX_TOPMOST) != 0)
			{
				window_list_topmost.push_front(*itr_window);
			}
			else
			{
				CRect getSize;
				GetWindowRect((*itr_window)->getWindow(), &getSize);

				RECT *windowRECT = new RECT
				{
					getSize.left + width,
					getSize.top + height,
					getSize.right + width,
					getSize.bottom + height
				};
				aeController->registerAero((*itr_window)->getWindow(), drSwitcher->m_hWnd, *windowRECT, pushThumbnail);
				//drSwitcher->switch_RECT_HTHUMBNAIL_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(windowRECT, pushThumbnail));
			}
		}
	}

	aeController->registerAero(drSwitcher->hTaskbarWnd, drSwitcher->m_hWnd, *(drSwitcher->switchTaskbarRECT), drSwitcher->switchTaskbarThumbnail);

	for (std::list<E_Window*>::iterator itr_window = window_list_topmost.begin(); itr_window != window_list_topmost.end(); itr_window++)
	{
		CRect getSize;
		GetWindowRect((*itr_window)->getWindow(), &getSize);

		RECT *windowRECT = new RECT
		{
			getSize.left + width,
			getSize.top + height,
			getSize.right + width,
			getSize.bottom + height
		};
		aeController->registerAero((*itr_window)->getWindow(), drSwitcher->m_hWnd, *windowRECT, pushThumbnail);
		//drSwitcher->switch_RECT_HTHUMBNAIL_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(windowRECT, pushThumbnail));
	}
}

void drawDragAndDropSwitcher()
{
	E_DragAndDropSwitcher* drSwitcher = E_DragAndDropSwitcher::getSingleton();

	drawCurrentDesktop();
	drawSwitchDesktop();

	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(*drSwitcher, &ps);
	EndPaint(*drSwitcher, &ps);
}

void drawDragAndDropSwitcher_noAero()
{
	E_Global* e_global = E_Global::getSingleton();
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_DragAndDropSwitcher* drSwitcher = E_DragAndDropSwitcher::getSingleton();
	E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();

	CPaintDC currentDC((drSwitcher->currentCWnd));
	CPaintDC switchDC((drSwitcher->switchCWnd));
	
	BITMAP bmpinfo_desktop, bmpinfo_taskbar;
	drSwitcher->desktopWindow->getScreenshot()->GetBitmap(&bmpinfo_desktop);
	drSwitcher->taskbarWindow->getScreenshot()->GetBitmap(&bmpinfo_taskbar);


	HGDIOBJ currentDesktopBitmap = drSwitcher->desktopWindow->getScreenshot()->GetSafeHandle(),
		currentTaskbarBitmap = drSwitcher->taskbarWindow->getScreenshot()->GetSafeHandle();

	// 더블 버퍼링을 위한 코드
	//메모리 DC를 생성한다. (버퍼 메모리 할당)
	CDC memDC_current, memDC_switch;
	//그릴 Bitmap을 생성한다. (한번에 그릴 도화지 정도 개념)
	CBitmap bmp_current, bmp_switch;
	//메모리 DC를 위의 CPaintDC인 dc에 호환되게 만들어 준다.
	memDC_current.CreateCompatibleDC(&currentDC);
	memDC_switch.CreateCompatibleDC(&switchDC);
	//주어진 dc에 호환하는 비트맵을 생성한다.
	bmp_current.CreateCompatibleBitmap(&currentDC, enManager->getWidth(), enManager->getHeight());
	bmp_switch.CreateCompatibleBitmap(&switchDC, enManager->getWidth(), enManager->getHeight());
	//이제 memDC에 생성된 비트맵을 연결한다.
	memDC_current.SelectObject(bmp_current);
	memDC_switch.SelectObject(bmp_switch);

	CDC cdc_current, cdc_switch;
	cdc_current.CreateCompatibleDC(&currentDC);
	cdc_current.SelectObject(currentDesktopBitmap);
	cdc_current.SetStretchBltMode(COLORONCOLOR);

	memDC_current.StretchBlt(0, 0,
		drSwitcher->currentCWndRECT->right - drSwitcher->currentCWndRECT->left,
		drSwitcher->currentCWndRECT->bottom - drSwitcher->currentCWndRECT->top,
		&cdc_current, 0, 0, bmpinfo_desktop.bmWidth, bmpinfo_desktop.bmHeight, SRCCOPY);

	for (list<HWND>::iterator itr_window_docked = e_global->dockedWindowList.begin(); itr_window_docked != e_global->dockedWindowList.end(); itr_window_docked++)
	{
		UINT state;
		WINDOWPLACEMENT windowinfo;
		GetWindowPlacement(*itr_window_docked, &windowinfo);
		UINT winstate = windowinfo.showCmd;
		BOOL isVisible = IsWindowVisible(*itr_window_docked);
		BOOL isMinimized = IsIconic(*itr_window_docked);

		if (winstate == SW_HIDE)
			::ShowWindow(*itr_window_docked, SW_SHOW);

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
			RECT windowRECT
		{
				getSize.left,
				getSize.top,
				getSize.right,
				getSize.bottom
		};

			E_Window* getttingWindow = new E_Window(*itr_window_docked);
			HGDIOBJ currentBitmap = getttingWindow->getScreenshot()->GetSafeHandle();

			cdc_current.SelectObject(currentBitmap);
			memDC_current.StretchBlt(windowRECT.left, windowRECT.top,
				windowRECT.right - windowRECT.left, windowRECT.bottom - windowRECT.top,
				&cdc_current, 0, 0, windowRECT.right - windowRECT.left, windowRECT.bottom - windowRECT.top, SRCCOPY);
		}
	}

	std::list<E_Window*> window_list = e_global->getSelectedDesktop()->windowList;
	std::list<E_Window*> window_list_topmost;
	for (std::list<E_Window*>::iterator itr_window = window_list.begin(); itr_window != window_list.end(); itr_window++)
		{
		UINT state;
		WINDOWPLACEMENT windowinfo;
		GetWindowPlacement((*itr_window)->getWindow(), &windowinfo);
		UINT winstate = windowinfo.showCmd;
		BOOL isVisible = IsWindowVisible((*itr_window)->getWindow());
		BOOL isMinimized = IsIconic((*itr_window)->getWindow());

		if (IsWindow((*itr_window)->getWindow()) && (!(*itr_window)->dock)
			&& !((state = winstate) == SW_FORCEMINIMIZE
			|| (state = winstate) == SW_HIDE		//HIDE는 사실 처리 안됨 (invisible)
			|| (state = winstate) == SW_MINIMIZE
			|| (state = winstate) == SW_SHOWMINIMIZED
			|| (state = winstate) == SW_SHOWMINNOACTIVE
			|| isVisible == FALSE
			|| isMinimized == TRUE))
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

				RECT windowRECT
		{
					getSize.left,
					getSize.top,
					getSize.right,
					getSize.bottom
		};

				(*itr_window)->takeScreenshot();
				HGDIOBJ currentBitmap = (*itr_window)->getScreenshot()->GetSafeHandle();

				cdc_current.SelectObject(currentBitmap);
				memDC_current.StretchBlt(windowRECT.left, windowRECT.top,
					windowRECT.right - windowRECT.left, windowRECT.bottom - windowRECT.top,
					&cdc_current, 0, 0, windowRECT.right - windowRECT.left, windowRECT.bottom - windowRECT.top, SRCCOPY);
			}
		}
	}

	cdc_current.SelectObject(currentTaskbarBitmap);
	memDC_current.StretchBlt(0, drSwitcher->currentTaskbarRECT->top,
		drSwitcher->currentTaskbarRECT->right - drSwitcher->currentTaskbarRECT->left,
		drSwitcher->currentTaskbarRECT->bottom - drSwitcher->currentTaskbarRECT->top,
		&cdc_current, 0, 0, bmpinfo_taskbar.bmWidth, bmpinfo_taskbar.bmHeight, SRCCOPY);

	for (std::list<E_Window*>::iterator itr_window = window_list_topmost.begin(); itr_window != window_list_topmost.end(); itr_window++)
	{
		CRect getSize;
		GetWindowRect((*itr_window)->getWindow(), &getSize);

		RECT windowRECT
		{
			getSize.left,
			getSize.top,
			getSize.right,
			getSize.bottom
		};

		(*itr_window)->takeScreenshot();
		HGDIOBJ currentBitmap = (*itr_window)->getScreenshot()->GetSafeHandle();

		cdc_current.SelectObject(currentBitmap);
		memDC_current.StretchBlt(windowRECT.left, windowRECT.top,
			windowRECT.right - windowRECT.left, windowRECT.bottom - windowRECT.top,
			&cdc_current, 0, 0, windowRECT.right - windowRECT.left, windowRECT.bottom - windowRECT.top, SRCCOPY);
	}

	currentDC.StretchBlt(0, 0,
		enManager->getWidth(), enManager->getHeight(),
		&memDC_current, 0, 0, bmpinfo_desktop.bmWidth, bmpinfo_desktop.bmHeight, SRCCOPY);

	cdc_current.DeleteDC();
	memDC_current.DeleteDC();
	bmp_current.DeleteObject();


	//Switch Draw
	
	e_global->getDesktop(drSwitcher->switchIndex)->setAllShow();

	cdc_switch.CreateCompatibleDC(&switchDC);
	cdc_switch.SelectObject(currentDesktopBitmap);
	cdc_switch.SetStretchBltMode(COLORONCOLOR);

	memDC_switch.StretchBlt(0, 0,
		drSwitcher->switchCWndRECT->right - drSwitcher->switchCWndRECT->left,
		drSwitcher->switchCWndRECT->bottom - drSwitcher->switchCWndRECT->top,
		&cdc_switch, 0, 0, bmpinfo_desktop.bmWidth, bmpinfo_desktop.bmHeight, SRCCOPY);

	for (list<HWND>::iterator itr_window_docked = e_global->dockedWindowList.begin(); itr_window_docked != e_global->dockedWindowList.end(); itr_window_docked++)
	{
		UINT state;
		WINDOWPLACEMENT windowinfo;
		GetWindowPlacement(*itr_window_docked, &windowinfo);
		UINT winstate = windowinfo.showCmd;
		BOOL isVisible = IsWindowVisible(*itr_window_docked);
		BOOL isMinimized = IsIconic(*itr_window_docked);

		if (winstate == SW_HIDE)
			::ShowWindow(*itr_window_docked, SW_SHOW);

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
			RECT windowRECT
			{
				getSize.left,
				getSize.top,
				getSize.right,
				getSize.bottom
			};

			E_Window* getttingWindow = new E_Window(*itr_window_docked);
			HGDIOBJ switchBitmap = getttingWindow->getScreenshot()->GetSafeHandle();

			cdc_switch.SelectObject(switchBitmap);
			memDC_switch.StretchBlt(windowRECT.left, windowRECT.top,
				windowRECT.right - windowRECT.left, windowRECT.bottom - windowRECT.top,
				&cdc_switch, 0, 0, windowRECT.right - windowRECT.left, windowRECT.bottom - windowRECT.top, SRCCOPY);
	}
	}

	std::list<E_Window*> switch_window_list = e_global->getDesktop(drSwitcher->switchIndex)->windowList;
	std::list<E_Window*> switch_window_list_topmost;
	for (std::list<E_Window*>::iterator itr_window = switch_window_list.begin(); itr_window != switch_window_list.end(); itr_window++)
	{
		UINT state;
		WINDOWPLACEMENT windowinfo;
		GetWindowPlacement((*itr_window)->getWindow(), &windowinfo);
		UINT winstate = windowinfo.showCmd;
		BOOL isVisible = IsWindowVisible((*itr_window)->getWindow());
		BOOL isMinimized = IsIconic((*itr_window)->getWindow());

		if (IsWindow((*itr_window)->getWindow()) && (!(*itr_window)->dock)
			&& !((state = winstate) == SW_FORCEMINIMIZE
			|| (state = winstate) == SW_HIDE		//HIDE는 사실 처리 안됨 (invisible)
			|| (state = winstate) == SW_MINIMIZE
			|| (state = winstate) == SW_SHOWMINIMIZED
			|| (state = winstate) == SW_SHOWMINNOACTIVE
			|| isVisible == FALSE
			|| isMinimized == TRUE))
		{
			DWORD dwExStyle = ::GetWindowLong((*itr_window)->getWindow(), GWL_EXSTYLE);
			if ((dwExStyle & WS_EX_TOPMOST) != 0)
			{
				switch_window_list_topmost.push_front(*itr_window);
			}
			else
			{
				CRect getSize;
				GetWindowRect((*itr_window)->getWindow(), &getSize);

				RECT windowRECT
				{
					getSize.left,
					getSize.top,
					getSize.right,
					getSize.bottom
				};

				(*itr_window)->takeScreenshot();
				HGDIOBJ switchBitmap = (*itr_window)->getScreenshot()->GetSafeHandle();

				cdc_switch.SelectObject(switchBitmap);
				memDC_switch.StretchBlt(windowRECT.left, windowRECT.top,
					windowRECT.right - windowRECT.left, windowRECT.bottom - windowRECT.top,
					&cdc_switch, 0, 0, windowRECT.right - windowRECT.left, windowRECT.bottom - windowRECT.top, SRCCOPY);
			}
		}
	}

	cdc_switch.SelectObject(currentTaskbarBitmap);
	memDC_switch.StretchBlt(0, drSwitcher->currentTaskbarRECT->top,
		drSwitcher->currentTaskbarRECT->right - drSwitcher->currentTaskbarRECT->left,
		drSwitcher->currentTaskbarRECT->bottom - drSwitcher->currentTaskbarRECT->top,
		&cdc_switch, 0, 0, bmpinfo_taskbar.bmWidth, bmpinfo_taskbar.bmHeight, SRCCOPY);

	for (std::list<E_Window*>::iterator itr_window = switch_window_list_topmost.begin(); itr_window != switch_window_list_topmost.end(); itr_window++)
	{
		CRect getSize;
		GetWindowRect((*itr_window)->getWindow(), &getSize);

		RECT windowRECT
		{
			getSize.left,
			getSize.top,
			getSize.right,
			getSize.bottom
		};

		(*itr_window)->takeScreenshot();
		HGDIOBJ switchBitmap = (*itr_window)->getScreenshot()->GetSafeHandle();

		cdc_switch.SelectObject(switchBitmap);
		memDC_switch.StretchBlt(windowRECT.left, windowRECT.top,
			windowRECT.right - windowRECT.left, windowRECT.bottom - windowRECT.top,
			&cdc_switch, 0, 0, windowRECT.right - windowRECT.left, windowRECT.bottom - windowRECT.top, SRCCOPY);
}

	switchDC.StretchBlt(0, 0,
		enManager->getWidth(), enManager->getHeight(),
		&memDC_switch, 0, 0, enManager->getWidth(), enManager->getHeight(), SRCCOPY);
		
	cdc_switch.DeleteDC();
	memDC_switch.DeleteDC();
	bmp_switch.DeleteObject();
}

E_DragAndDropSwitcher::E_DragAndDropSwitcher()
{
	restore = false;
	ison = false;
	started = false;
	switchable = false;
	cursor_left = false;
	cursor_right = false;
	cursor_top = false;
	cursor_bottom = false;

	winThumbProps.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY;
	// Use the window frame and client area
	winThumbProps.fSourceClientAreaOnly = FALSE;
	winThumbProps.fVisible = TRUE;
	winThumbProps.opacity = (255 * 70) / 100;

	currentDesktopRECT = NULL;
	currentTaskbarRECT = NULL;
	switchDesktopRECT = NULL;
	switchTaskbarRECT = NULL;
}

E_DragAndDropSwitcher* E_DragAndDropSwitcher::singleton = NULL;

E_DragAndDropSwitcher::~E_DragAndDropSwitcher()
{
}

E_DragAndDropSwitcher* E_DragAndDropSwitcher::getSingleton()
{
	if (singleton == NULL) {
		singleton = new E_DragAndDropSwitcher();
	}
	return singleton;
}

void E_DragAndDropSwitcher::initSwitcher()
{
	if (!ison)
	{
		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		if (cursor_left)
		{
			main_left = 0;
			main_top = enManager->getHeight() * 0.3;
			main_right = 5;
			main_bottom = enManager->getHeight() * 0.7;
		}
		else if (cursor_right)
		{
			main_left = enManager->getWidth() - 5;
			main_top = enManager->getHeight() * 0.3;
			main_right = enManager->getWidth();
			main_bottom = enManager->getHeight() * 0.7;
		}
		else if (cursor_top)
		{
			main_left = enManager->getWidth() * 0.3;
			main_top = 0;
			main_right = enManager->getWidth() * 0.7;
			main_bottom = 5;
		}
		else
		{
			main_left = enManager->getWidth() * 0.3;
			main_top = enManager->getHeight() - 5;
			main_right = enManager->getWidth() * 0.7;
			main_bottom = enManager->getHeight();
		}
		movingCRect = CRect(main_left, main_top, main_right, main_bottom);
		UINT nClassStyle_window = 0;
		CString szClassName_window = AfxRegisterWndClass(nClassStyle_window, 0, (HBRUSH)CreateSolidBrush(E_DragAndDropSwitcher::backgroundColor), 0);
		CreateEx(WS_EX_TOPMOST, szClassName_window, L"E_DragAndDropSwitcher", WS_VISIBLE | WS_POPUP, movingCRect, CWnd::GetDesktopWindow(), 0);
		//WS_EX_TOPMOST
		ison = true;

		E_Window::setIconInvisible(this->m_hWnd);

		SetCursor(LoadCursor(NULL, IDC_APPSTARTING));
	}
}

void E_DragAndDropSwitcher::startSwitcher()
{
	if (ison && (this->m_hWnd != NULL))
	{
		E_Global* e_global = E_Global::getSingleton();
		initindex = e_global->getSelectedIndex();
		E_Window::setIconInvisible(this->m_hWnd);

		e_global->onUpdate();

		hTaskbarWnd = ::FindWindowW(_T("Shell_TrayWnd"), NULL); // 작업표시줄 hwnd
		::GetWindowRect(hTaskbarWnd, &(sizeRect_taskbar)); // 작업표시줄 크기 얻기
		hShellWnd = ::GetShellWindow(); // 바탕화면 hwnd
		::GetWindowRect(hShellWnd, &(sizeRect_background)); // 바탕화면 크기 얻기


		list<E_Desktop*>::iterator itr_desktop = e_global->desktopList.begin();
		switchIndex = 0;
		if (cursor_left)
		{
			if ((e_global->getSelectedIndex() % e_global->getDesktopWidth()) == 0)
			{
				for (int p = 0; p != (e_global->getSelectedIndex() + e_global->getDesktopWidth() - 1); p++)
				{
					itr_desktop++;
					switchIndex++;
				}
			}
			else
			{
				for (int p = 0; p < e_global->getSelectedIndex() - 1; p++)
				{
					itr_desktop++;
					switchIndex++;
				}
			}
			
			switchDesktop = *itr_desktop;
		}
		else if (cursor_right)
		{
			if (((e_global->getSelectedIndex() + 1) % e_global->getDesktopWidth()) == 0)
			{
				switchIndex = ((int)(e_global->getSelectedIndex() / e_global->getDesktopWidth())) * e_global->getDesktopWidth();
			}
			else
			{
				switchIndex = e_global->getSelectedIndex() + 1;
			}
			for (int j = 0; j < switchIndex; j++)
			{
				itr_desktop++;
			}
			switchDesktop = *itr_desktop;
		}
		else if (cursor_top)
		{
			if ((e_global->getSelectedIndex() / e_global->getDesktopWidth()) == 0)
			{
				for (int p = 0; p != (e_global->getDesktopCount() - (e_global->getDesktopWidth() - e_global->getSelectedIndex())); p++)
				{
					itr_desktop++;
					switchIndex++;
				}
			}
			else
			{
				for (int p = 0; p != (e_global->getSelectedIndex() - e_global->getDesktopWidth()); p++)
				{
					itr_desktop++;
					switchIndex++;
				}
			}
			switchDesktop = *itr_desktop;
		}
		else
		{
			if (((int)(e_global->getSelectedIndex() / e_global->getDesktopWidth())) == (e_global->getDesktopHeight() - 1))
			{
				for (int p = 0; p != (e_global->getSelectedIndex() % e_global->getDesktopWidth()); p++)
				{
					itr_desktop++;
					switchIndex++;
				}
			}
			else
			{
				for (int p = 0; p != e_global->getSelectedIndex() + e_global->getDesktopWidth(); p++)
				{
					itr_desktop++;
					switchIndex++;
				}
			}
			switchDesktop = *itr_desktop;
		}

		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		if (E_AeroPeekController::getSingleton()->isAeroPeekMode())
		{
			drawDragAndDropSwitcher();

			if (cursor_left)
			{
				main_left = 0;
				main_top = 0;
				main_right = enManager->getWidth() * 2;
				main_bottom = enManager->getHeight();
				currentTaskbarRECT->left += -enManager->getWidth();
				currentTaskbarRECT->right += -enManager->getWidth();
			}
			else if (cursor_right)
			{
				main_left = 0;
				main_top = 0;
				main_right = enManager->getWidth() * 2;
				main_bottom = enManager->getHeight();
			}
			else if (cursor_top)
			{
				main_left = 0;				
				main_top = 0;
				main_right = enManager->getWidth();
				main_bottom = enManager->getHeight() * 2;
				currentTaskbarRECT->top += -enManager->getHeight();
				currentTaskbarRECT->bottom += -enManager->getHeight();
			}
			else
			{
				main_left = 0;
				main_top = 0;
				main_right = enManager->getWidth();
				main_bottom = enManager->getHeight() * 2;
			}

			movingCRect = CRect(main_left, main_top, main_right, main_bottom);
			MoveWindow(movingCRect);

			ShowWindow(SW_SHOW);

			::GetCursorPos(&prev_point);

			SetCursor(LoadCursor(NULL, IDC_HAND));

			restore = true;
		}
		else
		{
			if (cursor_left)
			{
				main_left = 0;
				main_top = 0;
				main_right = enManager->getWidth() * 2;
				main_bottom = enManager->getHeight();
				switchCWndRECT = new RECT{ -enManager->getWidth(), 0, 0, enManager->getHeight() };
			}
			else if (cursor_right)
			{
				main_left = 0;
				main_top = 0;
				main_right = enManager->getWidth() * 2;
				main_bottom = enManager->getHeight();
				switchCWndRECT = new RECT{ enManager->getWidth(), 0, enManager->getWidth() * 2, enManager->getHeight() };
			}
			else if (cursor_top)
			{
				main_left = 0;
				main_top = 0;
				main_right = enManager->getWidth();
				main_bottom = enManager->getHeight() * 2;
				switchCWndRECT = new RECT{ 0, -enManager->getHeight(), enManager->getWidth(), 0 };
			}
			else
			{
				main_left = 0;
				main_top = 0;
				main_right = enManager->getWidth();
				main_bottom = enManager->getHeight() * 2;
				switchCWndRECT = new RECT{ 0, enManager->getHeight(), enManager->getWidth(), enManager->getHeight() * 2 };
			}

			movingCRect = CRect(main_left, main_top, main_right, main_bottom);
			MoveWindow(movingCRect);

			currentCWndRECT = new RECT{ 0, 0, enManager->getWidth(), enManager->getHeight() };
			currentTaskbarRECT = new CRect(sizeRect_taskbar);

			UINT nClassStyle_window = 0;
			CString szClassName_window = AfxRegisterWndClass(nClassStyle_window, 0, (HBRUSH)CreateSolidBrush(NULL_BRUSH), 0);
			currentCWnd = new CWnd();
			currentCWnd->CreateEx(WS_EX_TOPMOST, szClassName_window, L"E_DragAndDropSwitcher", WS_VISIBLE | WS_POPUP, *currentCWndRECT, this, 0, NULL);
			//WS_EX_TOPMOST
			E_Window::setIconInvisible(currentCWnd->m_hWnd);
			switchCWnd = new CWnd();
			switchCWnd->CreateEx(WS_EX_TOPMOST, szClassName_window, L"E_DragAndDropSwitcher", WS_VISIBLE | WS_POPUP, *currentCWndRECT, this, 0, NULL);
			//WS_EX_TOPMOST
			E_Window::setIconInvisible(switchCWnd->m_hWnd);
			
			desktopWindow = new E_Window(hShellWnd);
			taskbarWindow = new E_Window(hTaskbarWnd);


			::MoveWindow(switchCWnd->GetSafeHwnd(), switchCWndRECT->left, switchCWndRECT->top, switchCWndRECT->right - switchCWndRECT->left, switchCWndRECT->bottom - switchCWndRECT->top, TRUE);
			drawDragAndDropSwitcher_noAero();

			if (cursor_left)
			{
				movingCRect = CRect(-enManager->getWidth(), 0, enManager->getWidth(), enManager->getHeight());
				MoveWindow(movingCRect);
			}
			else if (cursor_top)
			{
				movingCRect = CRect(0, -enManager->getHeight(), enManager->getWidth(), enManager->getHeight());
				MoveWindow(movingCRect);
			}

			ShowWindow(SW_SHOW);

			::GetCursorPos(&prev_point);

			SetCursor(LoadCursor(NULL, IDC_HAND));

			restore = true;
		}
	}
	else
	{
		terminateSwitcher();
	}
}

void E_DragAndDropSwitcher::terminateSwitcher()
{
	if (started && restore)
	{
		KillTimer(1);
	}
	if (restore)
	{
		E_Global::getSingleton()->getSelectedDesktop()->setAllIconInvisible();
		E_Global::getSingleton()->setSelectedIndex(initindex);
		E_Global::getSingleton()->getSelectedDesktop()->setAllIconVisible();
		restore = false;
	}
	if (started)
	{
		E_Window::setIconVisible(this->m_hWnd);
		DestroyWindow();

		ison = false;
		cursor_left = false;
		cursor_right = false;
		cursor_top = false;
		cursor_bottom = false;

		started = false;
		switchable = false;

		E_AeroPeekController::getSingleton()->unregisterAllAreo();
		
		if (currentTaskbarRECT != NULL)
		{
			delete currentDesktopRECT;
			delete currentTaskbarRECT;
			delete switchDesktopRECT;
			delete switchTaskbarRECT;
			currentDesktopRECT = NULL;
			currentTaskbarRECT = NULL;
			switchDesktopRECT = NULL;
			switchTaskbarRECT = NULL;
			::SetLayeredWindowAttributes(hTaskbarWnd, 0, 255, LWA_ALPHA); //투명해제
			::SetWindowLongW(hTaskbarWnd, GWL_EXSTYLE, GetWindowLong(hTaskbarWnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
		}

		if (E_DesktopSwitcher::getSingleton()->ison)
			E_DesktopSwitcher::getSingleton()->terminateSwitcher();

		if (initindex != E_Global::getSingleton()->getSelectedIndex())
			::SendMessage(E_Global::getSingleton()->hwnd_frame, WM_TRAY_EVENT, switchIndex, 0);
	}
	if (ison)
	{
		E_Window::setIconVisible(this->m_hWnd);
		DestroyWindow();

		ison = false;
		cursor_left = false;
		cursor_right = false;
		cursor_top = false;
		cursor_bottom = false;
	}
	SetCursor(LoadCursor(NULL, IDC_ARROW));
}

BEGIN_MESSAGE_MAP(E_DragAndDropSwitcher, CWnd)
	ON_WM_CREATE()
	ON_WM_TIMER()
ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

int E_DragAndDropSwitcher::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetTimer(1, 10, NULL);

	return 0;
}


void movingRects_x(int x)
{
	E_DragAndDropSwitcher* drSwitcher = E_DragAndDropSwitcher::getSingleton();
	drSwitcher->movingCRect.left += x;
	drSwitcher->movingCRect.right += x;
	if (!E_AeroPeekController::getSingleton()->isAeroPeekMode())
	{
		drSwitcher->currentCWndRECT->left += x;
		drSwitcher->currentCWndRECT->right += x;
		drSwitcher->switchCWndRECT->left += x;
		drSwitcher->switchCWndRECT->right += x;
}
}

void movingRects_y(int y)
{
	E_DragAndDropSwitcher* drSwitcher = E_DragAndDropSwitcher::getSingleton();
	drSwitcher->movingCRect.top += y;
	drSwitcher->movingCRect.bottom += y;
	if (!E_AeroPeekController::getSingleton()->isAeroPeekMode())
	{
		drSwitcher->currentCWndRECT->top += y;
		drSwitcher->currentCWndRECT->bottom += y;
		drSwitcher->switchCWndRECT->top += y;
		drSwitcher->switchCWndRECT->bottom += y;
}
}

void E_DragAndDropSwitcher::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == 1)
	{
		POINT point;
		GetCursorPos(&point);
		if (ison && !(movingCRect.PtInRect(point)))
		{
			terminateSwitcher();
		}
		else if ((GetAsyncKeyState(VK_RBUTTON) && 0x8000) && !(GetAsyncKeyState(VK_LBUTTON) && 0x8000))
		{
			if (!started)
			{
				started = true;

				startSwitcher();
				
				if (E_AeroPeekController::getSingleton()->isAeroPeekMode())
				{
					E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
					if (cursor_left)
					{
						movingCRect = CRect(-enManager->getWidth(), 0, enManager->getWidth(), enManager->getHeight());
						MoveWindow(movingCRect);
					}
					else if (cursor_top)
					{
						movingCRect = CRect(0, -enManager->getHeight(), enManager->getWidth(), enManager->getHeight());
						MoveWindow(movingCRect);
					}
					::SetWindowLongW(hTaskbarWnd, GWL_EXSTYLE, GetWindowLong(hTaskbarWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
					::SetLayeredWindowAttributes(hTaskbarWnd, 0, 0, LWA_ALPHA);
				}
				
				E_Window::setIconInvisible(this->m_hWnd);

				//E_Global::getSingleton()->getDesktop(switchIndex)->setAllIconInvisible();
			}
			else
			{
				E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
				E_Global* e_global = E_Global::getSingleton();

				if ((cursor_left || cursor_right) && (abs(point.x - prev_point.x) > 3))
				{
					movingRects_x(point.x - prev_point.x);
					MoveWindow(movingCRect, FALSE);
					
					if (!E_AeroPeekController::getSingleton()->isAeroPeekMode())
					{
						if (cursor_left)
							prev_point.x = currentCWndRECT->left;
						else
							prev_point.x = currentCWndRECT->right;
						prev_point.y = point.y;
						::MoveWindow(switchCWnd->GetSafeHwnd(), switchCWndRECT->left, switchCWndRECT->top, switchCWndRECT->right - switchCWndRECT->left, switchCWndRECT->bottom - switchCWndRECT->top, TRUE);
						::MoveWindow(currentCWnd->GetSafeHwnd(), currentCWndRECT->left, currentCWndRECT->top, currentCWndRECT->right - currentCWndRECT->left, currentCWndRECT->bottom - currentCWndRECT->top, TRUE);
						drawDragAndDropSwitcher_noAero();
						if ((((currentCWndRECT->left * 100 / enManager->getWidth()) >= 33) && cursor_left) || ((((-currentCWndRECT->left) * 100 / enManager->getWidth()) >= 33) && cursor_right))
						{
							if (!switchable)
							{
								e_global->getSelectedDesktop()->setAllIconInvisible();
								e_global->getDesktop(switchIndex)->setAllIconVisible();
								switchable = true;
							}
						}
						else if ((((-currentCWndRECT->left * 100 / enManager->getWidth()) < 33) && cursor_right) || ((((currentCWndRECT->left) * 100 / enManager->getWidth()) < 33) && cursor_left))
						{
							if (switchable)
							{
								e_global->getDesktop(switchIndex)->setAllIconInvisible();
								e_global->getSelectedDesktop()->setAllIconVisible();
								switchable = false;
							}
						}
					}
					
					else
					{
						prev_point = point;
					if ((((prev_point.x * 100 / enManager->getWidth()) >= 33) && cursor_left) || ((((enManager->getWidth() - prev_point.x) * 100 / enManager->getWidth()) >= 33) && cursor_right))
					{
						if (!switchable)
						{
							e_global->getSelectedDesktop()->setAllIconInvisible();
							e_global->getDesktop(switchIndex)->setAllIconVisible();
							switchable = true;
						}
					}
					else if ((((prev_point.x * 100 / enManager->getWidth()) >= 33) && cursor_right) || ((((enManager->getWidth() - prev_point.x) * 100 / enManager->getWidth()) >= 33) && cursor_left))
					{
						if (switchable)
						{
							e_global->getDesktop(switchIndex)->setAllIconInvisible();
							e_global->getSelectedDesktop()->setAllIconVisible();
							switchable = false;
						}
					}
				}
				}
				else if ((cursor_top || cursor_bottom) && (abs(point.y - prev_point.y) > 3))
				{
					movingRects_y(point.y - prev_point.y);
					MoveWindow(movingCRect);
					
					if (!E_AeroPeekController::getSingleton()->isAeroPeekMode())
					{
						if (cursor_top)
							prev_point.y = currentCWndRECT->top;
						else
							prev_point.y = currentCWndRECT->bottom;
						prev_point.x = point.x;
						::MoveWindow(switchCWnd->GetSafeHwnd(), switchCWndRECT->left, switchCWndRECT->top, switchCWndRECT->right - switchCWndRECT->left, switchCWndRECT->bottom - switchCWndRECT->top, TRUE);
						::MoveWindow(currentCWnd->GetSafeHwnd(), currentCWndRECT->left, currentCWndRECT->top, currentCWndRECT->right - currentCWndRECT->left, currentCWndRECT->bottom - currentCWndRECT->top, TRUE);
						drawDragAndDropSwitcher_noAero();
						if ((((currentCWndRECT->top * 100 / enManager->getHeight()) >= 33) && cursor_top) || ((((-currentCWndRECT->top) * 100 / enManager->getHeight()) >= 33) && cursor_bottom))
						{
							if (!switchable)
							{
								e_global->getSelectedDesktop()->setAllIconInvisible();
								e_global->getDesktop(switchIndex)->setAllIconVisible();
								switchable = true;
							}
						}
						else if ((((-currentCWndRECT->top * 100 / enManager->getHeight()) < 33) && cursor_bottom) || ((((currentCWndRECT->top) * 100 / enManager->getHeight()) < 33) && cursor_top))
						{
							if (switchable)
							{
								e_global->getDesktop(switchIndex)->setAllIconInvisible();
								e_global->getSelectedDesktop()->setAllIconVisible();
								switchable = false;
							}
						}
					}
					
					else
					{
						prev_point = point;
					if ((((prev_point.y * 100 / enManager->getHeight()) >= 33) && cursor_top) || ((((enManager->getHeight() - prev_point.y) * 100 / enManager->getHeight()) >= 33) && cursor_bottom))
					{
						if (!switchable)
						{
							e_global->getSelectedDesktop()->setAllIconInvisible();
							e_global->getDesktop(switchIndex)->setAllIconVisible();
							switchable = true;
						}
					}
					else if ((((prev_point.y * 100 / enManager->getHeight()) >= 33) && cursor_bottom) || ((((enManager->getHeight() - prev_point.y) * 100 / enManager->getHeight()) >= 33) && cursor_top))
					{
						if (switchable)
						{
							e_global->getDesktop(switchIndex)->setAllIconInvisible();
							e_global->getSelectedDesktop()->setAllIconVisible();
							switchable = false;
						}
					}
				}
			}
		}
		}
		else if (started)
		{
			KillTimer(1);
			restore = false;
			E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
			E_Global* e_global = E_Global::getSingleton();
			if (!E_AeroPeekController::getSingleton()->isAeroPeekMode())
			{
			if (cursor_left)
			{
				if (switchable)
				{
						while (currentCWndRECT->left <= enManager->getWidth())
					{
							movingRects_x(15);
						MoveWindow(movingCRect, FALSE);
							::MoveWindow(switchCWnd->GetSafeHwnd(), switchCWndRECT->left, switchCWndRECT->top, switchCWndRECT->right - switchCWndRECT->left, switchCWndRECT->bottom - switchCWndRECT->top, TRUE);
							::MoveWindow(currentCWnd->GetSafeHwnd(), currentCWndRECT->left, currentCWndRECT->top, currentCWndRECT->right - currentCWndRECT->left, currentCWndRECT->bottom - currentCWndRECT->top, TRUE);
							drawDragAndDropSwitcher_noAero();
							prev_point.x += 15;
					}
					Sleep(60);

						e_global->getSelectedDesktop()->setAllIconVisible();

						for (list<E_Window*>::iterator itr = e_global->getSelectedDesktop()->windowList.begin(); itr != e_global->getSelectedDesktop()->windowList.end(); itr++)
					{
							if (!(*itr)->dock)
							{
								(*itr)->SetMinimizeMaximizeAnimation(FALSE);
								(*itr)->setHide();
								(*itr)->SetMinimizeMaximizeAnimation(TRUE);
					}
						}
						e_global->setSelectedIndex(switchIndex);
					}
					else
					{
						while (currentCWndRECT->left >= 0)
						{
							movingRects_x(-15);
						MoveWindow(movingCRect, FALSE);
							::MoveWindow(switchCWnd->GetSafeHwnd(), switchCWndRECT->left, switchCWndRECT->top, switchCWndRECT->right - switchCWndRECT->left, switchCWndRECT->bottom - switchCWndRECT->top, TRUE);
							::MoveWindow(currentCWnd->GetSafeHwnd(), currentCWndRECT->left, currentCWndRECT->top, currentCWndRECT->right - currentCWndRECT->left, currentCWndRECT->bottom - currentCWndRECT->top, TRUE);
							drawDragAndDropSwitcher_noAero();
							prev_point.x += -15;
					}
					Sleep(60);

						switchDesktop->setAllIconVisible();

						for (list<E_Window*>::iterator itr = switchDesktop->windowList.begin(); itr != switchDesktop->windowList.end(); itr++)
					{
							if (!(*itr)->dock)
							{
								(*itr)->SetMinimizeMaximizeAnimation(FALSE);
								(*itr)->setHide();
								(*itr)->SetMinimizeMaximizeAnimation(TRUE);
					}
						}
					}

					Sleep(300);
					terminateSwitcher();
				}
				else if (cursor_right)
					{
					if (switchable)
					{
						while (currentCWndRECT->right >= 0)
						{
							movingRects_x(-15);
						MoveWindow(movingCRect, FALSE);
							::MoveWindow(switchCWnd->GetSafeHwnd(), switchCWndRECT->left, switchCWndRECT->top, switchCWndRECT->right - switchCWndRECT->left, switchCWndRECT->bottom - switchCWndRECT->top, TRUE);
							::MoveWindow(currentCWnd->GetSafeHwnd(), currentCWndRECT->left, currentCWndRECT->top, currentCWndRECT->right - currentCWndRECT->left, currentCWndRECT->bottom - currentCWndRECT->top, TRUE);
							drawDragAndDropSwitcher_noAero();
							prev_point.x += -15;
					}
					Sleep(60);
					
						e_global->getSelectedDesktop()->setAllIconVisible();
					for (list<E_Window*>::iterator itr = e_global->getSelectedDesktop()->windowList.begin(); itr != e_global->getSelectedDesktop()->windowList.end(); itr++)
					{
						if (!(*itr)->dock)
						{
							(*itr)->SetMinimizeMaximizeAnimation(FALSE);
							(*itr)->setHide();
							(*itr)->SetMinimizeMaximizeAnimation(TRUE);
						}
					}
					e_global->setSelectedIndex(switchIndex);
				}
				else
				{
						while (currentCWndRECT->right <= enManager->getWidth())
					{
							movingRects_x(15);
						MoveWindow(movingCRect, FALSE);
							::MoveWindow(switchCWnd->GetSafeHwnd(), switchCWndRECT->left, switchCWndRECT->top, switchCWndRECT->right - switchCWndRECT->left, switchCWndRECT->bottom - switchCWndRECT->top, TRUE);
							::MoveWindow(currentCWnd->GetSafeHwnd(), currentCWndRECT->left, currentCWndRECT->top, currentCWndRECT->right - currentCWndRECT->left, currentCWndRECT->bottom - currentCWndRECT->top, TRUE);
							drawDragAndDropSwitcher_noAero();
							prev_point.x += 15;
					}
					Sleep(60);

						switchDesktop->setAllIconVisible();

						for (list<E_Window*>::iterator itr = switchDesktop->windowList.begin(); itr != switchDesktop->windowList.end(); itr++)
						{
							if (!(*itr)->dock)
					{
								(*itr)->SetMinimizeMaximizeAnimation(FALSE);
								(*itr)->setHide();
								(*itr)->SetMinimizeMaximizeAnimation(TRUE);
							}
						}
					}

					Sleep(300);
					terminateSwitcher();
					}
				else if (cursor_top)
				{
					if (switchable)
					{
						while (currentCWndRECT->top <= enManager->getHeight())
					{
							movingRects_y(15);
						MoveWindow(movingCRect, FALSE);
							::MoveWindow(switchCWnd->GetSafeHwnd(), switchCWndRECT->left, switchCWndRECT->top, switchCWndRECT->right - switchCWndRECT->left, switchCWndRECT->bottom - switchCWndRECT->top, TRUE);
							::MoveWindow(currentCWnd->GetSafeHwnd(), currentCWndRECT->left, currentCWndRECT->top, currentCWndRECT->right - currentCWndRECT->left, currentCWndRECT->bottom - currentCWndRECT->top, TRUE);
							drawDragAndDropSwitcher_noAero();
							prev_point.y += 15;
					}
					Sleep(60);

						e_global->getSelectedDesktop()->setAllIconVisible();

						for (list<E_Window*>::iterator itr = e_global->getSelectedDesktop()->windowList.begin(); itr != e_global->getSelectedDesktop()->windowList.end(); itr++)
						{
							if (!(*itr)->dock)
					{
								(*itr)->SetMinimizeMaximizeAnimation(FALSE);
								(*itr)->setHide();
								(*itr)->SetMinimizeMaximizeAnimation(TRUE);
							}
						}
						e_global->setSelectedIndex(switchIndex);
					}
					else
					{
						while (currentCWndRECT->top >= 0)
					{
							movingRects_y(-15);
						MoveWindow(movingCRect, FALSE);
							::MoveWindow(switchCWnd->GetSafeHwnd(), switchCWndRECT->left, switchCWndRECT->top, switchCWndRECT->right - switchCWndRECT->left, switchCWndRECT->bottom - switchCWndRECT->top, TRUE);
							::MoveWindow(currentCWnd->GetSafeHwnd(), currentCWndRECT->left, currentCWndRECT->top, currentCWndRECT->right - currentCWndRECT->left, currentCWndRECT->bottom - currentCWndRECT->top, TRUE);
							drawDragAndDropSwitcher_noAero();
							prev_point.y += -15;
					}
					Sleep(60);

					switchDesktop->setAllIconVisible();
					
					for (list<E_Window*>::iterator itr = switchDesktop->windowList.begin(); itr != switchDesktop->windowList.end(); itr++)
					{
						if (!(*itr)->dock)
						{
							(*itr)->SetMinimizeMaximizeAnimation(FALSE);
							(*itr)->setHide();
							(*itr)->SetMinimizeMaximizeAnimation(TRUE);
						}
					}
				}

				Sleep(300);
				terminateSwitcher();
			}
				else
			{
				if (switchable)
				{
						while (currentCWndRECT->bottom >= 0)
					{
							movingRects_y(-15);
						MoveWindow(movingCRect, FALSE);
							::MoveWindow(switchCWnd->GetSafeHwnd(), switchCWndRECT->left, switchCWndRECT->top, switchCWndRECT->right - switchCWndRECT->left, switchCWndRECT->bottom - switchCWndRECT->top, TRUE);
							::MoveWindow(currentCWnd->GetSafeHwnd(), currentCWndRECT->left, currentCWndRECT->top, currentCWndRECT->right - currentCWndRECT->left, currentCWndRECT->bottom - currentCWndRECT->top, TRUE);
							drawDragAndDropSwitcher_noAero();
							prev_point.y += -15;
					}
					Sleep(60);

						e_global->getSelectedDesktop()->setAllIconVisible();

						for (list<E_Window*>::iterator itr = e_global->getSelectedDesktop()->windowList.begin(); itr != e_global->getSelectedDesktop()->windowList.end(); itr++)
						{
							if (!(*itr)->dock)
					{
								(*itr)->SetMinimizeMaximizeAnimation(FALSE);
								(*itr)->setHide();
								(*itr)->SetMinimizeMaximizeAnimation(TRUE);
							}
						}
						e_global->setSelectedIndex(switchIndex);

					}
					else
					{
						while (currentCWndRECT->bottom <= enManager->getHeight())
					{
							movingRects_y(15);
						MoveWindow(movingCRect, FALSE);
							::MoveWindow(switchCWnd->GetSafeHwnd(), switchCWndRECT->left, switchCWndRECT->top, switchCWndRECT->right - switchCWndRECT->left, switchCWndRECT->bottom - switchCWndRECT->top, TRUE);
							::MoveWindow(currentCWnd->GetSafeHwnd(), currentCWndRECT->left, currentCWndRECT->top, currentCWndRECT->right - currentCWndRECT->left, currentCWndRECT->bottom - currentCWndRECT->top, TRUE);
							drawDragAndDropSwitcher_noAero();
							prev_point.y += 15;
					}
					Sleep(60);

						switchDesktop->setAllIconVisible();

						for (list<E_Window*>::iterator itr = switchDesktop->windowList.begin(); itr != switchDesktop->windowList.end(); itr++)
						{
							if (!(*itr)->dock)
					{
								(*itr)->SetMinimizeMaximizeAnimation(FALSE);
								(*itr)->setHide();
								(*itr)->SetMinimizeMaximizeAnimation(TRUE);
							}
						}
					}

					Sleep(300);
					terminateSwitcher();
				}
					}

			else
			{
				if (cursor_left)
				{
					if (switchable)
					{
						while (prev_point.x <= enManager->getWidth())
					{
							movingRects_x(2);
						MoveWindow(movingCRect, FALSE);
							prev_point.x += 2;
							Sleep(0);
					}
					Sleep(60);

					e_global->getSelectedDesktop()->setAllIconVisible();

					for (list<E_Window*>::iterator itr = e_global->getSelectedDesktop()->windowList.begin(); itr != e_global->getSelectedDesktop()->windowList.end(); itr++)
					{
						if (!(*itr)->dock)
						{
							(*itr)->SetMinimizeMaximizeAnimation(FALSE);
							(*itr)->setHide();
							(*itr)->SetMinimizeMaximizeAnimation(TRUE);
						}
					}
					e_global->setSelectedIndex(switchIndex);
				}
				else
				{
						while (prev_point.x >= 0)
					{
							movingRects_x(-2);
						MoveWindow(movingCRect, FALSE);
							prev_point.x += -2;
						Sleep(0);
					}
					Sleep(60);

						switchDesktop->setAllIconVisible();

						for (list<E_Window*>::iterator itr = switchDesktop->windowList.begin(); itr != switchDesktop->windowList.end(); itr++)
						{
							if (!(*itr)->dock)
					{
								(*itr)->SetMinimizeMaximizeAnimation(FALSE);
								(*itr)->setHide();
								(*itr)->SetMinimizeMaximizeAnimation(TRUE);
							}
						}
					}

					Sleep(300);
					terminateSwitcher();
					}
				else if (cursor_right)
				{
					if (switchable)
					{
						while (prev_point.x >= 0)
					{
						movingRects_x(-2);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += -2;
							Sleep(0);
					}
					Sleep(60);

						e_global->getSelectedDesktop()->setAllIconVisible();
						for (list<E_Window*>::iterator itr = e_global->getSelectedDesktop()->windowList.begin(); itr != e_global->getSelectedDesktop()->windowList.end(); itr++)
						{
							if (!(*itr)->dock)
					{
								(*itr)->SetMinimizeMaximizeAnimation(FALSE);
								(*itr)->setHide();
								(*itr)->SetMinimizeMaximizeAnimation(TRUE);
							}
						}
						e_global->setSelectedIndex(switchIndex);
					}
					else
					{
						while (prev_point.x <= enManager->getWidth())
					{
							movingRects_x(2);
						MoveWindow(movingCRect, FALSE);
							prev_point.x += 2;
							Sleep(0);
					}
					Sleep(60);

					switchDesktop->setAllIconVisible();
					
					for (list<E_Window*>::iterator itr = switchDesktop->windowList.begin(); itr != switchDesktop->windowList.end(); itr++)
					{
						if (!(*itr)->dock)
						{
							(*itr)->SetMinimizeMaximizeAnimation(FALSE);
							(*itr)->setHide();
							(*itr)->SetMinimizeMaximizeAnimation(TRUE);
						}
					}
				}

				Sleep(300);
				terminateSwitcher();
			}
			else if (cursor_top)
			{
				if (switchable)
				{
					while (prev_point.y <= enManager->getHeight())
					{
						movingRects_y(2);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += 2;
						Sleep(0);
					}
					Sleep(60);

					e_global->getSelectedDesktop()->setAllIconVisible();
					
					for (list<E_Window*>::iterator itr = e_global->getSelectedDesktop()->windowList.begin(); itr != e_global->getSelectedDesktop()->windowList.end(); itr++)
					{
						if (!(*itr)->dock)
						{
							(*itr)->SetMinimizeMaximizeAnimation(FALSE);
							(*itr)->setHide();
							(*itr)->SetMinimizeMaximizeAnimation(TRUE);
						}
					}
					e_global->setSelectedIndex(switchIndex);
				}
				else
				{
					while (prev_point.y >= 0)
					{
						movingRects_y(-2);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += -2;
						Sleep(0);
					}
					Sleep(60);

					switchDesktop->setAllIconVisible();
					
					for (list<E_Window*>::iterator itr = switchDesktop->windowList.begin(); itr != switchDesktop->windowList.end(); itr++)
					{
						if (!(*itr)->dock)
						{
							(*itr)->SetMinimizeMaximizeAnimation(FALSE);
							(*itr)->setHide();
							(*itr)->SetMinimizeMaximizeAnimation(TRUE);
						}
					}
				}

				Sleep(300);
				terminateSwitcher();
			}
			else
			{
				if (switchable)
				{
					while (prev_point.y >= 0)
					{
						movingRects_y(-2);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += -2;
						Sleep(0);
					}
					Sleep(60);

					e_global->getSelectedDesktop()->setAllIconVisible();
					
					for (list<E_Window*>::iterator itr = e_global->getSelectedDesktop()->windowList.begin(); itr != e_global->getSelectedDesktop()->windowList.end(); itr++)
					{
						if (!(*itr)->dock)
						{
							(*itr)->SetMinimizeMaximizeAnimation(FALSE);
							(*itr)->setHide();
							(*itr)->SetMinimizeMaximizeAnimation(TRUE);
						}
					}
					e_global->setSelectedIndex(switchIndex);
					
				}
				else
				{
					while (prev_point.y <= enManager->getHeight())
					{
						movingRects_y(2);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += 2;
						Sleep(0);
					}
					Sleep(60);

					switchDesktop->setAllIconVisible();
					
					for (list<E_Window*>::iterator itr = switchDesktop->windowList.begin(); itr != switchDesktop->windowList.end(); itr++)
					{
						if (!(*itr)->dock)
						{
							(*itr)->SetMinimizeMaximizeAnimation(FALSE);
							(*itr)->setHide();
							(*itr)->SetMinimizeMaximizeAnimation(TRUE);
						}
					}
				}

				Sleep(300);
				terminateSwitcher();
			}
			}

			for (list<HWND>::iterator itr = e_global->dockedWindowList.begin(); itr != e_global->dockedWindowList.end(); itr++)
			{
				WINDOWPLACEMENT windowinfo;
				::GetWindowPlacement(*itr, &windowinfo);
				UINT winstate = windowinfo.showCmd;

				if ((winstate == SW_HIDE) && !(::IsWindowVisible(*itr)))
					::ShowWindow(*itr, SW_SHOW);
			}
		}
	}

	__super::OnTimer(nIDEvent);
}

void E_DragAndDropSwitcher::OnKillFocus(CWnd* pNewWnd)
{
	if (restore)
	{
		terminateSwitcher();
	}

	__super::OnKillFocus(pNewWnd);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
