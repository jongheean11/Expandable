#include "stdafx.h"
#include "E_WindowTest.h"


E_WindowTest::E_WindowTest()
{
}


E_WindowTest::~E_WindowTest()
{
}


// ½ºÅ©¸°¼¦ Å×½ºÆ®
void E_WindowTest::testTakeScreenshot()
{
	CWnd* cwnd = E_Global::getSingleton()->getKakaoWindow();


	E_Window* window = new E_Window(cwnd->GetSafeHwnd());

	//Åõ¸í
	//window->setTransparent();
	//window->setOpaque();
	//cwnd->ShowWindow(SW_RESTORE);
	//
	
	//window->takeScreenshot();
	//delete window;

	CWnd* testwin = new CWnd();

	CBrush brush_window;
	brush_window.CreateSolidBrush(RGB(255,255, 255));
	UINT nClassStyle = CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	CString szClassName_window = AfxRegisterWndClass(nClassStyle, 0, (HBRUSH)brush_window.GetSafeHandle(), 0);

	testwin->Create(szClassName_window, _T("¾È³ç"), WS_VISIBLE, CRect(0,0, 1000, 1000), CWnd::GetDesktopWindow(), 1234);

	CPaintDC dc(testwin);
	dc.SetDCBrushColor(RGB(0, 0, 0));
	CRect rect = { 0, 0, 100, 100 };
	dc.Rectangle(10, 10, 200, 200);
	RECT rect2 = { 0, 0, 111, 111 };
	wchar_t *d = L"asdfasdf";
	dc.DrawText(d, wcslen(d), &rect2, 0);


	//dc.BitBlt(200, 500, 100, 100, &cdc, 0, 0, SRCCOPY);
	testwin->ShowWindow(SW_SHOW);
	testwin->UpdateWindow();
	::SetWindowPos(testwin->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	//½ºÅ©¸°¼¦
	CBitmap* bitmap = window->getScreenshot();
	CDC cdc;
	cdc.CreateCompatibleDC(testwin->GetWindowDC());
	cdc.SelectObject(bitmap->GetSafeHandle());

	dc.SetStretchBltMode(COLORONCOLOR | HALFTONE);
	dc.StretchBlt(0, 0, 400, 400, &cdc, 0, 0, 1000, 1000, SRCCOPY);
}