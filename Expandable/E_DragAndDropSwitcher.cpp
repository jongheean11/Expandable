#include "stdafx.h"
#include "E_DragAndDropSwitcher.h"
#include <Windows.h> 
#include <WinUser.h>
#include "E_Global.h"
#include "E_EnvironmentManager.h"
#include "E_AeroPeekController.h"

const COLORREF E_DragAndDropSwitcher::backgroundColor = RGB(0x37, 0xb6, 0xeb);

void E_DragAndDropSwitcher::updateSelectedDesktop()
{
	//������Ʈ�� �߻��� ��� �ڵ����� ȣ���
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
	/*if (drSwitcher->cursor_left)
	{
		plus_width = drSwitcher->sizeRect_background.right;
	}
	else if (drSwitcher->cursor_top)
	{
		plus_height = drSwitcher->sizeRect_background.bottom;
	}*/
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

	std::list<E_Window*> window_list = e_global->getSelectedDesktop()->windowList;
	std::list<E_Window*> window_list_topmost;
	for (std::list<E_Window*>::iterator itr_window = window_list.begin(); itr_window != window_list.end(); itr_window++)
	{
		if ((*itr_window)->isAeroPossible())
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
				drSwitcher->current_RECT_HTHUMBNAIL_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(windowRECT, pushThumbnail));

				drSwitcher->current_RECT_list.push_front(windowRECT);
				drSwitcher->current_RECT_EWindow_map.insert(hash_map<RECT*, E_Window*>::value_type(windowRECT, *itr_window));
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
		drSwitcher->current_RECT_HTHUMBNAIL_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(windowRECT, pushThumbnail));

		drSwitcher->current_RECT_list.push_front(windowRECT);
		drSwitcher->current_RECT_EWindow_map.insert(hash_map<RECT*, E_Window*>::value_type(windowRECT, *itr_window));
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
		width = -drSwitcher->sizeRect_background.right;

		drSwitcher->switchDesktopRECT = new RECT
		{
			-(drSwitcher->sizeRect_background.right) + drSwitcher->sizeRect_background.left,
			drSwitcher->sizeRect_background.top,
			-(drSwitcher->sizeRect_background.right) + drSwitcher->sizeRect_background.right,
			drSwitcher->sizeRect_background.bottom
		};
		aeController->registerAero(drSwitcher->hShellWnd, drSwitcher->m_hWnd, *(drSwitcher->switchDesktopRECT), drSwitcher->switchDesktopThumbnail);
		drSwitcher->switchTaskbarRECT = new RECT
		{
			-(drSwitcher->sizeRect_taskbar.right) + drSwitcher->sizeRect_taskbar.left,
			drSwitcher->sizeRect_taskbar.top,
			-(drSwitcher->sizeRect_taskbar.right) + drSwitcher->sizeRect_taskbar.right,
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
		if (!(e_global->getSelectedIndex() == (e_global->getDesktopWidth() - 1)))
		{
			if (e_global->getSelectedIndex() > (e_global->getDesktopWidth() - 1))
			{
				for (int p = 0; p != ((((e_global->getSelectedIndex() + 1) / e_global->getDesktopHeight())) * e_global->getDesktopWidth()); p++)
				{
					itr_desktop++;
					drSwitcher->switchIndex++;
				}
			}
			if ((e_global->getSelectedIndex() % e_global->getDesktopWidth()) != (e_global->getDesktopWidth() - 1))
			{
				itr_desktop++;
				drSwitcher->switchIndex++;
			}
		}
		drSwitcher->switchDesktop = *itr_desktop;
	}
	else if (drSwitcher->cursor_top)
	{
		height = -drSwitcher->sizeRect_background.bottom;

		drSwitcher->switchDesktopRECT = new RECT
		{
			drSwitcher->sizeRect_background.left,
			-(drSwitcher->sizeRect_background.bottom) + drSwitcher->sizeRect_background.top,
			drSwitcher->sizeRect_background.right,
			-(drSwitcher->sizeRect_background.bottom) + drSwitcher->sizeRect_background.bottom
		};
		aeController->registerAero(drSwitcher->hShellWnd, drSwitcher->m_hWnd, *(drSwitcher->switchDesktopRECT), drSwitcher->switchDesktopThumbnail);
		drSwitcher->switchTaskbarRECT = new RECT
		{
			drSwitcher->sizeRect_taskbar.left,
			-(drSwitcher->sizeRect_taskbar.bottom) + drSwitcher->sizeRect_taskbar.top,
			drSwitcher->sizeRect_taskbar.right,
			-(drSwitcher->sizeRect_taskbar.bottom) + drSwitcher->sizeRect_taskbar.bottom
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
	
	std::list<E_Window*> window_list = drSwitcher->switchDesktop->getWindowList();
	std::list<E_Window*> window_list_topmost;
	for (std::list<E_Window*>::iterator itr_window = window_list.begin(); itr_window != window_list.end(); itr_window++)
	{
		(*itr_window)->setShow();
		if (((*itr_window)->isAeroPossible()) && IsWindow((*itr_window)->getWindow()))
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
					getSize.left + width,
					getSize.top + height,
					getSize.right + width,
					getSize.bottom + height
				};
				aeController->registerAero((*itr_window)->getWindow(), drSwitcher->m_hWnd, *windowRECT, pushThumbnail);
				drSwitcher->switch_RECT_HTHUMBNAIL_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(windowRECT, pushThumbnail));

				drSwitcher->switch_RECT_list.push_front(windowRECT);
				drSwitcher->switch_RECT_EWindow_map.insert(hash_map<RECT*, E_Window*>::value_type(windowRECT, *itr_window));
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
		drSwitcher->switch_RECT_HTHUMBNAIL_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(windowRECT, pushThumbnail));

		drSwitcher->switch_RECT_list.push_front(windowRECT);
		drSwitcher->switch_RECT_EWindow_map.insert(hash_map<RECT*, E_Window*>::value_type(windowRECT, *itr_window));
	}
}

