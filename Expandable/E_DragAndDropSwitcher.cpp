#include "stdafx.h"
#include "E_DragAndDropSwitcher.h"
#include "E_Global.h"
#include "E_EnvironmentManager.h"
#include "E_AeroPeekController.h"

void E_DragAndDropSwitcher::updateSelectedDesktop()
{
	//������Ʈ�� �߻��� ��� �ڵ����� ȣ���
	TRACE_WIN32A("[E_DragAndDropSwitcher::updateSelectedDesktop()]");
}

void drawCurrentDesktop()
{
}

void drawSwitchDesktop()
{
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
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();

	/*if (!ison)
	{
		E_Global* e_global = E_Global::getSingleton();

		CBrush brush_window;
		UINT nClassStyle_window = 0;
		CString szClassName_window = AfxRegisterWndClass(nClassStyle_window, 0, (HBRUSH)CreateSolidBrush(Color::Black), 0);
		if (cursour_left)
		{
			CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_window, L"DragAndDropSwitcher", WS_VISIBLE | WS_POPUP, CRect(-(enManager->getWidth() * 2) + 5, 0, 0 + 5, enManager->getHeight()), CWnd::GetDesktopWindow(), 0);
		}
		else if (cursour_right)
		{
			CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_window, L"DragAndDropSwitcher", WS_VISIBLE | WS_POPUP, CRect(enManager->getWidth() - 5, 0, enManager->getWidth() * 3 - 5, enManager->getHeight()), CWnd::GetDesktopWindow(), 0);
		}
		else if (cursour_top)
		{
			CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_window, L"DragAndDropSwitcher", WS_VISIBLE | WS_POPUP, CRect(0, -(enManager->getHeight() * 2) + 5, enManager->getWidth(), 0 + 5), CWnd::GetDesktopWindow(), 0);
		}
		else if (cursour_bottom)
		{
			CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_window, L"DragAndDropSwitcher", WS_VISIBLE | WS_POPUP, CRect(0, enManager->getHeight() - 5, enManager->getWidth(), enManager->getHeight() * 3 - 5), CWnd::GetDesktopWindow(), 0);
		}
		
		E_Window::setIconInvisible(this->m_hWnd);

		drawDragAndDropSwitcher();
		UpdateWindow();
		ison = true;

		ShowWindow(SW_SHOWMAXIMIZED);

		//e_global->startUpdate();
	}
	else
	{
		terminateSwitcher();
	}*/
}

void E_DragAndDropSwitcher::terminateSwitcher()
{
	E_Global* e_global = E_Global::getSingleton();
//	eraseWindowAnimation();

	ison = false;

	//e_global->stopUpdate();

	E_Window::setIconVisible(this->m_hWnd);
	DestroyWindow();
}

void E_DragAndDropSwitcher::OnResize()
{

}

void E_DragAndDropSwitcher::OnRMouseDown(int nx, int ny)
{

}

void E_DragAndDropSwitcher::OnRMouseMove(int nx, int ny)
{

}

void E_DragAndDropSwitcher::RmouseUp(int nx, int ny)
{

}
BEGIN_MESSAGE_MAP(E_DragAndDropSwitcher, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()


void E_DragAndDropSwitcher::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CWnd::OnPaint()��(��) ȣ������ ���ʽÿ�.
}

int E_DragAndDropSwitcher::getNextDesktop()
{
	return nextDesktopIndex;
}