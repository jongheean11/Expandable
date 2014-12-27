#include "stdafx.h"
#include "E_Window.h"
#include "E_Map.h"
#include "E_Global.h"
E_Map::E_Map()
{
	hwnd_cwnd_emap = NULL;
	hwnd_cwnd_emap = new CWnd;
	ison = false;
}
E_Map::~E_Map()
{
	hwnd_cwnd_emap->CloseWindow();
	hwnd_cwnd_emap->DestroyWindow();
	hwnd_cwnd_emap = NULL;
}

void E_Map::drawMap()
{
	CBrush m_oBkgndBrush;
	m_oBkgndBrush.CreateSolidBrush(RGB(255, 255, 255));
	UINT nClassStyle = CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	CString szClassName = AfxRegisterWndClass(nClassStyle, 0, (HBRUSH)m_oBkgndBrush.GetSafeHandle(), 0);
	if (!ison)
	{
		hwnd_cwnd_emap->Create(szClassName , _T(""), WS_VISIBLE, CRect(300, 300, 800, 800), CWnd::GetDesktopWindow(), 1235);
		//SetClassLong(hwnd_cwnd->)
		// nID : ID of the Window -> 고려안된점 : 해당 ID가 affordable한지 체크 안 되 있음.
		hwnd_cwnd_emap->ShowWindow(SW_SHOW);
		hwnd_cwnd_emap->UpdateWindow();
		E_Window::setIconInvisible(hwnd_cwnd_emap->m_hWnd);
		ison = true;
	}
	else
	{
		 
	}
}

void E_Map::onMouseLButtonDown()
{

}

void E_Map::onMouseMove()
{

}

void E_Map::OnMouseLButtonUp()
{

}

void E_Map::startTimer()
{
	sTime = clock();
	SetTimer(NULL, 0, 500, NULL);
}

void E_Map::stopTimer()
{
	nTime = clock();
	if (0)//마우스가 미니맵내부
	{
		sTime = clock();

	}
	else //외부
	{
		nTime = clock();
		if (nTime - sTime > visibleTime)
		{
			KillTimer(NULL, 0);
			E_Window::setIconVisible(hwnd_cwnd_emap->m_hWnd);
			hwnd_cwnd_emap->CloseWindow();
			hwnd_cwnd_emap->DestroyWindow();
			ison = false;
		}
	}

}