void drawDragAndDropSwitcher()
{
	E_Global* e_global = E_Global::getSingleton();
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_DragAndDropSwitcher* drSwitcher = E_DragAndDropSwitcher::getSingleton();
	E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();

	PAINTSTRUCT ps;
	HDC hdc;
	HTHUMBNAIL pushThumbnail;

	drSwitcher->hShellWnd = GetShellWindow(); // ����ȭ�� hwnd
	GetWindowRect(drSwitcher->hShellWnd, &(drSwitcher->sizeRect_background)); // ����ȭ�� ũ�� ���
	drSwitcher->hTaskbarWnd = FindWindowW(_T("Shell_TrayWnd"), NULL); // �۾�ǥ���� hwnd
	GetWindowRect(drSwitcher->hTaskbarWnd, &(drSwitcher->sizeRect_taskbar)); // �۾�ǥ���� ũ�� ���

	drawCurrentDesktop();

	drawSwitchDesktop();

	hdc = BeginPaint(*drSwitcher, &ps);
	EndPaint(*drSwitcher, &ps);
}


E_DragAndDropSwitcher::E_DragAndDropSwitcher()
{
	ison = false;
	cursor_left = false;
	cursor_right = false;
	cursor_top = false;
	cursor_bottom = false;

	winThumbProps.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY;
	// Use the window frame and client area
	winThumbProps.fSourceClientAreaOnly = FALSE;
	winThumbProps.fVisible = TRUE;
	winThumbProps.opacity = (255 * 70) / 100;
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

void E_DragAndDropSwitcher::startSwitcher()
{
	if (!ison)
	{
		E_Global::getSingleton()->onUpdate();
		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		
		UINT nClassStyle_window = 0;
		CString szClassName_window = AfxRegisterWndClass(nClassStyle_window, 0, (HBRUSH)CreateSolidBrush(E_DragAndDropSwitcher::backgroundColor), 0);
		GetCursorPos(&prev_point);
		if (cursor_left)
		{
			main_left = -(enManager->getWidth() + 10);
			main_top = 0;
			main_right = enManager->getWidth();
			main_bottom = enManager->getHeight();
		}
		else if (cursor_right)
		{
			main_left = 0;
			main_top = 0;
			main_right = enManager->getWidth() * 2 + 10;
			main_bottom = enManager->getHeight();
		}
		else if (cursor_top)
		{
			main_left = 0;
			main_top = -(enManager->getHeight() + 10);
			main_right = enManager->getWidth();
			main_bottom = enManager->getHeight();
		}
		else
		{
			main_left = 0;
			main_top = 0;
			main_right = enManager->getWidth();
			main_bottom = enManager->getHeight() * 2 + 10;
		}
		//CreateEx(WS_EX_TOPMOST, szClassName_window, L"DragAndDropSwitcher", WS_VISIBLE | WS_POPUP, CRect(main_left, main_top, main_right, main_bottom), CWnd::GetDesktopWindow(), 0);
		movingCRect = CRect(main_left, main_top, main_right, main_bottom);
		CreateEx(WS_EX_TOPMOST, szClassName_window, L"DragAndDropSwitcher", WS_VISIBLE | WS_POPUP, movingCRect, CWnd::GetDesktopWindow(), 0);
		//CreateEx(WS_EX_TOPMOST, szClassName_window, L"DragAndDropSwitcher", WS_VISIBLE | WS_POPUP, CRect(0, main_top, main_right-main_left, main_bottom), CWnd::GetDesktopWindow(), 0);
		drawDragAndDropSwitcher();
		
		E_Window::setIconInvisible(this->m_hWnd);

		ison = true;

		ShowWindow(SW_SHOWMAXIMIZED);
		
		
	}
	else
	{
		terminateSwitcher();
	}
}

void E_DragAndDropSwitcher::terminateSwitcher()
{
	//E_Global::getSingleton()->stopUpdate();

	ison = false;
	cursor_left = false;
	cursor_right = false;
	cursor_top = false;
	cursor_bottom = false;

	E_AeroPeekController* aeController = E_AeroPeekController::getSingleton();
	
	for (hash_map<RECT*, HTHUMBNAIL>::iterator itr_current = current_RECT_HTHUMBNAIL_map.begin(); itr_current != current_RECT_HTHUMBNAIL_map.end(); itr_current++)
		aeController->unregisterAero(itr_current->second);
	for (hash_map<RECT*, HTHUMBNAIL>::iterator itr_switch = switch_RECT_HTHUMBNAIL_map.begin(); itr_switch != switch_RECT_HTHUMBNAIL_map.end(); itr_switch++)
		aeController->unregisterAero(itr_switch->second);
	aeController->unregisterAero(currentDesktopThumbnail);
	aeController->unregisterAero(switchDesktopThumbnail);

	current_RECT_HTHUMBNAIL_map.clear();
	switch_RECT_HTHUMBNAIL_map.clear();
	current_RECT_list.clear();
	switch_RECT_list.clear();
	current_RECT_EWindow_map.clear();
	switch_RECT_EWindow_map.clear();
	

	E_Window::setIconVisible(this->m_hWnd);
	DestroyWindow();
}

void E_DragAndDropSwitcher::turnUpdateOn()
{
	E_Global::getSingleton()->startUpdate();
}

void E_DragAndDropSwitcher::turnUpdateOff()
{
	E_Global::getSingleton()->stopUpdate();
}

BEGIN_MESSAGE_MAP(E_DragAndDropSwitcher, CWnd)
//	ON_WM_RBUTTONUP()
//	ON_WM_MOUSEMOVE()
//	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


//void E_DragAndDropSwitcher::OnRButtonUp(UINT nFlags, CPoint point)
//{
//	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
//
//	__super::OnRButtonUp(nFlags, point);
//}

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
		drSwitcher->winThumbProps.rcDestination = *drSwitcher->currentDesktopRECT;
		DwmUpdateThumbnailProperties(drSwitcher->currentDesktopThumbnail, &drSwitcher->winThumbProps);
		drSwitcher->winThumbProps.rcDestination = *drSwitcher->currentTaskbarRECT;
		DwmUpdateThumbnailProperties(drSwitcher->currentTaskbarThumbnail, &drSwitcher->winThumbProps);

		list<RECT*> copy_current_RECT_list, copy_switch_RECT_list;
		hash_map<RECT*, HTHUMBNAIL> copy_current_RECT_HTHUMBNAIL_map, copy_switch_RECT_HTHUMBNAIL_map;
		for (list<RECT*>::iterator ritr_current_RECT = drSwitcher->current_RECT_list.begin(); ritr_current_RECT != drSwitcher->current_RECT_list.end(); ritr_current_RECT++)
		{
			RECT *pushRECT = *ritr_current_RECT;
			hash_map<RECT*, HTHUMBNAIL>::iterator itr_current_RECT_HTHUMBNAIL = drSwitcher->current_RECT_HTHUMBNAIL_map.find(*ritr_current_RECT);
			HTHUMBNAIL pushThumbnail = itr_current_RECT_HTHUMBNAIL->second;
			(*pushRECT).left += diff_x;
			(*pushRECT).right += diff_x;

			drSwitcher->winThumbProps.rcDestination = *pushRECT;
			DwmUpdateThumbnailProperties(pushThumbnail, &drSwitcher->winThumbProps);

			copy_current_RECT_list.push_front(pushRECT);
			copy_current_RECT_HTHUMBNAIL_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(pushRECT, pushThumbnail));
		}
		drSwitcher->current_RECT_list = copy_current_RECT_list;
		drSwitcher->current_RECT_HTHUMBNAIL_map = copy_current_RECT_HTHUMBNAIL_map;

		(*drSwitcher->switchDesktopRECT).left += diff_x;
		(*drSwitcher->switchDesktopRECT).right += diff_x;
		(*drSwitcher->switchTaskbarRECT).left += diff_x;
		(*drSwitcher->switchTaskbarRECT).right += diff_x;
		drSwitcher->winThumbProps.rcDestination = *drSwitcher->switchDesktopRECT;
		DwmUpdateThumbnailProperties(drSwitcher->switchDesktopThumbnail, &drSwitcher->winThumbProps);
		drSwitcher->winThumbProps.rcDestination = *drSwitcher->switchTaskbarRECT;
		DwmUpdateThumbnailProperties(drSwitcher->switchTaskbarThumbnail, &drSwitcher->winThumbProps);

		for (list<RECT*>::reverse_iterator ritr_switch_RECT = drSwitcher->switch_RECT_list.rbegin(); ritr_switch_RECT != drSwitcher->switch_RECT_list.rend(); ritr_switch_RECT++)
		{
			RECT *pushRECT = *ritr_switch_RECT;
			hash_map<RECT*, HTHUMBNAIL>::iterator itr_switch_RECT_HTHUMBNAIL = drSwitcher->switch_RECT_HTHUMBNAIL_map.find(*ritr_switch_RECT);
			HTHUMBNAIL pushThumbnail = itr_switch_RECT_HTHUMBNAIL->second;
			(*pushRECT).left += diff_x;
			(*pushRECT).right += diff_x;

			drSwitcher->winThumbProps.rcDestination = *pushRECT;
			DwmUpdateThumbnailProperties(pushThumbnail, &drSwitcher->winThumbProps);

			copy_switch_RECT_list.push_front(pushRECT);
			copy_switch_RECT_HTHUMBNAIL_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(pushRECT, pushThumbnail));
		}
		drSwitcher->switch_RECT_list = copy_switch_RECT_list;
		drSwitcher->switch_RECT_HTHUMBNAIL_map = copy_switch_RECT_HTHUMBNAIL_map;
	}
	else
	{
		(*drSwitcher->currentDesktopRECT).top += diff_y;
		(*drSwitcher->currentDesktopRECT).bottom += diff_y;
		(*drSwitcher->currentTaskbarRECT).top += diff_y;
		(*drSwitcher->currentTaskbarRECT).bottom += diff_y;
		drSwitcher->winThumbProps.rcDestination = *drSwitcher->currentDesktopRECT;
		DwmUpdateThumbnailProperties(drSwitcher->currentDesktopThumbnail, &drSwitcher->winThumbProps);
		drSwitcher->winThumbProps.rcDestination = *drSwitcher->currentTaskbarRECT;
		DwmUpdateThumbnailProperties(drSwitcher->currentTaskbarThumbnail, &drSwitcher->winThumbProps);

		list<RECT*> copy_current_RECT_list, copy_switch_RECT_list;
		hash_map<RECT*, HTHUMBNAIL> copy_current_RECT_HTHUMBNAIL_map, copy_switch_RECT_HTHUMBNAIL_map;
		for (list<RECT*>::reverse_iterator ritr_current_RECT = drSwitcher->current_RECT_list.rbegin(); ritr_current_RECT != drSwitcher->current_RECT_list.rend(); ritr_current_RECT++)
		{
			RECT *pushRECT = *ritr_current_RECT;
			hash_map<RECT*, HTHUMBNAIL>::iterator itr_current_RECT_HTHUMBNAIL = drSwitcher->current_RECT_HTHUMBNAIL_map.find(*ritr_current_RECT);
			HTHUMBNAIL pushThumbnail = itr_current_RECT_HTHUMBNAIL->second;
			(*pushRECT).top += diff_y;
			(*pushRECT).bottom += diff_y;

			drSwitcher->winThumbProps.rcDestination = *pushRECT;
			DwmUpdateThumbnailProperties(pushThumbnail, &drSwitcher->winThumbProps);

			copy_current_RECT_list.push_front(pushRECT);
			copy_current_RECT_HTHUMBNAIL_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(pushRECT, pushThumbnail));
		}
		drSwitcher->current_RECT_list = copy_current_RECT_list;
		drSwitcher->current_RECT_HTHUMBNAIL_map = copy_current_RECT_HTHUMBNAIL_map;

		(*drSwitcher->switchDesktopRECT).top += diff_y;
		(*drSwitcher->switchDesktopRECT).bottom += diff_y;
		(*drSwitcher->switchTaskbarRECT).top += diff_y;
		(*drSwitcher->switchTaskbarRECT).bottom += diff_y;
		drSwitcher->winThumbProps.rcDestination = *drSwitcher->switchDesktopRECT;
		DwmUpdateThumbnailProperties(drSwitcher->switchDesktopThumbnail, &drSwitcher->winThumbProps);
		drSwitcher->winThumbProps.rcDestination = *drSwitcher->switchTaskbarRECT;
		DwmUpdateThumbnailProperties(drSwitcher->switchTaskbarThumbnail, &drSwitcher->winThumbProps);

		for (list<RECT*>::reverse_iterator ritr_switch_RECT = drSwitcher->switch_RECT_list.rbegin(); ritr_switch_RECT != drSwitcher->switch_RECT_list.rend(); ritr_switch_RECT++)
		{
			RECT *pushRECT = *ritr_switch_RECT;
			hash_map<RECT*, HTHUMBNAIL>::iterator itr_switch_RECT_HTHUMBNAIL = drSwitcher->switch_RECT_HTHUMBNAIL_map.find(*ritr_switch_RECT);
			HTHUMBNAIL pushThumbnail = itr_switch_RECT_HTHUMBNAIL->second;
			(*pushRECT).top += diff_y;
			(*pushRECT).bottom += diff_y;

			drSwitcher->winThumbProps.rcDestination = *pushRECT;
			DwmUpdateThumbnailProperties(pushThumbnail, &drSwitcher->winThumbProps);

			copy_switch_RECT_list.push_front(pushRECT);
			copy_switch_RECT_HTHUMBNAIL_map.insert(hash_map<RECT*, HTHUMBNAIL>::value_type(pushRECT, pushThumbnail));
		}
		drSwitcher->switch_RECT_list = copy_switch_RECT_list;
		drSwitcher->switch_RECT_HTHUMBNAIL_map = copy_switch_RECT_HTHUMBNAIL_map;
	}
}

