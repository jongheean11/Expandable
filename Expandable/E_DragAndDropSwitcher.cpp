#include "stdafx.h"
#include "E_DragAndDropSwitcher.h"
#include <Windows.h> 
#include <WinUser.h>
#include "E_Global.h"
#include "E_EnvironmentManager.h"
#include "E_AeroPeekController.h"
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
	
	
	drSwitcher->currentTaskbarRECT = new RECT
	{
		drSwitcher->sizeRect_taskbar.left + plus_width,
		drSwitcher->sizeRect_taskbar.top + plus_height,
		drSwitcher->sizeRect_taskbar.right + plus_width,
		drSwitcher->sizeRect_taskbar.bottom + plus_height
	};
	aeController->registerAero(drSwitcher->hTaskbarWnd, drSwitcher->m_hWnd, *(drSwitcher->currentTaskbarRECT), drSwitcher->currentTaskbarThumbnail);
	
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
		aeController->registerAero(drSwitcher->hTaskbarWnd, drSwitcher->m_hWnd, *(drSwitcher->switchTaskbarRECT), drSwitcher->switchTaskbarThumbnail);

		drSwitcher->switchIndex = 0;
		list<E_Desktop*>::iterator itr_desktop = e_global->desktopList.begin();
		for (int p = 0; p < e_global->getSelectedIndex() - 1; p++)
		{
			itr_desktop++;
			drSwitcher->switchIndex++;
		}
		if ((e_global->getSelectedIndex() % e_global->getDesktopWidth()) == 0)
		{
			for (int p = 0; p != (e_global->getDesktopWidth() - 1); p++)
			{
				itr_desktop++;
				drSwitcher->switchIndex++;
			}
		}
		drSwitcher->switchDesktop = *itr_desktop;
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
		aeController->registerAero(drSwitcher->hTaskbarWnd, drSwitcher->m_hWnd, *(drSwitcher->switchTaskbarRECT), drSwitcher->switchTaskbarThumbnail);

		list<E_Desktop*>::iterator itr_desktop = e_global->desktopList.begin();
		drSwitcher->switchIndex = 0;
		if (((e_global->getSelectedIndex() + 1) % e_global->getDesktopWidth()) == 0)
		{
			drSwitcher->switchIndex = ((int)(e_global->getSelectedIndex() / e_global->getDesktopWidth())) * e_global->getDesktopWidth();
		}
		else
		{
			drSwitcher->switchIndex = e_global->getSelectedIndex() + 1;
		}
		for (int j = 0; j<drSwitcher->switchIndex; j++)
		{
			itr_desktop++;
		}
		drSwitcher->switchDesktop = *itr_desktop;
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
		aeController->registerAero(drSwitcher->hTaskbarWnd, drSwitcher->m_hWnd, *(drSwitcher->switchTaskbarRECT), drSwitcher->switchTaskbarThumbnail);

		list<E_Desktop*>::iterator itr_desktop = e_global->desktopList.begin();
		drSwitcher->switchIndex = 0;
		if ((e_global->getSelectedIndex() / e_global->getDesktopWidth()) == 0)
		{
			for (int p = 0; p != (e_global->getDesktopCount() - (e_global->getDesktopWidth() - e_global->getSelectedIndex())); p++)
			{
				itr_desktop++;
				drSwitcher->switchIndex++;
			}
		}
		else
		{
			for (int p = 0; p != (e_global->getSelectedIndex() - e_global->getDesktopWidth()); p++)
			{
				itr_desktop++; 
				drSwitcher->switchIndex++;
			}
		}
		drSwitcher->switchDesktop = *itr_desktop;
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
		aeController->registerAero(drSwitcher->hTaskbarWnd, drSwitcher->m_hWnd, *(drSwitcher->switchTaskbarRECT), drSwitcher->switchTaskbarThumbnail);

		list<E_Desktop*>::iterator itr_desktop = e_global->desktopList.begin();
		drSwitcher->switchIndex = 0;
		if ((e_global->getSelectedIndex() / e_global->getDesktopWidth()) == e_global->getDesktopWidth() - 1)
		{
			for (int p = 0; p != (e_global->getSelectedIndex() % e_global->getDesktopWidth()); p++)
			{
				itr_desktop++;
				drSwitcher->switchIndex++;
			}
		}
		else
		{
			for (int p = 0; p != e_global->getSelectedIndex() + e_global->getDesktopWidth(); p++)
			{
				itr_desktop++;
				drSwitcher->switchIndex++;
			}
		}
		drSwitcher->switchDesktop = *itr_desktop;
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
	E_Global* e_global = E_Global::getSingleton();
	E_DragAndDropSwitcher* drSwitcher = E_DragAndDropSwitcher::getSingleton();
	drSwitcher->hTaskbarWnd = FindWindowW(_T("Shell_TrayWnd"), NULL); // 작업표시줄 hwnd
	
	GetWindowRect(drSwitcher->hTaskbarWnd, &(drSwitcher->sizeRect_taskbar)); // 작업표시줄 크기 얻기
	drSwitcher->hShellWnd = GetShellWindow(); // 바탕화면 hwnd
	GetWindowRect(drSwitcher->hShellWnd, &(drSwitcher->sizeRect_background)); // 바탕화면 크기 얻기
	
	drawCurrentDesktop();
	drawSwitchDesktop();

	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(*drSwitcher, &ps);
	EndPaint(*drSwitcher, &ps);
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
		CreateEx(WS_EX_TOPMOST, szClassName_window, L"DragAndDropSwitcher", WS_VISIBLE | WS_POPUP, movingCRect, CWnd::GetDesktopWindow(), 0);
		
		ison = true;

		E_Window::setIconInvisible(this->m_hWnd);

		SetCursor(LoadCursor(NULL, IDC_APPSTARTING));
	}
}

