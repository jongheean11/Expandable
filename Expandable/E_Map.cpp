#include "stdafx.h"
#include "E_Window.h"
#include "E_Map.h"
#include "E_Global.h"

#define WM_TRAY_EVENT (WM_USER + 3)
const COLORREF E_Map::backgroundColor = RGB(0, 0, 0);
void E_Map::updateSelectedDesktop()
{
	//������Ʈ�� �߻��� ��� �ڵ����� ȣ���
	if (ison){
		//Invalidate(0);
		//this->Invalidate(0);
		//Invalidate(0);
		//TRACE_WIN32A("[E_Map::updateSelectedDesktop()]");
	}
}
E_Map* E_Map::singleton = NULL;
E_Map::E_Map()
{
	E_Global* e_global = E_Global::getSingleton();
	ison2 = false;
	maphwnd = this->GetSafeHwnd();
	//leave2 = false;
	leave = false;
	up = false;
	select = false;
	forSelectMap = false;
	hwnd = this->GetSafeHwnd();

	iconMoveMode = 0;
	hwnd_cwnd_emap = this;
	ison = false;
	redraw = false;
	clicked = false;

}
E_Map::~E_Map()
{
	if (hwnd_cwnd_emap != NULL)
	{
		hwnd_cwnd_emap->DestroyWindow();
		hwnd_cwnd_emap = NULL;
	}
}
CWnd* E_Map::getmapwindow()
{
	return hwnd_cwnd_emap;
}
const wchar_t* E_Map::caption = L"Map";
void E_Map::drawMap()
{
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_Global* e_global = E_Global::getSingleton();
	time = e_global->getTimer();
	e_global->mapopen = true;
	//leave2 = false;
	ison2 = true;
	//e_global->onUpdate();
	//e_global->startUpdate();
	if (!ison)
	{
		//ison = true;
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
		hwnd_cwnd_emap->CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_map, E_Map::caption, WS_VISIBLE | WS_POPUP, CRect(0, 0, mapunit*mapWidth, mapunit*mapHeight), CWnd::GetDesktopWindow(), 0);
		//
		e_global->hwnd_cwnd = hwnd_cwnd_emap;
		//
		SetTimer(1, 1000, NULL);
		hwnd_cwnd_emap->ShowWindow(SW_SHOW);
		::SetWindowLongW(hwnd_cwnd_emap->m_hWnd, GWL_EXSTYLE, GetWindowLong(hwnd_cwnd_emap->m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		::SetLayeredWindowAttributes(hwnd_cwnd_emap->m_hWnd, 0, e_global->getTransparent(), LWA_ALPHA); //â����
		hwnd_cwnd_emap->UpdateWindow();
		//hwnd_cwnd_emap->SetWindowPos(NULL, w*0.85, (h - th)*0.75, w*0.15, (h - th)*0.25, SWP_NOZORDER | SWP_SHOWWINDOW);
		hwnd_cwnd_emap->SetWindowPos(NULL, w - mapunit*mapWidth, (h - th) - mapunit*mapHeight, mapunit*mapWidth, mapunit*mapHeight, SWP_NOZORDER | SWP_SHOWWINDOW);


		E_Window::setIconInvisible(hwnd_cwnd_emap->m_hWnd);
		//cwnd_map->UpdateWindow();
	}
	else
		terminateMap();
	

}

void E_Map::terminateMap()
{
	E_Global* e_global = E_Global::getSingleton();
	//e_global->stopUpdate();
	iconRectList.clear();
	iconHwndList.clear();
	hwnd_cwnd_emap->DestroyWindow();
	ison = false;
	ison2 = false;
}


BEGIN_MESSAGE_MAP(E_Map, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_USER_EVENT, OnUserEvent)
	ON_WM_MOUSELEAVE()
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
END_MESSAGE_MAP()



void E_Map::OnPaint()
{
	//::SendMessage(this->GetSafeHwnd(), WM_LBUTTONUP, 0, 0);
	//e_global->onUpdate();
	CRect tmprect;
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
	double mapsize = e_global->getMapsize();

	if (forSelectMap)
	{

		drawable = true;
		int indexx = iconClick.x / (mapsize*w) + 1;
		int indexy = iconClick.y / (mapsize*w) + 1;

		select = true;
		//cbrush brush;
		//brush.createsolidbrush(rgb(255, 170, 85));    


		//memdc.rectangle(indexx*mapsize*w, indexy*mapsize*w, indexx*mapsize*w + mapsize*w, indexy*mapsize*w+mapsize*w);
		tmprect.left = (indexx - 1)*mapsize*w;
		tmprect.top = (indexy - 1)*mapsize*w;
		tmprect.right = tmprect.left + mapsize*w;
		tmprect.bottom = tmprect.top + mapsize*w;

		//forSelectMap = false;
	}

	if (!ison || clicked || forSelectMap || up || e_global->gethotkey())
	{
		e_global->sethotkey(false);
		drawable = true;
		forSelectMap = false;
		iconRectList.clear();
		iconHwndList.clear();
		ison = true;
		// device context for painting

		long x1, y1, x2, y2;
		//long tmp1 = (h - th)*0.25 / mapHeight;
		double tmp1 = mapsize* w;
		//long tmp2 = 0.15*w / mapWidth;
		double tmp2 = mapsize * w;
		RECT rectForIcon;
		CPen pen;
		pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		memDC.SelectObject(pen);
		HWND tmphwnd;
		//test ���� ����ȭ���� ���α׷� �ʿ� �׸���
		for (int i = 0; i < mapHeight; i++)
		{
			y1 = i*tmp1;
			y2 = y1 + tmp1;
			for (int j = 0; j < mapWidth; j++)
			{
				x1 = j*tmp2;
				x2 = x1 + tmp2;
				memDC.Rectangle(x1, y1, x2, y2);
				//dc.StretchBlt(0, 0, w, h, &memDC, 0, 0, w, h, SRCCOPY);
			}
		}
		RECT rectForSelectDesktop;
		int nowselect = e_global->getSelectedIndex();
		int selectx = e_global->getSelectedIndex() % mapWidth;
		int selecty = e_global->getSelectedIndex() / mapWidth;
		

		rectForSelectDesktop.left = selectx*w*mapsize;
		rectForSelectDesktop.top = selecty*w*mapsize;
		rectForSelectDesktop.right = rectForSelectDesktop.left + w*mapsize;
		rectForSelectDesktop.bottom = rectForSelectDesktop.top + w*mapsize;

		pen.DeleteObject();
		pen.CreatePen(PS_SOLID, 5, RGB(118, 35, 220));
		memDC.SelectObject(pen);
		memDC.MoveTo(rectForSelectDesktop.left, rectForSelectDesktop.top);
		memDC.LineTo(rectForSelectDesktop.right, rectForSelectDesktop.top);
		memDC.MoveTo(rectForSelectDesktop.right, rectForSelectDesktop.top);
		memDC.LineTo(rectForSelectDesktop.right, rectForSelectDesktop.bottom);
		memDC.MoveTo(rectForSelectDesktop.left, rectForSelectDesktop.top);
		memDC.LineTo(rectForSelectDesktop.left, rectForSelectDesktop.bottom);
		memDC.MoveTo(rectForSelectDesktop.left, rectForSelectDesktop.bottom);
		memDC.LineTo(rectForSelectDesktop.right, rectForSelectDesktop.bottom);
		pen.DeleteObject();
		if (up)
		{
			up = false;
			pen.DeleteObject();
			if (select)
			{
				select = false;
				pen.CreatePen(PS_SOLID, 5, RGB(255, 170, 85));
				memDC.SelectObject(pen);
				memDC.MoveTo(tmprect.left, tmprect.top);
				memDC.LineTo(tmprect.right, tmprect.top);
				memDC.MoveTo(tmprect.right, tmprect.top);
				memDC.LineTo(tmprect.right, tmprect.bottom);
				memDC.MoveTo(tmprect.left, tmprect.top);
				memDC.LineTo(tmprect.left, tmprect.bottom);
				memDC.MoveTo(tmprect.left, tmprect.bottom);
				memDC.LineTo(tmprect.right, tmprect.bottom);
				pen.DeleteObject();
			}
		}

		std::list<E_Desktop*> all_Desktop = e_global->desktopList;
		for (std::list<E_Desktop*>::iterator itr_desktop = all_Desktop.begin(); itr_desktop != all_Desktop.end(); itr_desktop++)	//�� ����ũž �������
		{
			(*itr_desktop)->getIndex();
			int idx = (*itr_desktop)->getIndex() % mapWidth;
			int idy = (*itr_desktop)->getIndex() / mapWidth;
			std::list<E_Window*> desktop_window = (*itr_desktop)->getWindowList();
			for (std::list<E_Window*>::reverse_iterator itr_window = desktop_window.rbegin(); itr_window != desktop_window.rend(); itr_window++)	//������ũž���� �ȿ� �ִ� ������ �ڵ� �����ͼ� ������ ���
			{
				//E_Winodw Ŭ����(*itr_window)�� getIcon()�� �׸����
				//�����ܺ���ġ��?


				tmphwnd = (*itr_window)->getWindow();
				::GetWindowRect(tmphwnd, &rectForIcon);
				long iconPosstx = rectForIcon.left*e_global->getMapsize() + idx*w*mapsize;
				long iconPossty = rectForIcon.top*e_global->getMapsize()*w / (h - th) + idy*w*mapsize;
				//rect ���λ������ �̴ϸ� ũ���� 1/8
				//rect ���� ��ġ�� rectForIcon �� x1,y1

				//rectForIcon.left = iconPosstx1 + rectForIcon.left*0.15; //������ ���ϱ�
				//rectForIcon.top = iconPossty1 + rectForIcon.top * 0.25;// ������ ���ϱ�
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
					{
						if (iconPosstx < iconSize / 2)
							iconPosstx = iconSize / 2;

						if (iconPossty < iconSize / 2)
							iconPossty = iconSize / 2;
						memDC.StretchBlt(iconPosstx + 2 - iconSize / 2, iconPossty + 2 - iconSize / 2, iconSize, iconSize, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, SRCCOPY);

					}
					
							
					RECT* iconRect = new RECT{ iconPosstx + 2 - iconSize / 2, iconPossty + 2 - iconSize / 2, iconPosstx + 2 + iconSize / 2, iconPossty + 2 + iconSize / 2 };

					iconRectList.push_front(iconRect);
					iconHwndList.push_front(tmphwnd);
					cdc.DeleteDC();
				}
			}

		}

		redraw = false;

		//pen.DeleteObject();
		//test ���� ����ȭ���� ���α׷� �ʿ� �׸���
	}

	if (iconMoveMode == 1)// Lbutton down
	{

		drawable = true;
		std::list<RECT*>::iterator itr_rect = iconRectList.begin();
		for (std::list<HWND>::iterator itr_hwnd = iconHwndList.begin(); itr_rect != iconRectList.end(); itr_rect++, itr_hwnd++)	//�� ����ũž �������
		{
			if ((*itr_hwnd) == selectIconHwnd)
			{

				std::list<E_Window*> desktop_window = e_global->getSelectedDesktop()->getWindowList();
				for (std::list<E_Window*>::iterator itr_window = desktop_window.begin(); itr_window != desktop_window.end(); itr_window++)	//������ũž���� �ȿ� �ִ� ������ �ڵ� �����ͼ� ������ ���
				{
					if ((*itr_window)->getWindow() == selectIconHwnd)
						::ShowWindow(selectIconHwnd, SW_NORMAL);
				}
				//leave2 = false;
				::BringWindowToTop(this->GetSafeHwnd());
				::BringWindowToTop(selectIconHwnd);
				memDC.FillRect(*itr_rect, &brush);
				foreRect.left = (*itr_rect)->left;
				foreRect.right = (*itr_rect)->right;
				foreRect.bottom = (*itr_rect)->bottom;
				foreRect.top = (*itr_rect)->top;

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


				memDC.StretchBlt(iconClick.x - iconSize / 2, iconClick.y - iconSize / 2, iconSize, iconSize, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, SRCCOPY);

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

		memDC.StretchBlt(iconClick.x - iconSize / 2, iconClick.y - iconSize / 2, iconSize, iconSize, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, SRCCOPY);
		foreRect.left = iconClick.x - iconSize / 2;
		foreRect.right = iconClick.x + iconSize / 2;
		foreRect.top = iconClick.y - iconSize / 2;
		foreRect.bottom = iconClick.y + iconSize / 2;

		RECT rectForMove;
		long newxpoint = (iconClick.x) / e_global->getMapsize()*mapWidth / mapWidth;
		long newypoint = (h - th)*(iconClick.y) / w / e_global->getMapsize() / mapHeight*mapHeight;
		//if (e_global->getSelectedDesktop()->getIndex == )
		int windowindext;
		std::list<E_Desktop*> all_Desktop = e_global->desktopList;
		for (std::list<E_Desktop*>::iterator itr_desktop = all_Desktop.begin(); itr_desktop != all_Desktop.end(); itr_desktop++)	//�� ����ũž �������
		{
			std::list<E_Window*> desktop_window = (*itr_desktop)->getWindowList();
			for (std::list<E_Window*>::iterator itr_window = desktop_window.begin(); itr_window != desktop_window.end(); itr_window++)	//������ũž���� �ȿ� �ִ� ������ �ڵ� �����ͼ� ������ ���
			{
				if ((*itr_window)->getWindow() == selectIconHwnd)
					windowindext = (*itr_desktop)->getIndex();
			}
		}

		//if ( (newxpoint < w && newypoint < h && e_global->getSelectedIndex() == windowindext) || e_global->getSelectedIndex() == getdesktop(movindexx,movindexy)-1)
		if (e_global->getSelectedIndex() == getdesktop(movindexx, movindexy) - 1)
		{
			::ShowWindow(selectIconHwnd, SW_SHOW);
			::GetWindowRect(selectIconHwnd, &rectForMove);
			::MoveWindow(selectIconHwnd, newxpoint - w*(movindexx - 1), newypoint - (h - th)*(movindexy - 1), rectForMove.right - rectForMove.left, rectForMove.bottom - rectForMove.top, TRUE);

		}
		else
		{
			::ShowWindow(selectIconHwnd, SW_HIDE);
		}

		::BringWindowToTop(selectIconHwnd);

		int indexx = mouse.x / (mapsize*w) + 1;
		int indexy = mouse.y / (mapsize*w) + 1;

		//memdc.rectangle(indexx*mapsize*w, indexy*mapsize*w, indexx*mapsize*w + mapsize*w, indexy*mapsize*w+mapsize*w);
		tmprect.left = (indexx - 1)*mapsize*w;
		tmprect.top = (indexy - 1)*mapsize*w;
		tmprect.right = tmprect.left + mapsize*w;
		tmprect.bottom = tmprect.top + mapsize*w;

		select = false;
		CPen pen;
		pen.CreatePen(PS_SOLID, 5, RGB(255, 170, 85));
		memDC.SelectObject(pen);
		memDC.MoveTo(tmprect.left, tmprect.top);
		memDC.LineTo(tmprect.right, tmprect.top);
		memDC.MoveTo(tmprect.right, tmprect.top);
		memDC.LineTo(tmprect.right, tmprect.bottom);
		memDC.MoveTo(tmprect.left, tmprect.top);
		memDC.LineTo(tmprect.left, tmprect.bottom);
		memDC.MoveTo(tmprect.left, tmprect.bottom);
		memDC.LineTo(tmprect.right, tmprect.bottom);
		pen.DeleteObject();




	}

	if (drawable)
		dc.StretchBlt(0, 0, w, h, &memDC, 0, 0, w, h, SRCCOPY);
	memDC.DeleteDC();
	bmp.DeleteObject();
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
	E_Global* e_global = E_Global::getSingleton();
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	long w = enManager->getWidth();
	double mapsize = e_global->getMapsize();
	//leave2 = false;
	::BringWindowToTop(this->GetSafeHwnd());
	up = true;
	forSelectMap = true;
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	std::list<HWND>::iterator itr_hwnd = iconHwndList.begin();
	for (std::list<RECT*>::iterator itr_rect = iconRectList.begin(); itr_rect != iconRectList.end(); itr_rect++, itr_hwnd++)	//�� ����ũž �������
	{
		if ((*itr_rect)->left < point.x && (*itr_rect)->right > point.x && (*itr_rect)->top < point.y && (*itr_rect)->bottom > point.y)
		{
			clicked = true;
			iconMoveMode = 1;
			iconClick = point;
			selectIconHwnd = (*itr_hwnd);
			//::ShowWindow(selectIconHwnd, SW_NORMAL);
			break;
		}
	}
	clickindexx = mouse.x / (mapsize*w) + 1;
	clickindexy = mouse.y / (mapsize*w) + 1;
	time = e_global->getTimer();
	//e_global->setSelectedIndex(getdesktop(clickindexx, clickindexy) - 1);



	Invalidate(0);
	CWnd::OnLButtonDown(nFlags, point);
}


void E_Map::OnLButtonUp(UINT nFlags, CPoint point)
{
	E_Global* e_global = E_Global::getSingleton();
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	if (leave)
	{
		point.x = mouse.x;
		point.y = mouse.y;
		leave = false;
	}
	//leave2 = false;
	::BringWindowToTop(this->GetSafeHwnd());
	long h = enManager->getHeight();
	long th = enManager->getTaskbarHeight();
	long w = enManager->getWidth();
	long iconSize = w*e_global->getMapsize() / 4 * e_global->getIconsize();
	double mapsize = e_global->getMapsize();
	upindexx = point.x / (mapsize*w) + 1;
	upindexy = point.y / (mapsize*w) + 1;
	up = true;
	bool in = false;
	forSelectMap = false;
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	iconMoveMode = 0;
	clicked = false;
	time = e_global->getTimer();

	RECT trect;
	trect.left = point.x - iconSize / 2;
	trect.top = point.y - iconSize / 2;
	trect.right = trect.left + iconSize;
	trect.bottom = trect.top + iconSize;

	int mapWidth = e_global->getDesktopWidth();
	int mapHeight = e_global->getDesktopHeight();
	int desktop = 0;
	int bre = 0;
	std::list<RECT*>::iterator itr_rect = iconRectList.begin();
	for (std::list<HWND>::iterator itr_hwnd = iconHwndList.begin(); itr_hwnd != iconHwndList.end(); itr_hwnd++, itr_rect++)	//�� ����ũž �������
	{
		if ((*itr_hwnd) == selectIconHwnd)
		{
			RECT trect2;
			int fromdesktop = getdesktop(clickindexx, clickindexy);

			int todesktop = getdesktop(upindexx, upindexy);
			long xp = point.x - (upindexx - 1)*w*mapsize;
			long yp = point.y - (upindexy - 1)*w*mapsize;
			//CWnd *pWnd = CWnd::FromHandle(selectIconHwnd);
			std::list<E_Window*> winlist = e_global->getDesktop(fromdesktop - 1)->getWindowList();//���⼭ selected�� �ƴ϶�
			//�ش� �������� �ִ� ����ũž����
			for (std::list<E_Window*>::iterator itr_wind = winlist.begin(); itr_wind != winlist.end(); itr_wind++)	//�� ����ũž �������
			{
				if ((*itr_wind)->getWindow() == selectIconHwnd)
				{
					e_global->getDesktop(fromdesktop - 1)->excludeWindow((*itr_wind));//���� �ڵ��� ������))
					e_global->getDesktop(todesktop - 1)->insertWindow((*itr_wind));
				}
			}

			::GetWindowRect(selectIconHwnd, &trect2);
			::MoveWindow(selectIconHwnd, xp / mapsize, yp*(h - th) / w / mapsize, trect2.right - trect2.left, trect2.bottom - trect2.top, TRUE);

			in = true;
			iconHwndList.remove((*itr_hwnd));
			iconRectList.remove((*itr_rect));
			iconHwndList.push_front(selectIconHwnd);
			iconRectList.push_front(&trect);
			selectIconHwnd = NULL;

			for (int i = 1; i < mapHeight + 1; i++)
			{
				for (int j = 1; j < mapWidth + 1; j++)
				{
					desktop++;
					if (j == upindexx && i == upindexy)
					{	//���⼭ �����츦 �ش� desktop���� ���� ����
						bre = 1;
						break;
					}
				}
				if (bre)
					break;
			}
			break;
		}
	}
	Invalidate(0);
	CWnd::OnLButtonUp(nFlags, point);
	if (!in && clickindexx == upindexx && clickindexy == upindexy)
	{
		for (int i = 1; i < mapHeight + 1; i++)
		{
			for (int j = 1; j < mapWidth + 1; j++)
			{
				desktop++;
				if (j == upindexx && i == upindexy)
				{
					e_global->setSelectedIndex(desktop - 1);
					::SendMessage(e_global->hwnd_frame, WM_TRAY_EVENT, e_global->getSelectedIndex(), 0);
					//���⼭ �����츦 �ش� desktop���� ���� ����
					bre = 1;
					break;
				}
			}
			if (bre)
				break;
		}


		//selecteddesktop �� �����츸 �����ְ� �������� �����
		std::list<E_Desktop*> desklist = e_global->desktopList;
		for (std::list<E_Desktop*>::iterator itr_desk = desklist.begin(); itr_desk != desklist.end(); itr_desk++)	//�� ����ũž �������
		{
			if ((*itr_desk)->getIndex() == e_global->getSelectedDesktop()->getIndex())
			{
				(*itr_desk)->setAllShow();
				continue;
			}
			(*itr_desk)->setAllHide();
		}
		//leave2 = false;
		::BringWindowToTop(this->GetSafeHwnd());
		//e_global->getSelectedDesktop()->setAllShow(); 


		//terminateMap();
	}
}
int E_Map::getdesktop(int indexx, int indexy)
{
	E_Global* e_global = E_Global::getSingleton();
	int mapWidth = e_global->getDesktopWidth();
	int mapHeight = e_global->getDesktopHeight();
	int desktop = 0;
	int bre = 0;
	for (int i = 1; i < mapHeight+1 ; i++)
	{
		for (int j = 1; j < mapWidth+1 ; j++)
		{
			desktop++;
			if (j == indexx && i == indexy)
			{	//���⼭ �����츦 �ش� desktop���� ���� ����
				bre = 1;
				break;
			}
		}
		if (bre)
			break;
	}
	return desktop;
}

void E_Map::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	SetCursor(LoadCursor(NULL, IDC_ARROW)); // �⺻
	E_Global* e_global = E_Global::getSingleton();
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton(); 
	long w = enManager->getWidth();
	double mapsize = e_global->getMapsize();
	time = e_global->getTimer();
	mouse = iconClick = point;
	movindexx = point.x / (mapsize*w) + 1;
	movindexy = point.y / (mapsize*w) + 1;
	iconMoveMode = 2;
	if (clicked)
		Invalidate(0);
	//leave2 = false;
	TRACKMOUSEEVENT MouseEvent;
	::ZeroMemory(&MouseEvent, sizeof(MouseEvent));
	MouseEvent.cbSize = sizeof(MouseEvent);
	MouseEvent.dwFlags = TME_LEAVE;
	MouseEvent.hwndTrack = m_hWnd;
	MouseEvent.dwHoverTime = 0;

	m_bTrack = ::_TrackMouseEvent(&MouseEvent);
	if (m_bTrack)
	{
		SetWindowText(_T("Tracking"));
	}
	
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
			E_Global* e_global = E_Global::getSingleton();
			KillTimer(1);
			time = e_global->getTimer();
			terminateMap();
		}
	}
	else if (nIDEvent == 10)
	{
		if ((GetAsyncKeyState(VK_LBUTTON) && 0x8000))
		{
			if (GetForegroundWindow() != hwnd_cwnd_emap)
			{
		
				KillTimer(10);
				terminateMap();
			}
		}
	}
	CWnd::OnTimer(nIDEvent);
}

