#include "stdafx.h"
#include "E_Notify.h"

#include "E_Global.h"

#pragma once

//E_Notify* E_Notify::singleton = NULL;
E_Notify::E_Notify()
{
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_Global* e_global = E_Global::getSingleton();
	long w = enManager->getWidth();
	long h = enManager->getHeight();
	hwnd_cwnd_notify = this;
	time = 3;
	notifywidth = w * 0.15;
	notifyheight = h * 0.08;

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
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	E_Global* e_global = E_Global::getSingleton();
	e_global->nowActiveNotify++;
	long w = enManager->getWidth();
	long h = enManager->getHeight();
	long th = enManager->getTaskbarHeight();
	
	
	hwnd_cwnd_notify = this;
	CBrush brush_map;
	UINT nClassStyle_map = 0;// CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	brush_map.CreateSolidBrush(RGB(255, 255, 255));
	brush_map.CreateStockObject(NULL_BRUSH);
	CString szClassName_notify = AfxRegisterWndClass(nClassStyle_map, 0, (HBRUSH)brush_map.GetSafeHandle(), 0);
	hwnd_cwnd_notify->CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_notify, E_Notify::caption, WS_VISIBLE | WS_POPUP, CRect(0, 0, notifywidth, notifyheight), CWnd::GetDesktopWindow(), 0);

	SetTimer(1, 1000, NULL);
	hwnd_cwnd_notify->ShowWindow(SW_SHOW);
	::SetWindowLongW(hwnd_cwnd_notify->m_hWnd, GWL_EXSTYLE, GetWindowLong(hwnd_cwnd_notify->m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	::SetLayeredWindowAttributes(hwnd_cwnd_notify->m_hWnd, 0, 150, LWA_ALPHA); //창투명
	hwnd_cwnd_notify->UpdateWindow();
	hwnd_cwnd_notify->SetWindowPos(NULL, 0, (h - th) - notifyheight*(e_global->nowActiveNotify) , notifywidth, notifyheight, SWP_NOZORDER | SWP_SHOWWINDOW);
	

}
BEGIN_MESSAGE_MAP(E_Notify, CWnd)
	ON_WM_TIMER()
	ON_WM_PAINT()
END_MESSAGE_MAP()



void E_Notify::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.
	CBrush brush;
	brush.CreateSolidBrush(RGB(230, 220, 50));
	dc.SelectObject(brush);
	dc.Rectangle(0, 0, notifywidth, notifyheight);
	

}

void E_Notify::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == 1)
	{
		time--;
		if (time < 0)
		{
			E_Global* e_global = E_Global::getSingleton();
			KillTimer(1);
			time = 5;
			terminateNotify();
		}
	}
	CWnd::OnTimer(nIDEvent);
}

void E_Notify::terminateNotify()
{
	E_Global* e_global = E_Global::getSingleton();
	e_global->nowActiveNotify--;

	
	hwnd_cwnd_notify->DestroyWindow();
	
}

