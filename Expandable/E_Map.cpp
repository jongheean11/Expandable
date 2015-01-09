#include "stdafx.h"
#include "E_Window.h"
#include "E_Map.h"
#include "E_Global.h"

E_Map::E_Map()
{
	hwnd_cwnd_emap = NULL;
	hwnd_cwnd_emap = new CWnd;
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
	//CPaintDC dc(this);
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_Global* e_global = E_Global::getSingleton();
	CBrush m_oBkgndBrush;
	m_oBkgndBrush.CreateSolidBrush(RGB(255, 255, 255));
	m_oBkgndBrush.CreateStockObject(NULL_BRUSH);
	long w = enManager->getWidth();
	long h = enManager->getHeight();
	long th = enManager->getTaskbarHeight();

	UINT nClassStyle = CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	CString szClassName = AfxRegisterWndClass(nClassStyle, 0, (HBRUSH)m_oBkgndBrush.GetSafeHandle(), 0);
	if (!ison)
	{
		ison = true;	//hwnd_cwnd_emap 미니맵 핸들
		hwnd_cwnd_emap->Create(szClassName, _T("map"), WS_SIZEBOX | WS_EX_TOPMOST, CRect(enManager->getWidth()*0.85, enManager->getHeight()*0.75, enManager->getWidth(), enManager->getHeight()), CWnd::GetDesktopWindow(), 1235);
		//SetClassLong(hwnd_cwnd->)
		// nID : ID of the Window -> 고려안된점 : 해당 ID가 affordable한지 체크 안 되 있음.
		hwnd_cwnd_emap->ShowWindow(SW_SHOWMAXIMIZED);
		::SetWindowLongW(hwnd_cwnd_emap->m_hWnd, GWL_EXSTYLE, GetWindowLong(hwnd_cwnd_emap->m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED );
		::SetLayeredWindowAttributes(hwnd_cwnd_emap->m_hWnd, 0, transparent, LWA_ALPHA); //창투명
		hwnd_cwnd_emap->UpdateWindow();
		hwnd_cwnd_emap->SetWindowPos(NULL, w*0.85, (h - th)*0.75,w*0.15, (h - th)*0.25, SWP_NOZORDER | SWP_SHOWWINDOW);
	
		E_Window::setIconInvisible(hwnd_cwnd_emap->m_hWnd);
		int mapWidth = e_global->desktopwidth;
		int mapHeight = e_global->desktopheight;
		int desktopCount = e_global->getDesktopCount();
		long x1, y1, x2, y2;
		
		//test 현재 바탕화면의 프로그램 맵에 그리기
		for (int i = 0; i < mapHeight; i++)
		{
			y1 = (h - th)*0.75 + i*(h - th)*0.25 / mapHeight;
			y2 = y1 + (h - th)*0.25 / mapHeight;
			for (int j = 0; j < mapWidth; j++)
			{
				x1 = w*0.85 + i*0.15*w / mapWidth;
				x2 = x1 + 0.15*w / mapWidth;
				
				RECT rect = { x1, y1, x2, y2 };
				CDC cdc;
				CPen* p;
				cdc.CreateCompatibleDC(this->GetWindowDC());
				cdc.SelectObject(p);
			//	dc.SetStretchBltMode(COLORONCOLOR);


				
			}

		}





		std::list<E_Desktop*> all_Desktop = e_global->desktopList;
		for (std::list<E_Desktop*>::iterator itr_desktop = all_Desktop.begin(); itr_desktop != all_Desktop.end(); itr_desktop++)
		{
			




		}



		//test 현재 바탕화면의 프로그램 맵에 그리기








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