void E_DragAndDropSwitcher::startSwitcher()
{
	if (ison)
	{
		initindex = E_Global::getSingleton()->getSelectedIndex();
		E_Window::setIconInvisible(this->m_hWnd);
		
		E_Global::getSingleton()->onUpdate();
		drawDragAndDropSwitcher();

		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		
		if (cursor_left)
		{
			main_left = 0;
			main_top = 0;
			main_right = enManager->getWidth() * 2;
			main_bottom = enManager->getHeight();
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

		GetCursorPos(&prev_point);
		
		ShowWindow(SW_SHOW);
		SetCursor(LoadCursor(NULL, IDC_HAND));
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
	}
	if (ison)
	{
		ison = false;
		cursor_left = false;
		cursor_right = false;
		cursor_top = false;
		cursor_bottom = false;

		SetCursor(LoadCursor(NULL, IDC_ARROW));

		E_Window::setIconVisible(this->m_hWnd);
		DestroyWindow();
	}
}

BEGIN_MESSAGE_MAP(E_DragAndDropSwitcher, CWnd)
	ON_WM_CREATE()
	ON_WM_TIMER()
//	ON_WM_PAINT()
ON_WM_KILLFOCUS()
END_MESSAGE_MAP()
/*
void invalidateSwitcher(LONG diff_x, LONG diff_y)
{
	E_Global* e_global = E_Global::getSingleton();
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_DragAndDropSwitcher* drSwitcher = E_DragAndDropSwitcher::getSingleton();
	E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();

	if ((drSwitcher->cursor_left) || (drSwitcher->cursor_right))
	{
		(*drSwitcher->currentDesktopRECT).left += diff_x;
		(*drSwitcher->currentDesktopRECT).right += diff_x;
		(*drSwitcher->currentTaskbarRECT).left += diff_x;
		(*drSwitcher->currentTaskbarRECT).right += diff_x;

		(*drSwitcher->switchDesktopRECT).left += diff_x;
		(*drSwitcher->switchDesktopRECT).right += diff_x;
		(*drSwitcher->switchTaskbarRECT).left += diff_x;
		(*drSwitcher->switchTaskbarRECT).right += diff_x;
		
		drSwitcher->winThumbProps.rcDestination = *drSwitcher->currentDesktopRECT;
		DwmUpdateThumbnailProperties(drSwitcher->currentDesktopThumbnail, &drSwitcher->winThumbProps);
		drSwitcher->winThumbProps.rcDestination = *drSwitcher->currentTaskbarRECT;
		DwmUpdateThumbnailProperties(drSwitcher->currentTaskbarThumbnail, &drSwitcher->winThumbProps);
		drSwitcher->winThumbProps.rcDestination = *drSwitcher->switchDesktopRECT;
		DwmUpdateThumbnailProperties(drSwitcher->switchDesktopThumbnail, &drSwitcher->winThumbProps);
		drSwitcher->winThumbProps.rcDestination = *drSwitcher->switchTaskbarRECT;
		DwmUpdateThumbnailProperties(drSwitcher->switchTaskbarThumbnail, &drSwitcher->winThumbProps);

		for (hash_map<RECT*, HTHUMBNAIL>::reverse_iterator itr_current_RECT_HTHUMBNAIL = drSwitcher->current_RECT_HTHUMBNAIL_map.rbegin(); itr_current_RECT_HTHUMBNAIL != drSwitcher->current_RECT_HTHUMBNAIL_map.rend(); itr_current_RECT_HTHUMBNAIL++)
		{
			(*itr_current_RECT_HTHUMBNAIL).first->left += diff_x;
			(*itr_current_RECT_HTHUMBNAIL).first->right += diff_x;
			drSwitcher->winThumbProps.rcDestination = *(*itr_current_RECT_HTHUMBNAIL).first;
			DwmUpdateThumbnailProperties((*itr_current_RECT_HTHUMBNAIL).second, &drSwitcher->winThumbProps);
		}

		for (hash_map<RECT*, HTHUMBNAIL>::reverse_iterator itr_switch_RECT_HTHUMBNAIL = drSwitcher->switch_RECT_HTHUMBNAIL_map.rbegin(); itr_switch_RECT_HTHUMBNAIL != drSwitcher->switch_RECT_HTHUMBNAIL_map.rend(); itr_switch_RECT_HTHUMBNAIL++)
		{
			(*itr_switch_RECT_HTHUMBNAIL).first->left += diff_x;
			(*itr_switch_RECT_HTHUMBNAIL).first->right += diff_x;
			drSwitcher->winThumbProps.rcDestination = *(*itr_switch_RECT_HTHUMBNAIL).first;
			DwmUpdateThumbnailProperties((*itr_switch_RECT_HTHUMBNAIL).second, &drSwitcher->winThumbProps);
		}
	}
	else
	{
		(*drSwitcher->currentDesktopRECT).top += diff_y;
		(*drSwitcher->currentDesktopRECT).bottom += diff_y;
		(*drSwitcher->currentTaskbarRECT).top += diff_y;
		(*drSwitcher->currentTaskbarRECT).bottom += diff_y;
		
		(*drSwitcher->switchDesktopRECT).top += diff_y;
		(*drSwitcher->switchDesktopRECT).bottom += diff_y;
		(*drSwitcher->switchTaskbarRECT).top += diff_y;
		(*drSwitcher->switchTaskbarRECT).bottom += diff_y;
		
		drSwitcher->winThumbProps.rcDestination = *drSwitcher->currentDesktopRECT;
		DwmUpdateThumbnailProperties(drSwitcher->currentDesktopThumbnail, &drSwitcher->winThumbProps);
		drSwitcher->winThumbProps.rcDestination = *drSwitcher->currentTaskbarRECT;
		DwmUpdateThumbnailProperties(drSwitcher->currentTaskbarThumbnail, &drSwitcher->winThumbProps);
		drSwitcher->winThumbProps.rcDestination = *drSwitcher->switchDesktopRECT;
		DwmUpdateThumbnailProperties(drSwitcher->switchDesktopThumbnail, &drSwitcher->winThumbProps);
		drSwitcher->winThumbProps.rcDestination = *drSwitcher->switchTaskbarRECT;
		DwmUpdateThumbnailProperties(drSwitcher->switchTaskbarThumbnail, &drSwitcher->winThumbProps);

		for (hash_map<RECT*, HTHUMBNAIL>::reverse_iterator itr_current_RECT_HTHUMBNAIL = drSwitcher->current_RECT_HTHUMBNAIL_map.rbegin(); itr_current_RECT_HTHUMBNAIL != drSwitcher->current_RECT_HTHUMBNAIL_map.rend(); itr_current_RECT_HTHUMBNAIL++)
		{
			(*itr_current_RECT_HTHUMBNAIL).first->top += diff_y;
			(*itr_current_RECT_HTHUMBNAIL).first->bottom += diff_y;
			drSwitcher->winThumbProps.rcDestination = *(*itr_current_RECT_HTHUMBNAIL).first;
			DwmUpdateThumbnailProperties((*itr_current_RECT_HTHUMBNAIL).second, &drSwitcher->winThumbProps);
		}

		for (hash_map<RECT*, HTHUMBNAIL>::reverse_iterator itr_switch_RECT_HTHUMBNAIL = drSwitcher->switch_RECT_HTHUMBNAIL_map.rbegin(); itr_switch_RECT_HTHUMBNAIL != drSwitcher->switch_RECT_HTHUMBNAIL_map.rend(); itr_switch_RECT_HTHUMBNAIL++)
		{
			(*itr_switch_RECT_HTHUMBNAIL).first->top += diff_y;
			(*itr_switch_RECT_HTHUMBNAIL).first->bottom += diff_y;
			drSwitcher->winThumbProps.rcDestination = *(*itr_switch_RECT_HTHUMBNAIL).first;
			DwmUpdateThumbnailProperties((*itr_switch_RECT_HTHUMBNAIL).second, &drSwitcher->winThumbProps);
		}
	} 
}*/

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
	drSwitcher->currentTaskbarRECT->left += x;
	drSwitcher->currentTaskbarRECT->right += x;
	drSwitcher->switchTaskbarRECT->left += x;
	drSwitcher->switchTaskbarRECT->right += x;
}

