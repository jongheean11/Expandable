#include "stdafx.h"
#include "E_Window.h"
#include "E_Map.h"
#include "E_Global.h"

E_Map* E_Map::singleton = NULL;
E_Map::E_Map()
{

	hwnd_cwnd_emap = this;
	ison = false;
	transparent = 160;
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
	if (!ison)
	{
		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		E_Global* e_global = E_Global::getSingleton();

		long w = enManager->getWidth();
		long h = enManager->getHeight();
		long th = enManager->getTaskbarHeight();
		
		
		
		CBrush brush_map;
		UINT nClassStyle_map = CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		brush_map.CreateSolidBrush(RGB(255, 255, 255));
		brush_map.CreateStockObject(NULL_BRUSH);
		CString szClassName_map = AfxRegisterWndClass(nClassStyle_map, 0, (HBRUSH)brush_map.GetSafeHandle(), 0);
		hwnd_cwnd_emap->Create(szClassName_map, _T("map"), WS_SIZEBOX, CRect(enManager->getWidth()*0.85, enManager->getHeight()*0.75, enManager->getWidth(), enManager->getHeight()), CWnd::GetDesktopWindow(), 1235);

		hwnd_cwnd_emap->ShowWindow(SW_SHOWMAXIMIZED);
		::SetWindowLongW(hwnd_cwnd_emap->m_hWnd, GWL_EXSTYLE, GetWindowLong(hwnd_cwnd_emap->m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		::SetLayeredWindowAttributes(hwnd_cwnd_emap->m_hWnd, 0, transparent, LWA_ALPHA); //창투명
		hwnd_cwnd_emap->UpdateWindow();
		hwnd_cwnd_emap->SetWindowPos(NULL, w*0.85, (h - th)*0.75, w*0.15, (h - th)*0.25, SWP_NOZORDER | SWP_SHOWWINDOW);

		
		E_Window::setIconInvisible(hwnd_cwnd_emap->m_hWnd);
		//cwnd_map->UpdateWindow();
	}
	else
		terminateMap();
	//this->ShowWindow(SW_SHOWMAXIMIZED);
	//Invalidate(0);

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
BEGIN_MESSAGE_MAP(E_Map, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



void E_Map::OnPaint()
{
	if (!ison)
	{
		ison = true;
		CPaintDC dc(this); // device context for painting


		E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		E_Global* e_global = E_Global::getSingleton();

		long w = enManager->getWidth();
		long h = enManager->getHeight();
		long th = enManager->getTaskbarHeight();

		//hwnd_cwnd_emap 미니맵 핸들
		//SetClassLong(hwnd_cwnd->)
		// nID : ID of the Window -> 고려안된점 : 해당 ID가 affordable한지 체크 안 되 있음.
		//hwnd_cwnd_emap->ShowWindow(SW_SHOWMAXIMIZED);
		
		
		
		int mapWidth = e_global->getDesktopWidth();
		int mapHeight = e_global->getDesktopHeight();
		int desktopCount = e_global->getDesktopCount();
		long x1, y1, x2, y2;
		long tmp1 = (h - th)*0.25 / mapHeight;
		long tmp2 = 0.15*w / mapWidth;
		RECT rectForIcon;
		HWND tmphwnd;
		long iconSize = 0.15*w / 8;
		//test 현재 바탕화면의 프로그램 맵에 그리기
		for (int i = 0; i < mapHeight; i++)
		{
			y1 = (h - th)*0.75 + i*tmp1;
			y2 = y1 + tmp1;
			for (int j = 0; j < mapWidth; j++)
			{
				x1 = w*0.85 + j*tmp2;
				x2 = x1 + tmp2;
				dc.Rectangle(x1, y1, x2, y2);
			}
		}
		std::list<E_Desktop*> all_Desktop = e_global->desktopList;
		for (std::list<E_Desktop*>::iterator itr_desktop = all_Desktop.begin(); itr_desktop != all_Desktop.end(); itr_desktop++)	//각 데스크탑 별로출력
		{
			std::list<E_Window*> desktop_window = (*itr_desktop)->windowList;
			for (std::list<E_Window*>::iterator itr_window = desktop_window.begin(); itr_window != desktop_window.end(); itr_window++)	//각데스크탑별로 안에 있는 윈도우 핸들 가져와서 아이콘 출력
			{
				//E_Winodw 클래스(*itr_window)의 getIcon()을 그리면됨
				//아이콘별위치는?
				tmphwnd = (*itr_window)->getWindow();
				::GetWindowRect(tmphwnd, &rectForIcon);
				//rect 가로사이즈는 미니맵 크기의 1/8




			}
			
		}

		//test 현재 바탕화면의 프로그램 맵에 그리기

	}


	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.
}

E_Map* E_Map::getSingleton()
{
	if (singleton == NULL)
		singleton = new E_Map();
	return singleton;
}