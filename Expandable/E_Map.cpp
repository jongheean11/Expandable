#include "stdafx.h"
#include "E_Window.h"
#include "E_Map.h"
#include "E_Global.h"

const COLORREF E_Map::backgroundColor = RGB(0, 0,0);
void E_Map::updateSelectedDesktop()
{
	//������Ʈ�� �߻��� ��� �ڵ����� ȣ���
	//Invalidate(0);
	TRACE_WIN32A("[E_Map::updateSelectedDesktop()]");
}
E_Map* E_Map::singleton = NULL;
E_Map::E_Map()
{
	forSelectMap = false;
	settingTimer = 5;
	time = settingTimer;
	iconMoveMode = 0;
	hwnd_cwnd_emap = this;
	ison = false;
	redraw = false;
	clicked = false;
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
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_Global* e_global = E_Global::getSingleton();
	//e_global->onUpdate();
	e_global->startUpdate();
	if (!ison)
	{
		int mapWidth = e_global->getDesktopWidth();
		int mapHeight = e_global->getDesktopHeight();
		
		long w = enManager->getWidth();
		long h = enManager->getHeight();
		long th = enManager->getTaskbarHeight();
		
		double mapunit = w*e_global->getMapsize();
		
		
		CBrush brush_map;
		UINT nClassStyle_map = 0;// CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		brush_map.CreateSolidBrush(RGB(255, 255, 255));
		brush_map.CreateStockObject(NULL_BRUSH);
		CString szClassName_map = AfxRegisterWndClass(nClassStyle_map, 0, (HBRUSH)brush_map.GetSafeHandle(), 0);
		//hwnd_cwnd_emap->Create(szClassName_map, _T("map"), WS_SIZEBOX, CRect(enManager->getWidth()*0.85, enManager->getHeight()*0.75, enManager->getWidth(), enManager->getHeight()), CWnd::GetDesktopWindow(), 1235);
		//hwnd_cwnd_emap->CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_map, E_Map::caption, WS_VISIBLE | WS_POPUP , CRect(0, 0, w*0.15, (h - th)*0.25), CWnd::GetDesktopWindow(), 0);
		hwnd_cwnd_emap->CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_map, E_Map::caption, WS_VISIBLE | WS_POPUP, CRect(0, 0, mapunit*mapWidth, mapunit*mapHeight), CWnd::GetDesktopWindow(), 0);
		SetTimer(1, 1000, NULL);
		hwnd_cwnd_emap->ShowWindow(SW_SHOW);
		::SetWindowLongW(hwnd_cwnd_emap->m_hWnd, GWL_EXSTYLE, GetWindowLong(hwnd_cwnd_emap->m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		::SetLayeredWindowAttributes(hwnd_cwnd_emap->m_hWnd, 0, transparent, LWA_ALPHA); //â����
		hwnd_cwnd_emap->UpdateWindow();
		//hwnd_cwnd_emap->SetWindowPos(NULL, w*0.85, (h - th)*0.75, w*0.15, (h - th)*0.25, SWP_NOZORDER | SWP_SHOWWINDOW);
		hwnd_cwnd_emap->SetWindowPos(NULL, w - mapunit*mapWidth, (h - th) - mapunit*mapHeight, mapunit*mapWidth, mapunit*mapHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

		
		E_Window::setIconInvisible(hwnd_cwnd_emap->m_hWnd);
		//cwnd_map->UpdateWindow();
	}
	else
		terminateMap();
	//this->ShowWindow(SW_SHOWMAXIMIZED);
	//Invalidate(0);

}



void E_Map::terminateMap()
{
	E_Global* e_global = E_Global::getSingleton();
	e_global->stopUpdate();
	iconRectList.clear();
	iconHwndList.clear();
	hwnd_cwnd_emap->DestroyWindow();
	ison = false;
}

void E_Map::setTransparent(int value)
{
	transparent = value;
}
BEGIN_MESSAGE_MAP(E_Map, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()



void E_Map::OnPaint()
{
	//e_global->onUpdate();
	bool drawable = false;
	CPaintDC dc(this);
	CDC memDC;
	CBitmap bmp;
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_Global* e_global = E_Global::getSingleton();
	//e_global->onUpdate();
	long w = enManager->getWidth();
	long h = enManager->getHeight();
	long th = enManager->getTaskbarHeight();
	memDC.CreateCompatibleDC(&dc);
	bmp.CreateCompatibleBitmap(&dc, w, h);
	memDC.SelectObject(bmp);
	int mapWidth = e_global->getDesktopWidth();
	int mapHeight = e_global->getDesktopHeight();
	int desktopCount = e_global->getDesktopCount();
	//long iconSize = e_global->getMapsize()*mapWidth*w / mapWidth * e_global->getMapsize() *3 ;
	long iconSize = w*e_global->getMapsize() / 4 * e_global->getIconsize();
	CBrush brush;
	brush.CreateSolidBrush(RGB(255, 255, 255));   // Blue brush.

	if (!ison || clicked )
	{
		drawable = true;
		iconRectList.clear();
		iconHwndList.clear();
		ison = true;
		 // device context for painting
	
		long x1, y1, x2, y2;
		//long tmp1 = (h - th)*0.25 / mapHeight;
		double tmp1 = e_global->getMapsize()* w;
		//long tmp2 = 0.15*w / mapWidth;
		double tmp2 = e_global->getMapsize() * w;
		RECT rectForIcon;
	
		HWND tmphwnd;
			
		//test ���� ����ȭ���� ���α׷� �ʿ� �׸���
		for (int i = 0; i < mapHeight; i++)
		{
			y1 =  i*tmp1;
			y2 = y1 + tmp1;
			for (int j = 0; j < mapWidth; j++)
			{
				x1 = j*tmp2;
				x2 = x1 + tmp2;
				memDC.Rectangle(x1, y1, x2, y2);
			}
		}
		std::list<E_Desktop*> all_Desktop = e_global->desktopList;
		for (std::list<E_Desktop*>::iterator itr_desktop = all_Desktop.begin(); itr_desktop != all_Desktop.end(); itr_desktop++)	//�� ����ũž �������
		{
			std::list<E_Window*> desktop_window = (*itr_desktop)->getWindowList();
			for (std::list<E_Window*>::iterator itr_window = desktop_window.begin(); itr_window != desktop_window.end(); itr_window++)	//������ũž���� �ȿ� �ִ� ������ �ڵ� �����ͼ� ������ ���
			{
				//E_Winodw Ŭ����(*itr_window)�� getIcon()�� �׸����
				//�����ܺ���ġ��?
				
				tmphwnd = (*itr_window)->getWindow();
				::GetWindowRect(tmphwnd, &rectForIcon);
				long iconPosstx = rectForIcon.left*e_global->getMapsize();
				long icpnPossty = rectForIcon.top*e_global->getMapsize();
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
					memDC.SetStretchBltMode(COLORONCOLOR);
					//memDC.StretchBlt(rectForIcon.left, rectForIcon.top, iconSize, iconSize, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, SRCCOPY);
					if (selectIconHwnd != (*itr_window)->getWindow())
						memDC.StretchBlt(iconPosstx + 1, icpnPossty + 2, iconSize, iconSize, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, SRCCOPY);
					RECT *iconRect = new RECT{ iconPosstx + 1, icpnPossty + 2, iconPosstx + 3 + iconSize, icpnPossty + 3 + iconSize };
					iconRectList.push_front(iconRect);
					iconHwndList.push_front(tmphwnd);
					cdc.DeleteDC();
				}
			}
			
		}
		redraw = false;
		//test ���� ����ȭ���� ���α׷� �ʿ� �׸���
	}
	if (forSelectMap)
	{
		int indexx = iconClick.x / e_global->getMapsize()*w;
	}
	if (iconMoveMode==1)// Lbutton down
	{
		
		drawable = true;
		std::list<RECT*>::iterator itr_rect = iconRectList.begin();
		for (std::list<HWND>::iterator itr_hwnd = iconHwndList.begin(); itr_rect != iconRectList.end(); itr_rect++, itr_hwnd++)	//�� ����ũž �������
		{
			if ((*itr_hwnd) == selectIconHwnd)
			{
				::BringWindowToTop(selectIconHwnd);
				memDC.FillRect(*itr_rect, &brush);
				foreRect.left = (*itr_rect)->left;
				foreRect.right = (*itr_rect)->right;
				foreRect.bottom = (*itr_rect)->bottom;
				foreRect.top = (*itr_rect)->top;

				CBitmap icon ;
				int width = E_Util::getSystemSmallIconSize();
				HICON hicon = E_Util::getIconHandle(selectIconHwnd);
				HBITMAP bitmap = E_Util::ConvertIconToBitmap(hicon, width, width);
				icon.Attach(bitmap);
				
				BITMAP icon_info;
				icon.GetBitmap(&icon_info);
				memDC.SetStretchBltMode(COLORONCOLOR);
				CDC cdc;
				cdc.CreateCompatibleDC(this->GetWindowDC());
				cdc.SelectObject(icon);
				cdc.SetBkMode(1);
				cdc.SetBkColor(E_Map::backgroundColor);


				memDC.StretchBlt(iconClick.x, iconClick.y, iconSize, iconSize, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, SRCCOPY);
				
				break;

			}
		}
	}
	if (clicked && iconMoveMode == 2)
	{
		
		drawable = true;
		memDC.FillRect(&foreRect, &brush);//������ �����
		CBitmap icon;
		int width = E_Util::getSystemSmallIconSize();
		HICON hicon = E_Util::getIconHandle(selectIconHwnd);
		HBITMAP bitmap = E_Util::ConvertIconToBitmap(hicon, width, width);
		icon.Attach(bitmap);

		BITMAP icon_info;
		icon.GetBitmap(&icon_info);
		memDC.SetStretchBltMode(COLORONCOLOR);
		CDC cdc;
		cdc.CreateCompatibleDC(this->GetWindowDC());
		cdc.SelectObject(icon);
		cdc.SetBkMode(1);
		cdc.SetBkColor(E_Map::backgroundColor);

		memDC.StretchBlt(iconClick.x, iconClick.y, iconSize, iconSize, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, SRCCOPY);
		foreRect.left = iconClick.x ;
		foreRect.right = iconClick.x + iconSize;
		foreRect.top = iconClick.y ;
		foreRect.bottom = iconClick.y + iconSize ;
		
		RECT rectForMove;
		long newxpoint = iconClick.x / e_global->getMapsize()*mapWidth / mapWidth;
		long newypoint = (h - th)*iconClick.y / w / e_global->getMapsize() / mapHeight*mapHeight;
		if (newxpoint < w && newypoint < h)
		{
			::ShowWindow(selectIconHwnd, SW_SHOW);
			::GetWindowRect(selectIconHwnd, &rectForMove);
			::MoveWindow(selectIconHwnd, newxpoint, newypoint, rectForMove.right - rectForMove.left, rectForMove.bottom - rectForMove.top, TRUE);
		}
		else
		{
			::ShowWindow(selectIconHwnd, SW_HIDE);
		}
		
		
		//CWnd* pChild = GetWindow(GW_CHILD);
		//long xc = iconClick.x * 100 / 15 * mapWidth;
		//long yc = iconClick.y * 100 / 25 * mapHeight;
	
		//while (pChild)
	//	{
		//	::MoveWindow(pChild->m_hWnd, xc+rectForMove.right-rectForMove.left, yc+rectForMove.bottom - rectForMove.top, rectForMove.right - rectForMove.left, rectForMove.bottom - rectForMove.top, TRUE);
		//	pChild = pChild->GetNextWindow();
		//	::GetWindowRect(pChild->m_hWnd, &rectForMove);
		//}
		
		
		
		::BringWindowToTop(selectIconHwnd);

		//SetWindowPos(CWnd::FromHandle(selectIconHwnd),rectForMove.left,rectForMove.top, rectForMove.right -rectForMove.left, rectForMove.bottom - rectForMove.top, SWP_NOSIZE | SWP_SHOWWINDOW);
		/*CDC cdc;
		cdc.CreateCompatibleDC(this->GetWindowDC());
		CBitmap icon;
		int width = E_Util::getSystemSmallIconSize();
		HICON hicon = E_Util::getIconHandle(selectIconHwnd);
		HBITMAP bitmap = E_Util::ConvertIconToBitmap(hicon, width, width);
		icon.Attach(bitmap);
		BITMAP icon_info;
		icon.GetBitmap(&icon_info);
		dc.StretchBlt(iconClick.x, iconClick.y, iconSize, iconSize, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, SRCCOPY);
*/

	}
	if (drawable)
		dc.StretchBlt(0, 0, w, h, &memDC, 0, 0, w, h, SRCCOPY);
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CWnd::OnPaint()��(��) ȣ������ ���ʽÿ�.
}

E_Map* E_Map::getSingleton()
{
	if (singleton == NULL)
		singleton = new E_Map();
	return singleton;
}

void E_Map::OnLButtonDown(UINT nFlags, CPoint point)
{
	forSelectMap = true;
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	std::list<HWND>::iterator itr_hwnd = iconHwndList.begin();
	for (std::list<RECT*>::iterator itr_rect = iconRectList.begin(); itr_rect != iconRectList.end(); itr_rect++,itr_hwnd++)	//�� ����ũž �������
	{
		if ((*itr_rect)->left < point.x && (*itr_rect)->right > point.x && (*itr_rect)->top < point.y && (*itr_rect)->bottom > point.y)
		{
			clicked = true;
			iconMoveMode = 1;
			iconClick = point;
			selectIconHwnd = (*itr_hwnd);
			break;
		}
	}
	time = settingTimer;
	Invalidate(0);
	CWnd::OnLButtonDown(nFlags, point);
}


void E_Map::OnLButtonUp(UINT nFlags, CPoint point)
{
	forSelectMap = false;
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	iconMoveMode = 0;
	clicked = false;
	time = settingTimer;
	std::list<RECT*>::iterator itr_rect = iconRectList.begin();
	for (std::list<HWND>::iterator itr_hwnd = iconHwndList.begin(); itr_hwnd != iconHwndList.end(); itr_hwnd++, itr_rect++)	//�� ����ũž �������
	{
		if ((*itr_hwnd) == selectIconHwnd)
		{
			iconHwndList.remove((*itr_hwnd));
			iconRectList.remove((*itr_rect));
			iconHwndList.push_front(selectIconHwnd);
			iconRectList.push_front(&foreRect);
			selectIconHwnd = NULL;
			break;
		}
	}
	Invalidate(0);
	CWnd::OnLButtonUp(nFlags, point);
}


void E_Map::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	time = settingTimer;
	mouse = iconClick = point;
	iconMoveMode = 2;
	Invalidate(0);
	CWnd::OnMouseMove(nFlags, point);
}


void E_Map::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (nIDEvent == 1)
	{
		time--;
		if (time < 0)
		{
			KillTimer(1);
			//terminateMap();
			time = settingTimer;
			terminateMap();
		}
	}
	CWnd::OnTimer(nIDEvent);
}

void E_Map::setTimer(int value)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	settingTimer = value;
}
