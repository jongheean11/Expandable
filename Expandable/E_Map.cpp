#include "stdafx.h"
#include "E_Window.h"
#include "E_Map.h"
#include "E_Global.h"
#include <stdint.h>
#include "E_AeroPeekController.h"
#define WM_TRAY_EVENT (WM_USER + 3)
#define WM_USER_MAPR (WM_USER + 5)
#define WM_INVALIDATE (WM_USER + 6)
const COLORREF E_Map::backgroundColor = RGB(0, 0, 0);
void E_Map::updateSelectedDesktop()
{
	//업데이트가 발생한 경우 자동으로 호출됨
	//if (ison){
	//Invalidate(0);
	//this->Invalidate(0);
	//	Invalidate(0);
	//TRACE_WIN32A("[E_Map::updateSelectedDesktop()]");
	//}
}
E_Map* E_Map::singleton = NULL;
E_Map::E_Map()
{
	E_Global* e_global = E_Global::getSingleton();
	checkdelete = false;
	ison2 = false;
	maphwnd = this->GetSafeHwnd();
	//leave2 = false;
	leave = false;
	up = false;
	select = false;
	forSelectMap = false;
	hwnd = this->GetSafeHwnd();
	clickedforerror = false;
	iconMoveMode = 0;
	hwnd_cwnd_emap = this;
	ison = false;
	redraw = false;
	clicked = false;
	alreadyin = false;
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


	std::list<HWND> docklist = e_global->dockedWindowList;
	for (std::list<HWND>::iterator itr_dock = docklist.begin(); itr_dock != docklist.end(); itr_dock++)
	{
		if (!IsWindow(*itr_dock))
		{
			e_global->dockedWindowList.remove((*itr_dock));
			e_global->dockcount--;
		}
	}

	time = e_global->getTimer();
	e_global->mapopen = true;
	ison2 = true;
	TRACE_WIN32A("[E_Map::drawMap] onUpdate() BEFORE");
	e_global->onUpdate();
	TRACE_WIN32A("[E_Map::drawMap] onUpdate() AFTER");
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


		//CBrush brush_map;
		UINT nClassStyle_map = 0;// CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		//brush_map.CreateSolidBrush(RGB(255, 255, 255));
		//brush_map.CreateStockObject(NULL_BRUSH);
		CString szClassName_map = AfxRegisterWndClass(nClassStyle_map, 0, NULL, 0);
		hwnd_cwnd_emap->CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_map, E_Map::caption, WS_VISIBLE | WS_POPUP, CRect(0, 0, mapunit*mapWidth, mapunit*mapHeight), CWnd::GetDesktopWindow(), 0);
		//
		e_global->hwnd_cwnd = hwnd_cwnd_emap;
		//
		SetTimer(1, 500, NULL);
		hwnd_cwnd_emap->ShowWindow(SW_SHOW);
		if (E_AeroPeekController::getSingleton()->isAeroPeekMode())
			::SetWindowLongW(hwnd_cwnd_emap->m_hWnd, GWL_EXSTYLE, GetWindowLong(hwnd_cwnd_emap->m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		::SetLayeredWindowAttributes(hwnd_cwnd_emap->m_hWnd, 0, e_global->getTransparent(), LWA_ALPHA); //창투명
		hwnd_cwnd_emap->UpdateWindow();
		//hwnd_cwnd_emap->SetWindowPos(NULL, w*0.85, (h - th)*0.75, w*0.15, (h - th)*0.25, SWP_NOZORDER | SWP_SHOWWINDOW);
		hwnd_cwnd_emap->SetWindowPos(NULL, w - mapunit*mapWidth, (h - th) - mapunit*mapHeight, mapunit*mapWidth, mapunit*mapHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

		GetWindowRect(getSize);
		E_Window::setIconInvisible(hwnd_cwnd_emap->m_hWnd);
		//cwnd_map->UpdateWindow();
		//brush_map.DeleteObject();
	}
	else
		terminateMap();


}

void E_Map::terminateMap()
{
	E_Global* e_global = E_Global::getSingleton();
	//e_global->stopUpdate();
	//::SendMessage(this->GetSafeHwnd(), WM_LBUTTONUP, 0, 0);
	std::list<RECT*> Rectlist2 = E_Map::getSingleton()->iconRectList;
	for (std::list<RECT*>::iterator itr_rect = Rectlist2.begin(); itr_rect != Rectlist2.end(); itr_rect++)	//각 데스크탑 별로출력
	{
		if ((*itr_rect))
			delete *itr_rect;
		*itr_rect = NULL;
	}


	e_global->mapopen = false;
	iconRectList.clear();
	iconHwndList.clear();
	
	
	ison = false;
	ison2 = false;

	if (clickedforerror)
	{
		//클릭하고 타임아웃시 처리
		int stopdesktop = getdesktop(clickindexx, clickindexy); //stopdesktop-1 이 해당 윈도우가 있던 위치
		RECT rectForerror;
		int foredesktop = E_Global::getSingleton()->getSelectedIndex();

		if (stopdesktop != foredesktop)
			::ShowWindow(selectIconHwnd, SW_SHOW);
		//E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
		//E_Global* e_global = E_Global::getSingleton();
		//long w = enManager->getWidth();
		//long h = enManager->getHeight();
		//long th = enManager->getTaskbarHeight();
		//int mapWidth = e_global->getDesktopWidth();
		//int mapHeight = e_global->getDesktopHeight();
		//double mapsize = e_global->getMapsize();
		//long iconSize = w*e_global->getMapsize() / 4 * e_global->getIconsize();

		//int idx = (stopdesktop-1) % mapWidth;
		//int idy = (stopdesktop - 1) / mapWidth;
		//::GetWindowRect(selectIconHwnd, &rectForerror);
		//long iconPosstx = rectForerror.left *e_global->getMapsize() + idx*w*mapsize;  //check
		//long iconPossty = rectForerror.top *e_global->getMapsize()*w / (h - th) + idy*w*mapsize;//check

	

		//selectIconHwnd = NULL;

	}
	hwnd_cwnd_emap->DestroyWindow();
}


BEGIN_MESSAGE_MAP(E_Map, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_USER_EVENT, OnUserEvent)
	ON_MESSAGE(WM_INVALIDATE, OnInvali)
	ON_WM_MOUSELEAVE()
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()



void E_Map::OnPaint()
{
	//::SendMessage(this->GetSafeHwnd(), WM_LBUTTONUP, 0, 0);
	//e_global->onUpdate();
	CRect tmprect;
	bool drawable = false;
	CPaintDC dc(this);
	CDC memDC;
	CDC* pDCM = GetDC();
	CBitmap bmp;
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_Global* e_global = E_Global::getSingleton();



	
	
	//cdc.CreateCompatibleDC(pDC);
	//e_global->onUpdate();
	long w = enManager->getWidth();
	long h = enManager->getHeight();
	long th = enManager->getTaskbarHeight();
	memDC.CreateCompatibleDC(pDCM);
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

	if (!ison || clicked || forSelectMap || up || e_global->gethotkey() || checkdelete)
	{
		e_global->sethotkey(false);
		drawable = true;
		forSelectMap = false;


		
		std::list<RECT*> Rectlist2 = E_Map::getSingleton()->iconRectList;
		for (std::list<RECT*>::iterator itr_rect = Rectlist2.begin(); itr_rect != Rectlist2.end(); itr_rect++)	//각 데스크탑 별로출력
		{
			if ((*itr_rect))
				delete *itr_rect;
			*itr_rect = NULL;
		}


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
		CPen* oldpen;
		pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		oldpen = memDC.SelectObject(&pen);
		HWND tmphwnd;
		//test 현재 바탕화면의 프로그램 맵에 그리기
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

		int isOneWindow;
		std::list<E_Desktop*> all_Desktop = e_global->desktopList;
		for (std::list<E_Desktop*>::iterator itr_desktop = all_Desktop.begin(); itr_desktop != all_Desktop.end(); itr_desktop++)	//각 데스크탑 별로출력
		{
			if ((*itr_desktop)->getWindowList().size() == 1 && e_global->dockcount == 0)
				isOneWindow = 1;
			else
				isOneWindow = 0;

			(*itr_desktop)->getIndex();
			int idx = (*itr_desktop)->getIndex() % mapWidth;
			int idy = (*itr_desktop)->getIndex() / mapWidth;
			std::list<E_Window*> desktop_window = (*itr_desktop)->getWindowList();
			for (std::list<E_Window*>::iterator itr_window = desktop_window.begin(); itr_window != desktop_window.end(); itr_window++)	//각데스크탑별로 안에 있는 윈도우 핸들 가져와서 아이콘 출력
			{
				//E_Winodw 클래스(*itr_window)의 getIcon()을 그리면됨
				//아이콘별위치는?
				if (!IsWindow((*itr_window)->getWindow()))
					continue;
				tmphwnd = (*itr_window)->getWindow();
				::GetWindowRect(tmphwnd, &rectForIcon);
				long iconPosstx = rectForIcon.left *e_global->getMapsize() + idx*w*mapsize;  //check
				long iconPossty = rectForIcon.top *e_global->getMapsize()*w / (h - th) + idy*w*mapsize;//check
				//rect 가로사이즈는 미니맵 크기의 1/8
				//rect 시작 위치는 rectForIcon 의 x1,y1

				//rectForIcon.left = iconPosstx1 + rectForIcon.left*0.15; //비율값 더하기
				//rectForIcon.top = iconPossty1 + rectForIcon.top * 0.25;// 비율값 더하기
				//rectForIcon.right = rectForIcon.left + iconSize;	//x2
				//rectForIcon.bottom = rectForIcon.top + iconSize;	//y2
				//아직 데스크탑 2 3 4 5 ... 에 대해서는 관여하지 못하고 1번만 그림
				CBitmap* icon = (*itr_window)->getIcon();
				BITMAP icon_info;
				int dr = 0;
				if (icon->m_hObject != NULL)
				{
					icon->GetBitmap(&icon_info);

					CDC cdc;
					CDC* pDC = GetDC();
					cdc.CreateCompatibleDC(pDC);
					cdc.SelectObject((*itr_window)->getIcon());
					cdc.SetBkMode(1);
					cdc.SetBkColor(E_Map::backgroundColor);
					memDC.SetStretchBltMode(COLORONCOLOR);
					//memDC.StretchBlt(rectForIcon.left, rectForIcon.top, iconSize, iconSize, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, SRCCOPY);
					RECT* iconRect;
					if (selectIconHwnd != (*itr_window)->getWindow())
					{
						if (isOneWindow)
							memDC.TransparentBlt(idx*w*mapsize + w*mapsize*0.15, idy*w*mapsize + w*mapsize*0.15, w*mapsize*0.7, w*mapsize*0.7, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, RGB(0, 0, 0));// SRCCOPY);
						else
							memDC.TransparentBlt(iconPosstx + 2, iconPossty + 2, iconSize, iconSize, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, RGB(0, 0, 0));//SRCCOPY);
					}

					if (isOneWindow)
					{
						dr = 1;
						iconRect = new RECT{ idx*w*mapsize + w*mapsize*0.15, idy*w*mapsize + w*mapsize*0.15, idx*w*mapsize + w*mapsize*0.85, idy*w*mapsize + w*mapsize*0.85 };
					}
					else
						iconRect = new RECT{ iconPosstx + 2, iconPossty + 2, iconPosstx + 2 + iconSize, iconPossty + 2 + iconSize };

					if (dr && !clicked)
					{
						memDC.SelectObject(oldpen);
						pen.DeleteObject();
						pen.CreatePen(PS_SOLID, 3, RGB(65, 205, 25));	//초록
						oldpen = memDC.SelectObject(&pen);
						memDC.MoveTo(iconRect->left, iconRect->top);
						memDC.LineTo(iconRect->right, iconRect->top);
						memDC.MoveTo(iconRect->right, iconRect->top);
						memDC.LineTo(iconRect->right, iconRect->bottom);
						memDC.MoveTo(iconRect->left, iconRect->top);
						memDC.LineTo(iconRect->left, iconRect->bottom);
						memDC.MoveTo(iconRect->left, iconRect->bottom);
						memDC.LineTo(iconRect->right, iconRect->bottom);
						memDC.SelectObject(oldpen);
						pen.DeleteObject();
						 
					}
					
					
					if ((*itr_window)->dock)//고정윈도우의 경우
					{
						memDC.SelectObject(oldpen);
						pen.CreatePen(PS_SOLID, 3, RGB(240, 40, 40));	//빨강
						oldpen = memDC.SelectObject(&pen);
						memDC.MoveTo(iconRect->left, iconRect->top);
						memDC.LineTo(iconRect->right, iconRect->top);
						memDC.MoveTo(iconRect->right, iconRect->top);
						memDC.LineTo(iconRect->right, iconRect->bottom);
						memDC.MoveTo(iconRect->left, iconRect->top);
						memDC.LineTo(iconRect->left, iconRect->bottom);
						memDC.MoveTo(iconRect->left, iconRect->bottom);
						memDC.LineTo(iconRect->right, iconRect->bottom);
						memDC.SelectObject(oldpen);
						pen.DeleteObject();

						
						for (int jdx = 0; jdx < mapWidth; jdx++)
						{
							for (int jdy = 0; jdy < mapHeight; jdy++)
							{
								if (jdx == idx && jdy == idy)
									continue;
								
								iconPosstx = rectForIcon.left *e_global->getMapsize() + jdx*w*mapsize;  //check
								iconPossty = rectForIcon.top *e_global->getMapsize()*w / (h - th) + jdy*w*mapsize;//check
								memDC.SelectObject(oldpen);
								pen.CreatePen(PS_SOLID, 2, RGB(160, 160, 160));	//노랑
								memDC.MoveTo(iconPosstx + 2, iconPossty + 2);
								oldpen = memDC.SelectObject(&pen);
								memDC.LineTo(iconPosstx + 2 +iconSize, iconPossty + 2);
								memDC.MoveTo(iconPosstx + 2 + iconSize, iconPossty + 2);
								memDC.LineTo(iconPosstx + 2 + iconSize, iconPossty + 2 + iconSize);
								memDC.MoveTo(iconPosstx + 2, iconPossty + 2);
								memDC.LineTo(iconPosstx + 2, iconPossty + 2 + iconSize);
								memDC.MoveTo(iconPosstx + 2, iconPossty + 2 + iconSize);
								memDC.LineTo(iconPosstx + 2 + iconSize, iconPossty + 2 + iconSize);
								pen.DeleteObject();
								
								memDC.TransparentBlt(iconPosstx + 2, iconPossty + 2, iconSize, iconSize, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, RGB(0, 0, 0));//SRCCOPY);

							}
						}
					

					}

					iconRectList.push_front(iconRect);
					iconHwndList.push_front(tmphwnd);

					ReleaseDC(pDC);
					cdc.DeleteDC();

				}
			}

		}
	
		RECT rectForSelectDesktop;
		//int nowselect = e_global->getSelectedIndex();
		int selectx = e_global->getSelectedIndex() % mapWidth;
		int selecty = e_global->getSelectedIndex() / mapWidth;


		rectForSelectDesktop.left = selectx*w*mapsize;
		rectForSelectDesktop.top = selecty*w*mapsize;
		rectForSelectDesktop.right = rectForSelectDesktop.left + w*mapsize;
		rectForSelectDesktop.bottom = rectForSelectDesktop.top + w*mapsize;

		memDC.SelectObject(oldpen);
		pen.DeleteObject();
		pen.CreatePen(PS_SOLID, 5, RGB(118, 35, 220));	//보라
		oldpen = memDC.SelectObject(&pen);
		memDC.MoveTo(rectForSelectDesktop.left, rectForSelectDesktop.top);
		memDC.LineTo(rectForSelectDesktop.right, rectForSelectDesktop.top);
		memDC.MoveTo(rectForSelectDesktop.right, rectForSelectDesktop.top);
		memDC.LineTo(rectForSelectDesktop.right, rectForSelectDesktop.bottom);
		memDC.MoveTo(rectForSelectDesktop.left, rectForSelectDesktop.top);
		memDC.LineTo(rectForSelectDesktop.left, rectForSelectDesktop.bottom);
		memDC.MoveTo(rectForSelectDesktop.left, rectForSelectDesktop.bottom);
		memDC.LineTo(rectForSelectDesktop.right, rectForSelectDesktop.bottom);

		memDC.SelectObject(oldpen);
		pen.DeleteObject();
		if (up)
		{
			up = false;
			pen.DeleteObject();
			if (select)
			{
				select = false;
				pen.CreatePen(PS_SOLID, 5, RGB(255, 170, 85));	//주황
				memDC.SelectObject(pen);
				memDC.MoveTo(tmprect.left, tmprect.top);
				memDC.LineTo(tmprect.right, tmprect.top);
				memDC.MoveTo(tmprect.right, tmprect.top);
				memDC.LineTo(tmprect.right, tmprect.bottom);
				memDC.MoveTo(tmprect.left, tmprect.top);
				memDC.LineTo(tmprect.left, tmprect.bottom);
				memDC.MoveTo(tmprect.left, tmprect.bottom);
				memDC.LineTo(tmprect.right, tmprect.bottom);
				memDC.SelectObject(oldpen);
				pen.DeleteObject();
			}
		}
		pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));	//주황
		memDC.SelectObject(pen);
		memDC.MoveTo(0, 0);
		memDC.LineTo(w*mapsize*mapWidth, 0);
		memDC.MoveTo(w*mapsize*mapWidth, 0);
		memDC.LineTo(w*mapsize*mapWidth, w*mapsize*mapWidth);
		memDC.MoveTo(0, 0);
		memDC.LineTo(0, w*mapsize*mapWidth);
		memDC.MoveTo(0, w*mapsize*mapWidth);
		memDC.LineTo(w*mapsize*mapWidth, w*mapsize*mapWidth);
		memDC.SelectObject(oldpen);
		pen.DeleteObject();

		redraw = false;

		//pen.DeleteObject();
		//test 현재 바탕화면의 프로그램 맵에 그리기
	}

	if (iconMoveMode == 1)// Lbutton down
	{

		drawable = true;
		std::list<RECT*>::iterator itr_rect = iconRectList.begin();
		for (std::list<HWND>::iterator itr_hwnd = iconHwndList.begin(); itr_rect != iconRectList.end(); itr_rect++, itr_hwnd++)	//각 데스크탑 별로출력
		{
			if ((*itr_hwnd) == selectIconHwnd)
			{
				std::list<E_Window*> desktop_window = e_global->getSelectedDesktop()->getWindowList();
				for (std::list<E_Window*>::iterator itr_window = desktop_window.begin(); itr_window != desktop_window.end(); itr_window++)	//각데스크탑별로 안에 있는 윈도우 핸들 가져와서 아이콘 출력
				{
					if ((*itr_window)->getWindow() == selectIconHwnd)
						::ShowWindow(selectIconHwnd, SW_NORMAL);
				}

				::BringWindowToTop(this->GetSafeHwnd());
				::BringWindowToTop(selectIconHwnd);
				memDC.FillRect(*itr_rect, &brush);
				foreRect.left = (*itr_rect)->left;
				foreRect.right = (*itr_rect)->right;
				foreRect.bottom = (*itr_rect)->bottom;
				foreRect.top = (*itr_rect)->top;

				CBitmap icon;
				CBitmap oldicon;
				int width = E_Util::getSystemSmallIconSize();
				HICON hicon = E_Util::getIconHandle(selectIconHwnd);
				HBITMAP bitmap = E_Util::ConvertIconToBitmap(hicon, width, width);
				icon.Attach(bitmap);

				BITMAP icon_info;
				icon.GetBitmap(&icon_info);
				memDC.SetStretchBltMode(COLORONCOLOR);
				CDC cdc;
				CDC* pDC2 = GetDC();
				cdc.CreateCompatibleDC(pDC2);
				//cdc.CreateCompatibleDC(this->GetWindowDC());
				cdc.SelectObject(icon);
				cdc.SetBkMode(1);
				cdc.SetBkColor(E_Map::backgroundColor);

				memDC.TransparentBlt(iconClick.x - iconSize / 2, iconClick.y - iconSize / 2, iconSize, iconSize, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, RGB(0, 0, 0));// SRCCOPY);
				cdc.DeleteDC(); 
				ReleaseDC(pDC2);
				icon.DeleteObject();
				DeleteObject(hicon);
				DeleteObject(bitmap);
				break;
			}
		}
	}
	CBitmap *icon;
	if (clicked && iconMoveMode == 2)
	{
		drawable = true;
		memDC.FillRect(&foreRect, &brush);//이전것 지우기
		
		//int width = E_Util::getSystemSmallIconSize();
		//HICON hicon = E_Util::getIconHandle(selectIconHwnd);
		//HBITMAP bitmap = E_Util::ConvertIconToBitmap(hicon, width, width);
		
		std::list<E_Desktop*> all_Desktop = e_global->desktopList;
		for (std::list<E_Desktop*>::iterator itr_desktop = all_Desktop.begin(); itr_desktop != all_Desktop.end(); itr_desktop++)	//각 데스크탑 별로출력
		{
			std::list<E_Window*> desktop_window = (*itr_desktop)->getWindowList();
			for (std::list<E_Window*>::iterator itr_window = desktop_window.begin(); itr_window != desktop_window.end(); itr_window++)	//각데스크탑별로 안에 있는 윈도우 핸들 가져와서 아이콘 출력
			{
				if ((*itr_window)->getWindow() == selectIconHwnd)
				{
					icon->Attach(*(*itr_window)->getIcon());
				}
				//windowindext = (*itr_desktop)->getIndex();
			}
		}






		//.
		
		//icon.Attach(bitmap);

		BITMAP icon_info;
		icon->GetBitmap(&icon_info);
		memDC.SetStretchBltMode(COLORONCOLOR);
		CDC cdc;
		CDC* pDC3 = GetDC();
		cdc.CreateCompatibleDC(pDC3);
		//cdc.CreateCompatibleDC(this->GetWindowDC());
		cdc.SelectObject(icon);
		cdc.SetBkMode(1);
		cdc.SetBkColor(E_Map::backgroundColor);

		memDC.TransparentBlt(iconClick.x - iconSize / 2, iconClick.y - iconSize / 2, iconSize, iconSize, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, RGB(0, 0, 0));// SRCCOPY);
		foreRect.left = iconClick.x - iconSize / 2;
		foreRect.right = iconClick.x + iconSize / 2;
		foreRect.top = iconClick.y - iconSize / 2;
		foreRect.bottom = iconClick.y + iconSize / 2;

		RECT rectForMove;
		long newxpoint = (iconClick.x - iconSize / 2) / e_global->getMapsize()*mapWidth / mapWidth;
		long newypoint = (h - th)*(iconClick.y - iconSize / 2) / w / e_global->getMapsize() / mapHeight*mapHeight;

		//int windowindext;
		std::list<E_Desktop*> all_Desktop3 = e_global->desktopList;
		for (std::list<E_Desktop*>::iterator itr_desktop3 = all_Desktop3.begin(); itr_desktop3 != all_Desktop3.end(); itr_desktop3++)	//각 데스크탑 별로출력
		{
			std::list<E_Window*> desktop_window = (*itr_desktop3)->getWindowList();
			for (std::list<E_Window*>::iterator itr_window = desktop_window.begin(); itr_window != desktop_window.end(); itr_window++)	//각데스크탑별로 안에 있는 윈도우 핸들 가져와서 아이콘 출력
			{
				if ((*itr_window)->getWindow() == selectIconHwnd)
				{
					if ((*itr_window)->dock)
					{
						(*itr_window)->dock = false;
						e_global->dockcount--;
					}
				}
					//windowindext = (*itr_desktop)->getIndex();
			}
		}

		WCHAR name[60];
		::GetWindowText(selectIconHwnd, name, 60);
		//
		int get = 0;
		char* pStr;
		int strSize = WideCharToMultiByte(CP_ACP, 0, name, -1, NULL, 0, NULL, NULL);
		pStr = new char[strSize];
		WideCharToMultiByte(CP_ACP, 0, name, -1, pStr, strSize, 0, 0);
		int resutr = 0;
		if (strstr(pStr, "곰오디오") != NULL || strstr(pStr, "곰플레이어") || strstr(pStr, "스티커"))
			get = 1;
		delete pStr;

		//if ( (newxpoint < w && newypoint < h && e_global->getSelectedIndex() == windowindext) || e_global->getSelectedIndex() == getdesktop(movindexx,movindexy)-1)
		if (e_global->getSelectedIndex() == getdesktop(movindexx, movindexy) - 1)
		{
			::ShowWindow(selectIconHwnd, SW_SHOW);
			::GetWindowRect(selectIconHwnd, &rectForMove);
			rectForChildMove = rectForMove;
			::MoveWindow(selectIconHwnd, newxpoint - w*(movindexx - 1), newypoint - (h - th)*(movindexy - 1), rectForMove.right - rectForMove.left, rectForMove.bottom - rectForMove.top, TRUE);
			//여기에 자식 프로그램 또는 비슷한 프로그램까지 같이 움직여야함
			//부모 핸들로 윈도우 프로세스 얻어오고
			//그 이후에 얻어온 프로세스 아이디로 모든 창들 이동!
			childmovx = newxpoint - w*(movindexx - 1);
			childmovy = newypoint - (h - th)*(movindexy - 1);
			parentprocessId = GetWindowThreadProcessId(selectIconHwnd, NULL);
			if (get)
				EnumWindows(E_Map::EnumCallBackMap, 1);

		}
		else
		{
			::ShowWindow(selectIconHwnd, SW_HIDE);
			if (get)
				EnumWindows(E_Map::EnumCallBackMap, 0);
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
		CPen* oldpen = memDC.SelectObject(&pen);
		memDC.MoveTo(tmprect.left, tmprect.top);
		memDC.LineTo(tmprect.right, tmprect.top);
		memDC.MoveTo(tmprect.right, tmprect.top);
		memDC.LineTo(tmprect.right, tmprect.bottom);
		memDC.MoveTo(tmprect.left, tmprect.top);
		memDC.LineTo(tmprect.left, tmprect.bottom);
		memDC.MoveTo(tmprect.left, tmprect.bottom);
		memDC.LineTo(tmprect.right, tmprect.bottom);
		memDC.SelectObject(oldpen);
		pen.DeleteObject();
		
		
		ReleaseDC(pDC3);
		cdc.DeleteDC();
		
		//DeleteObject(hicon);
		//DeleteObject(bitmap);
	}

	if (drawable)
		dc.StretchBlt(0, 0, w, h, &memDC, 0, 0, w, h, SRCCOPY);
	::ReleaseDC(this->GetSafeHwnd(),pDCM->GetSafeHdc());
	
	
	memDC.DeleteDC();
	bmp.DeleteObject();
	brush.DeleteObject();

	
	//DeleteDC(dc);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.
}