//void E_DragAndDropSwitcher::OnMouseMove(UINT nFlags, CPoint point)
//{
//	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
//	//if (nFlags & MK_RBUTTON)
//	//{
//	/*
//		LONG diff_x = point.x - prev_point.x,
//			diff_y = point.y - prev_point.y;
//		
//		invalidateSwitcher(diff_x, diff_y);
//		prev_point = point;
//		*/
//		/*if ((cursor_left) || (cursor_right))
//		{
//			//main_left += diff_x;
//			//main_right += diff_x;
//			//this->SetWindowPos(&wndTopMost, main_left + diff_x, main_top, main_right + diff_x, main_bottom, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
//			movingCRect.left += diff_x;
//			movingCRect.right += diff_x;
//			this->MoveWindow(movingCRect, TRUE);
//		}
//		else
//		{
//			if (abs(diff_y) < 5)
//				return;
//			//main_top += diff_y;
//			//main_bottom += diff_y;
//			//this->SetWindowPos(&wndTopMost, main_left, main_top + diff_y, main_right, main_bottom + diff_y, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
//			movingCRect.top += diff_y;
//			movingCRect.bottom += diff_y;
//			this->MoveWindow(movingCRect, TRUE);
//		}*/
//		
//		UpdateWindow();
//		//Invalidate();
//		//InvalidateRect(CRect(0, 0, enManager->getWidth(), enManager->getHeight()), true);
//	//}
//	__super::OnMouseMove(nFlags, point);
//}


