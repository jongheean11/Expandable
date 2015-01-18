#include "stdafx.h"
#include "E_Notify.h"

#include "E_Global.h"
#include "E_Map.h"
#pragma once

//E_Notify* E_Notify::singleton = NULL;
E_Notify::E_Notify()
{
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_Global* e_global = E_Global::getSingleton();
	long w = enManager->getWidth();
	long h = enManager->getHeight();
	hwnd_cwnd_notify = this;
	time = 50;
	click = false;
	notifywidth = w * 0.15;
	notifyheight = h * 0.08;
	myPos = 0; 
	colornum = 250;

}

const wchar_t* E_Notify::caption = L"Notify";
E_Notify::~E_Notify()
{
	if (hwnd_cwnd_notify != NULL)
	{
		hwnd_cwnd_notify->DestroyWindow();
		hwnd_cwnd_notify = NULL;
	}
}

//E_Notify* E_Notify::getSingleton()
//{
//	if (singleton == NULL)
//		singleton = new E_Notify();
//	return singleton;
//}

void E_Notify::showNotify(int processId, char* processName, HWND hwnd)
{
	pId = processId;
	pName = processName;
	//pHwnd = hwnd;
	pHwnd = ::FindWindow(NULL, L"카카오톡");
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_Global* e_global = E_Global::getSingleton();
	e_global->nowActiveNotify++;
	e_global->nextActiveNotify = e_global->nowActiveNotify % 5;
	long w = enManager->getWidth();
	long h = enManager->getHeight();
	long th = enManager->getTaskbarHeight();
	hwnd_cwnd_notify = this;
	int notPos = 0;
	for (int i = 0; i < 5; i++)
	{
		if (e_global->notifyAblePos[i] == 0)
		{
			e_global->notifyAblePos[i] = 1;
			myPos = i;
			notPos = 1;	// notPos 가 1인경우 자리 있음 0인경우 5자리 다차있음
			e_global->notifyHwnd[i] = hwnd_cwnd_notify;
			break;
		}
	}
	if (!notPos)
	{
		e_global->notifyHwnd[e_global->nextActiveNotify]->DestroyWindow();
		myPos = e_global->nextActiveNotify;
		e_global->notifyHwnd[myPos] = hwnd_cwnd_notify;
	}
	
	CBrush brush_map;
	UINT nClassStyle_map = 0;// CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	brush_map.CreateSolidBrush(RGB(255, 255, 255));
	brush_map.CreateStockObject(NULL_BRUSH);
	CString szClassName_notify = AfxRegisterWndClass(nClassStyle_map, 0, (HBRUSH)brush_map.GetSafeHandle(), 0);
	hwnd_cwnd_notify->CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_notify, E_Notify::caption, WS_VISIBLE | WS_POPUP, CRect(0, 0, notifywidth, notifyheight), CWnd::GetDesktopWindow(), 0);

	SetTimer(1, 100, NULL);
	hwnd_cwnd_notify->ShowWindow(SW_SHOW);
	::SetWindowLongW(hwnd_cwnd_notify->m_hWnd, GWL_EXSTYLE, GetWindowLong(hwnd_cwnd_notify->m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	::SetLayeredWindowAttributes(hwnd_cwnd_notify->m_hWnd, 0, e_global->getTransparent(), LWA_ALPHA); //창투명
	hwnd_cwnd_notify->UpdateWindow();
	hwnd_cwnd_notify->SetWindowPos(NULL, 0, (h - th) - notifyheight*(myPos + 1), notifywidth, notifyheight, SWP_NOZORDER | SWP_SHOWWINDOW);
	

}
BEGIN_MESSAGE_MAP(E_Notify, CWnd)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()



void E_Notify::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.
	CBrush brush;

	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton(); 
	long w = enManager->getWidth();
	if (click)
		brush.CreateSolidBrush(RGB(100, 100, 50));
	else
		brush.CreateSolidBrush(RGB(colornum, colornum, colornum / 4));
		//brush.CreateSolidBrush(RGB(colornum, colornum, colornum /4));
	dc.SelectObject(brush);
	dc.Rectangle(0, 0, notifywidth, notifyheight);
	CString s1,s2;
	s1 = pName;
	s2 = "작업 완료.";

	CFont font;
	// 폰트를 굴림체, 11포인트, 보통속성 크기로 생성
	font.CreateFont(w*1/63, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("굴림체"));
	CFont *pOld_Font = dc.SelectObject(&font);
	dc.SetTextColor(RGB(0, 0, 0)); // 텍스트 검정색으로 설정
	dc.SetBkMode(TRANSPARENT);
	
	dc.TextOutW(10, 10, s1);
	font.DeleteObject(); 
	font.CreateFont(w/95, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("굴림체"));
	pOld_Font = dc.SelectObject(&font);
	dc.TextOutW(10,  notifyheight/2+10, s2);

}