void movingRects_y(int y)
{
	E_DragAndDropSwitcher* drSwitcher = E_DragAndDropSwitcher::getSingleton();
	drSwitcher->movingCRect.top += y;
	drSwitcher->movingCRect.bottom += y;
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
		else if ((GetAsyncKeyState(VK_RBUTTON) && 0x8000) && !(GetAsyncKeyState(VK_LBUTTON) && 0x8000)) //&& !((prev_point.x == point.x) && (prev_point.y == point.y)))
		{
			if (!started)
			{
				started = true;

				startSwitcher();
				
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
				E_Window::setIconInvisible(this->m_hWnd);

				E_Global::getSingleton()->getDesktop(switchIndex)->setAllIconInvisible();

				::SetWindowLongW(hTaskbarWnd, GWL_EXSTYLE, GetWindowLong(hTaskbarWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
				::SetLayeredWindowAttributes(hTaskbarWnd, 0, 0, LWA_ALPHA);
			}
			else
			{
				E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
				E_Global* e_global = E_Global::getSingleton();
				if ((cursor_left || cursor_right) && (abs(point.x - prev_point.x) > 3))
				{
					movingRects_x(point.x - prev_point.x);
					MoveWindow(movingCRect, FALSE);
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
				else if ((cursor_top || cursor_bottom) && (abs(point.y - prev_point.y) > 3))
				{
					movingRects_y(point.y - prev_point.y);
					MoveWindow(movingCRect);
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
		else if (started)
		{
			KillTimer(1);
			E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
			E_Global* e_global = E_Global::getSingleton();
			if (cursor_left)
			{
				if (switchable)
				{
					while (prev_point.x <= enManager->getWidth())
					{
						movingRects_x(2);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += 2;
					}
					Sleep(60);
					while (prev_point.x >= enManager->getWidth()*0.8)
					{
						movingRects_x(-2);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += -2;
					}
					Sleep(60);
					while (prev_point.x <= enManager->getWidth())
					{
						movingRects_x(1);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += 1;
					}
					Sleep(60);
					while (prev_point.x >= enManager->getWidth()*0.9)
					{
						movingRects_x(-1);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += -1;
					}
					Sleep(60);
					while (prev_point.x <= enManager->getWidth())
					{
						movingRects_x(1);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += 1;
					}
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
					::SendMessage(e_global->hwnd_frame, WM_TRAY_EVENT, switchIndex, 0);
				}
				else
				{
					while (prev_point.x >= 0)
					{
						movingRects_x(-2);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += -2;
					}
					Sleep(60);
					while (prev_point.x <= enManager->getWidth()*0.2)
					{
						movingRects_x(2);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += 2;
					}
					Sleep(60);
					while (prev_point.x >= 0)
					{
						movingRects_x(-1);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += -1;
					}
					Sleep(60);
					while (prev_point.x <= enManager->getWidth()*0.1)
					{
						movingRects_x(1);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += 1;
					}
					Sleep(60);
					while (prev_point.x >= 0)
					{
						movingRects_x(-1);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += -1;
					}
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
					}
					Sleep(60);
					while (prev_point.x <= enManager->getWidth()*0.2)
					{
						movingRects_x(2);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += 2;
					}
					Sleep(60);
					while (prev_point.x >= 0)
					{
						movingRects_x(-1);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += -1;
					}
					Sleep(60);
					while (prev_point.x <= enManager->getWidth()*0.1)
					{
						movingRects_x(1);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += 1;
					}
					Sleep(60);
					while (prev_point.x >= 0)
					{
						movingRects_x(-1);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += -1;
					}
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
					::SendMessage(e_global->hwnd_frame, WM_TRAY_EVENT, switchIndex, 0);
				}
				else
				{
					while (prev_point.x <= enManager->getWidth())
					{
						movingRects_x(2);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += 2;
					}
					Sleep(60);
					while (prev_point.x >= enManager->getWidth()*0.8)
					{
						movingRects_x(-2);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += -2;
					}
					Sleep(60);
					while (prev_point.x <= enManager->getWidth())
					{
						movingRects_x(1);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += 1;
					}
					Sleep(60);
					while (prev_point.x >= enManager->getWidth()*0.9)
					{
						movingRects_x(-1);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += -1;
					}
					Sleep(60);
					while (prev_point.x <= enManager->getWidth())
					{
						movingRects_x(1);
						MoveWindow(movingCRect, FALSE);
						prev_point.x += 1;
					}
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
					}
					Sleep(60);
					while (prev_point.y >= enManager->getHeight()*0.8)
					{
						movingRects_y(-2);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += -2;
					}
					Sleep(60);
					while (prev_point.y <= enManager->getHeight())
					{
						movingRects_y(1);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += 1;
					}
					Sleep(60);
					while (prev_point.y >= enManager->getHeight()*0.9)
					{
						movingRects_y(-1);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += -1;
					}
					Sleep(60);
					while (prev_point.y <= enManager->getHeight())
					{
						movingRects_y(1);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += 1;
					}
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
					::SendMessage(e_global->hwnd_frame, WM_TRAY_EVENT, switchIndex, 0);
				}
				else
				{
					while (prev_point.y >= 0)
					{
						movingRects_y(-2);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += -2;
					}
					Sleep(60);
					while (prev_point.y <= enManager->getHeight()*0.2)
					{
						movingRects_y(2);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += 2;						
					}
					Sleep(60);
					while (prev_point.y >= 0)
					{
						movingRects_y(-1);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += -1;
					}
					Sleep(60);
					while (prev_point.y <= enManager->getHeight()*0.1)
					{
						movingRects_y(1);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += 1;
					}
					Sleep(60);
					while (prev_point.y >= 0)
					{
						movingRects_y(-1);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += -1;
					}
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
					}
					Sleep(60);
					while (prev_point.y <= enManager->getHeight()*0.2)
					{
						movingRects_y(2);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += 2;
					}
					Sleep(60);
					while (prev_point.y >= 0)
					{
						movingRects_y(-1);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += -1;
					}
					Sleep(60);
					while (prev_point.y <= enManager->getHeight()*0.1)
					{
						movingRects_y(1);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += 1;
					}
					Sleep(60);
					while (prev_point.y >= 0)
					{
						movingRects_y(-1);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += -1;
					}
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
					::SendMessage(e_global->hwnd_frame, WM_TRAY_EVENT, switchIndex, 0);
				}
				else
				{
					while (prev_point.y <= enManager->getHeight())
					{
						movingRects_y(2);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += 2;
					}
					Sleep(60);
					while (prev_point.y >= enManager->getHeight()*0.8)
					{
						movingRects_y(-2);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += -2;
					}
					Sleep(60);
					while (prev_point.y <= enManager->getHeight())
					{
						movingRects_y(1);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += 1;
					}
					Sleep(60);
					while (prev_point.y >= enManager->getHeight()*0.9)
					{
						movingRects_y(-1);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += -1;
					}
					Sleep(60);
					while (prev_point.y <= enManager->getHeight())
					{
						movingRects_y(1);
						MoveWindow(movingCRect, FALSE);
						prev_point.y += 1;
					}
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

			for (list<HWND>::iterator itr = e_global->dockedWindowList.begin(); itr != e_global->dockedWindowList.end(); itr++)
			{
				WINDOWPLACEMENT windowinfo;
				::GetWindowPlacement(*itr, &windowinfo);
				UINT winstate = windowinfo.showCmd;

				if ((winstate == SW_HIDE) && !(::IsWindowVisible(*itr)) )
					::ShowWindow(*itr, SW_SHOW);
			}
		}
	}

	__super::OnTimer(nIDEvent);
}

void E_DragAndDropSwitcher::OnKillFocus(CWnd* pNewWnd)
{
	terminateSwitcher();

	__super::OnKillFocus(pNewWnd);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