//void E_DragAndDropSwitcher::OnRButtonDown(UINT nFlags, CPoint point)
//{
//	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
//
//	__super::OnRButtonDown(nFlags, point);
//}

void stealFocus()
{
	DWORD dwCurrentThread = GetCurrentThreadId();
	DWORD dwFGThread = GetWindowThreadProcessId(GetForegroundWindow(), NULL);

	AttachThreadInput(dwCurrentThread, dwFGThread, TRUE);

	HWND hwnd = E_DragAndDropSwitcher::getSingleton()->m_hWnd;
	// Possible actions you may wan to bring the window into focus.
	SetForegroundWindow(hwnd);
	SetCapture(hwnd);
	SetFocus(hwnd);
	SetActiveWindow(hwnd);
	EnableWindow(hwnd, TRUE);

	AttachThreadInput(dwCurrentThread, dwFGThread, FALSE);
}


int E_DragAndDropSwitcher::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
	/*BringWindowToTop();
	GetFocus();
	
	UpdateWindow();
	SetCapture();*/
	//SetFocus();
	
	stealFocus();
	BringWindowToTop();
	UpdateWindow();
	SetTimer(1, 12, NULL);

	return 0;
}


void E_DragAndDropSwitcher::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (nIDEvent == 1)
	{
		if ((GetAsyncKeyState(VK_RBUTTON) && 0x8000) && !(GetAsyncKeyState(VK_LBUTTON) && 0x8000))
		{
			POINT point;
			GetCursorPos(&point);
			invalidateSwitcher(point.x - prev_point.x, point.y - prev_point.y);
			prev_point = point;
		}
		else
		{
			KillTimer(1);
			E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
			E_Global* e_global = E_Global::getSingleton();
			if (cursor_left)
			{
				prev_point.x = currentDesktopRECT->left;
				if (prev_point.x * 100 / enManager->getWidth() >= 33)
				{
					while (prev_point.x <= enManager->getWidth())
					{
						invalidateSwitcher(2, 0);
						prev_point.x += 2;
					}
					e_global->getSelectedDesktop()->setAllHide();
					e_global->setSelectedIndex(switchIndex);
				}
				else
				{
					while (prev_point.x >= 0)
					{
						invalidateSwitcher(-2, 0);
						prev_point.x -= 2;
					}
					switchDesktop->setAllHide();
				}
				
				Sleep(200);
				terminateSwitcher();
			}
			else if (cursor_right)
			{
				prev_point.x = currentDesktopRECT->right;
				if ((enManager->getWidth() - prev_point.x) * 100 / enManager->getWidth() >= 33)
				{
					while (prev_point.x >= 0)
					{
						invalidateSwitcher(-2, 0);
						prev_point.x -= 2;
					}
					e_global->getSelectedDesktop()->setAllHide();
					e_global->setSelectedIndex(switchIndex);
				}
				else
				{
					while (prev_point.x <= enManager->getWidth())
					{
						invalidateSwitcher(2, 0);
						prev_point.x += 2;
					}
					switchDesktop->setAllHide();
				}

				Sleep(200);
				terminateSwitcher();
			}
			else if (cursor_top)
			{
				prev_point.y = currentDesktopRECT->top;
				if (prev_point.y * 100 / enManager->getHeight() >= 33)
				{
					while (prev_point.y <= enManager->getHeight())
					{
						invalidateSwitcher(0, 2);
						prev_point.y += 2;
					}
					e_global->getSelectedDesktop()->setAllHide();
					e_global->setSelectedIndex(switchIndex);
				}
				else
				{
					while (prev_point.y >= 0)
					{
						invalidateSwitcher(0, -2);
						prev_point.y -= 2;
					}
					switchDesktop->setAllHide();
				}

				Sleep(200);
				terminateSwitcher();
			}
			else
			{
				prev_point.y = currentDesktopRECT->bottom;
				if ((enManager->getHeight() - prev_point.y) * 100 / enManager->getHeight() >= 33)
				{
					while (prev_point.y >= 0)
					{
						invalidateSwitcher(0, -2);
						prev_point.y -= 2;
					}
					
					e_global->getSelectedDesktop()->setAllHide();
					e_global->setSelectedIndex(switchIndex);
				}
				else
				{
					while (prev_point.y <= enManager->getHeight())
					{
						invalidateSwitcher(0, 2);
						prev_point.y += 2;
					}
					switchDesktop->setAllHide();
				}

				Sleep(200);
				terminateSwitcher();
			}
		}
	}

	__super::OnTimer(nIDEvent);
}