void E_Notify::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == 1)
	{
		time--;
		colornum -= 3.5;
		Invalidate(0);
		if (time < 0)
		{
			KillTimer(1);
			terminateNotify();
		}
		
	}
	CWnd::OnTimer(nIDEvent);
}

void E_Notify::terminateNotify()
{
	E_Global* e_global = E_Global::getSingleton();
	//e_global->nowActiveNotify--;
	e_global->notifyAblePos[myPos] = 0;
	hwnd_cwnd_notify->DestroyWindow();
	
}



void E_Notify::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	click = true;
	time = 50;
	Invalidate(0);
	CWnd::OnLButtonDown(nFlags, point);
}

BOOL CALLBACK  E_Notify::EnumCallHide(HWND hwnd, LPARAM lParam)
{
	E_Global* e_global = E_Global::getSingleton();
	WCHAR name[10];
	WCHAR name2[4];
	WCHAR name3[] = L"스티커";
	::GetWindowText(hwnd, name2, 4);

	if ((::GetWindowText(hwnd, name, 10) && ::IsWindowVisible(hwnd)) || wcscmp(name2, name3) == 0)//|| wcscmp(name4, name5) == 0)
	{
		DWORD pidforchild;
		pidforchild = GetWindowThreadProcessId(hwnd, NULL);
		if (pidforchild == e_global->pidforhideNotify)
		{
			if (lParam)
				::ShowWindow(hwnd, SW_HIDE);
			else
				::ShowWindow(hwnd, SW_SHOW);
		}
	}
	return true;
}

void E_Notify::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	E_Map* e_map = E_Map::getSingleton();
	E_Global* e_global = E_Global::getSingleton();
	//선택된 핸들인 pHwnd핸들이 어떤 데스크탑인지 확인하고
	//해당 데스크탑 show
	int desDesk = e_global->getSelectedIndex();
	int firstDesk = desDesk;
	//bool useHwndFind = false;
	std::list<E_Desktop*> desklist = e_global->desktopList;
	for (std::list<E_Desktop*>::iterator itr_desk = desklist.begin(); itr_desk != desklist.end(); itr_desk++)	//각 데스크탑 별로출력
	{
		std::list<E_Window*> winlist1 = (*itr_desk)->getWindowList();
		for (std::list<E_Window*>::iterator itr_window = winlist1.begin(); itr_window != winlist1.end(); itr_window++)	//각 데스크탑 별로출력
		{
			if ((*itr_window)->getWindow() == pHwnd)
			{
				desDesk = (*itr_desk)->getIndex();
				e_global->setSelectedIndex(desDesk);
				//useHwndFind = true;	//찾았는지여부
				break;
			}
		}
		if (desDesk != e_global->getSelectedIndex())
			break;
	}
	//해당 데스크탑 찾음 
	//만약 못찾았으면 ->useHwndFInd 가 false 일때

	//
	std::list<E_Desktop*> desklist2 = e_global->desktopList;
	for (std::list<E_Desktop*>::iterator itr_desk = desklist2.begin(); itr_desk != desklist2.end(); itr_desk++)	//각 데스크탑 별로출력
	{
		if ((*itr_desk)->getIndex() == desDesk)
		{
			(*itr_desk)->setAllShow();

			std::list<E_Window*> winlist1 = (*itr_desk)->getWindowList();
			for (std::list<E_Window*>::iterator itr_window = winlist1.begin(); itr_window != winlist1.end(); itr_window++)	//각 데스크탑 별로출력
			{
				e_global->pidforhideNotify = GetWindowThreadProcessId((*itr_window)->getWindow(), NULL);
				EnumWindows(E_Notify::EnumCallHide, 0);
			}
			continue;
		}
		(*itr_desk)->setAllHide();

		std::list<E_Window*> winlist2 = (*itr_desk)->getWindowList();
		for (std::list<E_Window*>::iterator itr_window = winlist2.begin(); itr_window != winlist2.end(); itr_window++)	//각 데스크탑 별로출력
		{
			e_global->pidforhideNotify = GetWindowThreadProcessId((*itr_window)->getWindow(), NULL);
			EnumWindows(E_Notify::EnumCallHide, 1);
		}
	}
	terminateNotify();
	if (firstDesk != e_global->getSelectedIndex())
		e_map->drawMap();

	::BringWindowToTop(pHwnd);
	click = false;
	CWnd::OnLButtonUp(nFlags, point);
}


void E_Notify::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	SetCursor(LoadCursor(NULL, IDC_ARROW)); // 기본
	time = 50;
	colornum = 250;
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


void E_Notify::OnMouseLeave()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	TRACKMOUSEEVENT MouseEvent;
	::ZeroMemory(&MouseEvent, sizeof(MouseEvent));
	MouseEvent.cbSize = sizeof(MouseEvent);
	MouseEvent.dwFlags = TME_CANCEL;
	MouseEvent.hwndTrack = m_hWnd;
	::_TrackMouseEvent(&MouseEvent);
	m_bTrack = false;


	click = false;
	Invalidate(0);
	CWnd::OnMouseLeave();
}
