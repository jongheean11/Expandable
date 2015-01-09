#include "stdafx.h"
#include "E_Window.h"
#include "E_Map.h"
#include "E_Global.h"

E_Map::E_Map()
{
	hwnd_cwnd_emap = NULL;
	hwnd_cwnd_emap = new CWnd;
	ison = false;
	transparent = 128;
}
E_Map::~E_Map()
{
	if (hwnd_cwnd_emap != NULL)
	{
		hwnd_cwnd_emap->DestroyWindow();
		hwnd_cwnd_emap = NULL;
	}
}

void E_Map::drawMap()
{
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	CBrush m_oBkgndBrush;
	m_oBkgndBrush.CreateSolidBrush(RGB(255, 255, 255));
	//m_oBkgndBrush.CreateStockObject(NULL_BRUSH);

	UINT nClassStyle = CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	CString szClassName = AfxRegisterWndClass(nClassStyle, 0, (HBRUSH)m_oBkgndBrush.GetSafeHandle(), 0);
	if (!ison)
	{
		hwnd_cwnd_emap->Create(szClassName, _T("map"), WS_SIZEBOX  , CRect(enManager->getWidth()*0.85, enManager->getHeight()*0.75, enManager->getWidth(), enManager->getHeight()), CWnd::GetDesktopWindow(), 1235);
		//SetClassLong(hwnd_cwnd->)
		// nID : ID of the Window -> 고려안된점 : 해당 ID가 affordable한지 체크 안 되 있음.
		hwnd_cwnd_emap->ShowWindow(SW_SHOWMAXIMIZED);
		::SetWindowLongW(hwnd_cwnd_emap->m_hWnd, GWL_EXSTYLE, GetWindowLong(hwnd_cwnd_emap->m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		::SetLayeredWindowAttributes(hwnd_cwnd_emap->m_hWnd, 0, transparent, LWA_ALPHA); //창투명
		hwnd_cwnd_emap->UpdateWindow();
		hwnd_cwnd_emap->SetWindowPos(NULL, enManager->getWidth()*0.85, (enManager->getHeight() - enManager->getTaskbarHeight())*0.75, enManager->getWidth()*0.15, (enManager->getHeight() - enManager->getTaskbarHeight())*0.25, SWP_NOZORDER | SWP_SHOWWINDOW );
		E_Window::setIconInvisible(hwnd_cwnd_emap->m_hWnd);
		ison = true;
	}
	else
		terminateMap();
}

void E_Map::onMouseLButtonDown(int x, int y)
{

}

void E_Map::onMouseMove()
{

}

void E_Map::OnMouseLButtonUp(int x, int y)
{

}

void E_Map::startTimer()
{
	
	
}

void E_Map::stopTimer()
{
	

}

void E_Map::terminateMap()
{
	hwnd_cwnd_emap->DestroyWindow();
	ison = false;
}

void E_Map::setTransparent(int value)
{
	transparent = value;
}