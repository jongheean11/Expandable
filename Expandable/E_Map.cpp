#include "stdafx.h"
#include "E_Window.h"
#include "E_Map.h"
#include "E_Global.h"

const COLORREF E_Map::backgroundColor = RGB(0, 0,0);
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
const wchar_t* E_Map::caption = L"Map";
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
		UINT nClassStyle_map = 0;// CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		brush_map.CreateSolidBrush(RGB(255, 255, 255));
		brush_map.CreateStockObject(NULL_BRUSH);
		CString szClassName_map = AfxRegisterWndClass(nClassStyle_map, 0, (HBRUSH)brush_map.GetSafeHandle(), 0);
		//hwnd_cwnd_emap->Create(szClassName_map, _T("map"), WS_SIZEBOX, CRect(enManager->getWidth()*0.85, enManager->getHeight()*0.75, enManager->getWidth(), enManager->getHeight()), CWnd::GetDesktopWindow(), 1235);
		hwnd_cwnd_emap->CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_map, E_Map::caption, WS_VISIBLE | WS_POPUP , CRect(0, 0, w*0.15, (h - th)*0.25), CWnd::GetDesktopWindow(), 0);

		hwnd_cwnd_emap->ShowWindow(SW_SHOW);
		::SetWindowLongW(hwnd_cwnd_emap->m_hWnd, GWL_EXSTYLE, GetWindowLong(hwnd_cwnd_emap->m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		::SetLayeredWindowAttributes(hwnd_cwnd_emap->m_hWnd, 0, transparent, LWA_ALPHA); //â����
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

		//hwnd_cwnd_emap �̴ϸ� �ڵ�
		//SetClassLong(hwnd_cwnd->)
		// nID : ID of the Window -> ����ȵ��� : �ش� ID�� affordable���� üũ �� �� ����.
		//hwnd_cwnd_emap->ShowWindow(SW_SHOWMAXIMIZED);
		
		
		
		int mapWidth = e_global->getDesktopWidth();
		int mapHeight = e_global->getDesktopHeight();
		int desktopCount = e_global->getDesktopCount();
		long x1, y1, x2, y2;
		long tmp1 = (h - th)*0.25 / mapHeight;
		long tmp2 = 0.15*w / mapWidth;
		
		RECT rectForIcon;
	
		HWND tmphwnd;
		long iconSize = 0.15*w / 6 / mapWidth;
		
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		//test ���� ����ȭ���� ���α׷� �ʿ� �׸���
		for (int i = 0; i < mapHeight; i++)
		{
			y1 =  i*tmp1;
			y2 = y1 + tmp1;
			for (int j = 0; j < mapWidth; j++)
			{
				x1 = j*tmp2;
				x2 = x1 + tmp2;
				dc.Rectangle(x1, y1, x2, y2);
			}
		}
		std::list<E_Desktop*> all_Desktop = e_global->desktopList;
		for (std::list<E_Desktop*>::iterator itr_desktop = all_Desktop.begin(); itr_desktop != all_Desktop.end(); itr_desktop++)	//�� ����ũž �������
		{
			std::list<E_Window*> desktop_window = (*itr_desktop)->windowList;
			for (std::list<E_Window*>::iterator itr_window = desktop_window.begin(); itr_window != desktop_window.end(); itr_window++)	//������ũž���� �ȿ� �ִ� ������ �ڵ� �����ͼ� ������ ���
			{
				//E_Winodw Ŭ����(*itr_window)�� getIcon()�� �׸����
				//�����ܺ���ġ��?
				tmphwnd = (*itr_window)->getWindow();
				::GetWindowRect(tmphwnd, &rectForIcon);
				long iconPosstx = rectForIcon.left*0.15 / mapWidth;
				long icpnPossty = rectForIcon.top*0.25 / mapHeight;
				//rect ���λ������ �̴ϸ� ũ���� 1/8
				//rect ���� ��ġ�� rectForIcon �� x1,y1
				//rectForIcon.left = iconPosstx1 + rectForIcon.left*0.15; //������ ���ϱ�
				//rectForIcon.top = icpnPossty1 + rectForIcon.top * 0.25;// ������ ���ϱ�
				//rectForIcon.right = rectForIcon.left + iconSize;	//x2
				//rectForIcon.bottom = rectForIcon.top + iconSize;	//y2
				//���� ����ũž 2 3 4 5 ... �� ���ؼ��� �������� ���ϰ� 1���� �׸�
				CBitmap* icon = (*itr_window)->getIcon();
				BITMAP icon_info;
				if (icon->m_hObject != NULL)
				{
					icon->GetBitmap(&icon_info);
					CDC cdc;
					cdc.CreateCompatibleDC(this->GetWindowDC());
					cdc.SelectObject((*itr_window)->getIcon());
					cdc.SetBkMode(1);
					cdc.SetBkColor(E_Map::backgroundColor);
					dc.SetStretchBltMode(COLORONCOLOR);
					//memDC.StretchBlt(rectForIcon.left, rectForIcon.top, iconSize, iconSize, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, SRCCOPY);
					dc.StretchBlt(iconPosstx+1, icpnPossty+1, iconSize, iconSize, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, SRCCOPY);

					cdc.DeleteDC();
				}
			}
			
		}
		//test ���� ����ȭ���� ���α׷� �ʿ� �׸���
	}


	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CWnd::OnPaint()��(��) ȣ������ ���ʽÿ�.
}

E_Map* E_Map::getSingleton()
{
	if (singleton == NULL)
		singleton = new E_Map();
	return singleton;
}