BOOL CALLBACK  E_Map::EnumCallBackMap(HWND hwnd, LPARAM lParam)
{
	WCHAR name[10];
	WCHAR name2[4];
	WCHAR name3[] = L"스티커";
	//WCHAR name4[] = L"카카오";
	::GetWindowText(hwnd, name2, 4);

	WCHAR expandable[11];
	WCHAR expan[] = TEXT("expandable");
	::GetWindowText(hwnd, expandable, 11);
	if (wcscmp(expandable, expan) == 0)
		return TRUE;

	if ((::GetWindowText(hwnd, name, 10) && ::IsWindowVisible(hwnd)) || wcscmp(name2, name3) == 0)//|| wcscmp(name4, name5) == 0)
	{
		E_Map* e_map = E_Map::getSingleton();
		DWORD childprocessId;
		childprocessId = GetWindowThreadProcessId(hwnd, NULL);
		if (childprocessId == e_map->parentprocessId)
		{
			if (lParam)
			{
				//if (wcscmp(name2, name4) == 0)
				//return false;
				RECT rectforchildmov;
				::ShowWindow(hwnd, SW_SHOW);
				::GetWindowRect(hwnd, &rectforchildmov);
				::MoveWindow(hwnd, e_map->childmovx, e_map->childmovy, rectforchildmov.right - rectforchildmov.left, rectforchildmov.bottom - rectforchildmov.top, TRUE);
			}
			else
				::ShowWindow(hwnd, SW_HIDE);
		}
	}
	return true;
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
	clickedforerror = true;
	//leave2 = false;
	::BringWindowToTop(this->GetSafeHwnd());
	up = true;
	forSelectMap = true;
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	std::list<HWND>::iterator itr_hwnd = iconHwndList.begin();
	for (std::list<RECT*>::iterator itr_rect = iconRectList.begin(); itr_rect != iconRectList.end(); itr_rect++, itr_hwnd++)	//각 데스크탑 별로출력
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
	clickedforerror = false;
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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	iconMoveMode = 0;
	clicked = false;
	time = e_global->getTimer();

	RECT trect;
	trect.left = point.x;// -iconSize / 2;
	trect.top = point.y;// -iconSize / 2;
	trect.right = trect.left + iconSize;
	trect.bottom = trect.top + iconSize;

	int mapWidth = e_global->getDesktopWidth();
	int mapHeight = e_global->getDesktopHeight();
	int desktop = 0;
	int bre = 0;
	std::list<RECT*>::iterator itr_rect = iconRectList.begin();
	for (std::list<HWND>::iterator itr_hwnd = iconHwndList.begin(); itr_hwnd != iconHwndList.end(); itr_hwnd++, itr_rect++)	//각 데스크탑 별로출력
	{
		if ((*itr_hwnd) == selectIconHwnd)
		{
			RECT trect2;
			int fromdesktop = getdesktop(clickindexx, clickindexy);

			int todesktop = getdesktop(upindexx, upindexy);
			long xp = point.x - (upindexx - 1)*w*mapsize;
			long yp = point.y - (upindexy - 1)*w*mapsize;
			//CWnd *pWnd = CWnd::FromHandle(selectIconHwnd);
			std::list<E_Window*> winlist = e_global->getDesktop(fromdesktop - 1)->getWindowList();//여기서 selected가 아니라
			//해당 아이콘이 있는 데스크탑으로
			for (std::list<E_Window*>::iterator itr_wind = winlist.begin(); itr_wind != winlist.end(); itr_wind++)	//각 데스크탑 별로출력
			{
				if ((*itr_wind)->getWindow() == selectIconHwnd)
				{
					e_global->getDesktop(fromdesktop - 1)->excludeWindow((*itr_wind));//현재 핸들인 윈도우))
					e_global->getDesktop(todesktop - 1)->insertWindow((*itr_wind));
				}
			}

			::GetWindowRect(selectIconHwnd, &trect2);
			::MoveWindow(selectIconHwnd, (xp - iconSize / 2) / mapsize, (yp - iconSize / 2) * (h - th) / w / mapsize, trect2.right - trect2.left, trect2.bottom - trect2.top, TRUE);

			in = true;
			/*iconHwndList.remove((*itr_hwnd));
			iconRectList.remove((*itr_rect));
			iconHwndList.push_front(selectIconHwnd);
			iconRectList.push_front(&trect);*/
			selectIconHwnd = NULL;

			for (int i = 1; i < mapHeight + 1; i++)
			{
				for (int j = 1; j < mapWidth + 1; j++)
				{
					desktop++;
					if (j == upindexx && i == upindexy)
					{	//여기서 윈도우를 해당 desktop으로 집어 넣음
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
					//여기서 윈도우를 해당 desktop으로 집어 넣음
					bre = 1;
					break;
				}
			}
			if (bre)
				break;
		}



		///


		//selecteddesktop 의 윈도우만 보여주고 나머지는 지우기
	
		WCHAR name5[60];
		char* pStr;
		int index = e_global->getSelectedDesktop()->getIndex();
		std::list<E_Desktop*> desklist = e_global->desktopList;
		for (std::list<E_Desktop*>::iterator itr_desk = desklist.begin(); itr_desk != desklist.end(); itr_desk++)	//각 데스크탑 별로출력
		{
			if ((*itr_desk)->getIndex() == index)
			{
				
				(*itr_desk)->setAllShow();
				std::list<E_Window*> winlist1 = (*itr_desk)->getWindowList();
				for (std::list<E_Window*>::iterator itr_window = winlist1.begin(); itr_window != winlist1.end(); itr_window++)	//각 데스크탑 별로출력
				{
					::GetWindowText((*itr_window)->getWindow(), name5, 60);
					int strSize = WideCharToMultiByte(CP_ACP, 0, name5, -1, NULL, 0, NULL, NULL);
					pStr = new char[strSize];
					WideCharToMultiByte(CP_ACP, 0, name5, -1, pStr, strSize, 0, 0);
					if (strstr(pStr, "곰오디오")  || strstr(pStr, "곰플레이어")  || strstr(pStr, "스티커") || strstr(pStr, "GXWINDOW") )
					{
						pidforhide = GetWindowThreadProcessId((*itr_window)->getWindow(), NULL);
						EnumWindows(E_Map::special, 1);
					}
					else
					{

						pidforhide = GetWindowThreadProcessId((*itr_window)->getWindow(), NULL);
						EnumWindows(E_Map::EnumCallHide, 0);
					}
					delete pStr;
				}
				
				continue;
			}
			(*itr_desk)->setAllHide();
			std::list<E_Window*> winlist2 = (*itr_desk)->getWindowList();
			for (std::list<E_Window*>::iterator itr_window = winlist2.begin(); itr_window != winlist2.end(); itr_window++)	//각 데스크탑 별로출력
			{
				::GetWindowText((*itr_window)->getWindow(), name5, 60);
				int strSize = WideCharToMultiByte(CP_ACP, 0, name5, -1, NULL, 0, NULL, NULL);
				pStr = new char[strSize];
				WideCharToMultiByte(CP_ACP, 0, name5, -1, pStr, strSize, 0, 0);
				if (strstr(pStr, "곰오디오") || strstr(pStr, "곰플레이어") || strstr(pStr, "스티커") || strstr(pStr, "GXWINDOW"))
				{
					pidforhide = GetWindowThreadProcessId((*itr_window)->getWindow(), NULL);
					EnumWindows(E_Map::special, 0);
				}
				else
				{

					pidforhide = GetWindowThreadProcessId((*itr_window)->getWindow(), NULL);
					EnumWindows(E_Map::EnumCallHide, 1);
				}
				delete pStr;
			}
			
		}

		//std::list<HWND> docklist = e_global->dockedWindowList;
		//for (std::list<HWND>::iterator itr_dock = docklist.begin(); itr_dock != docklist.end(); itr_dock++)	//각 데스크탑 별로출력
		//{
		//	::ShowWindow((*itr_dock), SW_SHOW);
		//}
		std::list<E_Desktop*> desklist2 = e_global->desktopList;
		for (std::list<E_Desktop*>::iterator itr_desk = desklist2.begin(); itr_desk != desklist2.end(); itr_desk++)	//각 데스크탑 별로출력
		{
			std::list<E_Window*> winlist2 = (*itr_desk)->getWindowList();
			for (std::list<E_Window*>::iterator itr_window = winlist2.begin(); itr_window != winlist2.end(); itr_window++)	//각 데스크탑 별로출력
			{
				if ((*itr_window)->dock)
				{
					::ShowWindow((*itr_window)->getWindow(), SW_SHOW);
					WCHAR name[60];
					::GetWindowText((*itr_window)->getWindow(), name, 60);
					char* pStr;
					int strSize = WideCharToMultiByte(CP_ACP, 0, name, -1, NULL, 0, NULL, NULL);
					pStr = new char[strSize];
					WideCharToMultiByte(CP_ACP, 0, name, -1, pStr, strSize, 0, 0);
					int resutr = 0;
					parnetpid = GetWindowThreadProcessId((*itr_window)->getWindow(), NULL);
					if (strstr(pStr, "곰오디오") != NULL || strstr(pStr, "곰플레이어") || strstr(pStr, "스티커"))
						EnumWindows(EnumShow, 0);
				}
			}
		}




		::BringWindowToTop(this->GetSafeHwnd());
		//e_global->getSelectedDesktop()->setAllShow(); 





	}
}
BOOL CALLBACK  E_Map::special(HWND hwnd, LPARAM lParam)
{
	WCHAR name[10];
	WCHAR name2[4];
	WCHAR name3[] = L"스티커";
	//WCHAR name4[] = L"카카오";
	::GetWindowText(hwnd, name2, 4);

	WCHAR expandable[11];
	WCHAR expan[] = TEXT("expandable");
	::GetWindowText(hwnd, expandable, 11);
	if (wcscmp(expandable, expan) == 0)
		return TRUE;

	if (::GetWindowText(hwnd, name, 10) && ::IsWindowVisible(hwnd))//&& ::IsWindowVisible(hwnd)) || wcscmp(name2, name3) == 0)//|| wcscmp(name4, name5) == 0)
	{
		E_Map* e_map = E_Map::getSingleton();
		DWORD childprocessId;
		childprocessId = GetWindowThreadProcessId(hwnd, NULL);
		if (childprocessId == e_map->pidforhide)
		{
			if (lParam)
			{
				::ShowWindow(hwnd, SW_SHOW);
			}
			else
				::ShowWindow(hwnd, SW_HIDE);
		}
	}
	return true;
}

BOOL CALLBACK  E_Map::EnumShow(HWND hwnd, LPARAM lParam)
{
	WCHAR name[10];
	WCHAR name2[4];
	WCHAR name3[] = L"스티커";
	//WCHAR name4[] = L"카카오";
	WCHAR expandable[11];
	WCHAR expan[] = TEXT("expandable");
	::GetWindowText(hwnd, name2, 4);
	::GetWindowText(hwnd, expandable, 11);

	if (wcscmp(expandable, expan) == 0)
		return TRUE;
	if ((::GetWindowText(hwnd, name, 10) && ::IsWindowVisible(hwnd)) || wcscmp(name2, name3) == 0)//|| wcscmp(name4, name5) == 0)
	{
		E_Map* e_map = E_Map::getSingleton();
		DWORD childprocessId;
		childprocessId = GetWindowThreadProcessId(hwnd, NULL);
		if (childprocessId == e_map->parnetpid)
			::ShowWindow(hwnd, SW_SHOW);
	}
	return true;
}

BOOL CALLBACK  E_Map::EnumCallHide(HWND hwnd, LPARAM lParam)
{

	WCHAR name[] = TEXT("expandable");
	WCHAR name2[] = L"스티커 메모";
	WCHAR name3[] = L"곰오디오";
	WCHAR name4[] = L"곰플레이어";
	WCHAR expandable[100];
	



	::GetWindowText(hwnd, expandable, 100);
	if ( wcscmp(name, expandable) == 0 || wcscmp(name2, expandable) == 0 || wcscmp(name3, expandable) == 0 || wcscmp(name4, expandable) == 0 )
	{

	}
	else
		return TRUE;


	//WCHAR name4[] = L"Microsoft Spy++";
	//WCHAR name5[16];
	//	::GetWindowText(hwnd, name4, 4);
	//::GetWindowText(hwnd, name5, 16);
	
	if ((::GetWindowText(hwnd, name, 10) &&  ::IsWindowVisible(hwnd) ) )//|| wcscmp(name4, name5) == 0)
	{
		E_Map* e_map = E_Map::getSingleton();
		DWORD pidforchild;
		pidforchild = GetWindowThreadProcessId(hwnd, NULL);
		if (pidforchild == e_map->pidforhide)
		{
			if (lParam)
				::ShowWindow(hwnd, SW_HIDE);
			else
				::ShowWindow(hwnd, SW_SHOW);
		}
	}
	return true;
}


int E_Map::getdesktop(int indexx, int indexy)
{
	E_Global* e_global = E_Global::getSingleton();
	int mapWidth = e_global->getDesktopWidth();
	int mapHeight = e_global->getDesktopHeight();
	int desktop = 0;
	int bre = 0;
	for (int i = 1; i < mapHeight + 1; i++)
	{
		for (int j = 1; j < mapWidth + 1; j++)
		{
			desktop++;
			if (j == indexx && i == indexy)
			{	//여기서 윈도우를 해당 desktop으로 집어 넣음
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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	SetCursor(LoadCursor(NULL, IDC_ARROW)); // 기본
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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
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
		if ((GetAsyncKeyState(VK_LBUTTON) && 0x8000) || (GetAsyncKeyState(VK_RBUTTON) && 0x8000))
		{
			GetCursorPos(&pt);
			if (getSize.PtInRect(pt))
				alreadyin = true;
			if (GetForegroundWindow() != hwnd_cwnd_emap && !alreadyin)
			{
				KillTimer(10);
				//::SendMessage(this->GetSafeHwnd(), WM_LBUTTONUP, 0, 0);
				terminateMap();
			}
		}
		else
			alreadyin = false;

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
HRESULT E_Map::OnInvali(WPARAM wParam, LPARAM lParam)
{
	// TODO: Your Code
	checkdelete = true;
	if ((int)lParam == 0)
	{
		HWND hwnd = (HWND)wParam;
		E_Global* e_global = E_Global::getSingleton();
		


		std::list<E_Desktop*> desklist = e_global->desktopList;
		for (std::list<E_Desktop*>::iterator itr_desk = desklist.begin(); itr_desk != desklist.end(); itr_desk++)	//각 데스크탑 별로출력
		{
			std::list<E_Window*> winlist = (*itr_desk)->getWindowList();
			for (std::list<E_Window*>::iterator itr_window = winlist.begin(); itr_window != winlist.end(); itr_window++)	//각 데스크탑 별로출력
			{
				if ((*itr_window)->getWindow() == hwnd)
				{
					(*itr_desk)->removeWindow((*itr_window));
				}
			}
		}
	}
	Invalidate(0);
	return TRUE;
}
void E_Map::OnMouseLeave()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
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
//	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
//}


int E_Map::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	SetTimer(10, 15, NULL);

	return 0;
}


void E_Map::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	E_Global* e_global = E_Global::getSingleton();
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	long w = enManager->getWidth();
	double mapsize = e_global->getMapsize();
	//leave2 = false;
	::BringWindowToTop(this->GetSafeHwnd());
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	std::list<HWND>::iterator itr_hwnd = iconHwndList.begin();
	for (std::list<RECT*>::iterator itr_rect = iconRectList.begin(); itr_rect != iconRectList.end(); itr_rect++, itr_hwnd++)	//각 데스크탑 별로출력
	{
		if ((*itr_rect)->left < point.x && (*itr_rect)->right > point.x && (*itr_rect)->top < point.y && (*itr_rect)->bottom > point.y)
		{

			::SendMessage(e_global->hwnd_frame, WM_USER_MAPR, (WPARAM)(*itr_hwnd), 0);
			
			//
			break;
		}
	}




	__super::OnRButtonDown(nFlags, point);
}