HRESULT E_Map::OnUserEvent(WPARAM wParam, LPARAM lParam)
{
	// TODO: Your Code
	E_Global* e_global = E_Global::getSingleton();
	Invalidate(0);
	time = e_global->getTimer();
	return TRUE;
}

void E_Map::OnMouseLeave()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	OutputDebugString(L"OnMouseLeave star");
		
	TRACKMOUSEEVENT MouseEvent;
	::ZeroMemory(&MouseEvent, sizeof(MouseEvent));
	MouseEvent.cbSize = sizeof(MouseEvent);
	MouseEvent.dwFlags = TME_CANCEL;
	MouseEvent.hwndTrack = m_hWnd;
	::_TrackMouseEvent(&MouseEvent);
	m_bTrack = false;
	//leave2 = true;
	if (clicked)
	{
		leave = true;
		::SendMessage(this->GetSafeHwnd(), WM_LBUTTONUP, 0, 0);
	}
	__super::OnMouseLeave();
}


//void E_Map::OnKillFocus(CWnd* pNewWnd)
//{
//	__super::OnKillFocus(pNewWnd);
//	OutputDebugString(L"kill focus");
//	//if (leave2)
//	//	terminateMap();
//	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
//}


int E_Map::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.

	SetTimer(10, 15, NULL);

	return 0;
}
