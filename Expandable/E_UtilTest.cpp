#include "stdafx.h"
#include "E_UtilTest.h"

void E_UtilTest::testGetDefaultIconSize()
{
	int width = E_Util::getSystemSmallIconSize();
	TRACE_WIN32A("[E_UtilTest::testGetDefaultIconSize()] width: %d", width);
}
//Ä«Ä«¿ÀÅå ½ÇÇà ÇÊ¿ä
void E_UtilTest::testGetIconHandle()
{
	HICON icon = E_Util::getIconHandle(E_Global::getSingleton()->getKakaoWindow()->GetSafeHwnd());
	if (icon)
		TRACE_WIN32A("[OK] testGetIconHandle");
	else
		TRACE_WIN32A("[FAIL] testGetIconHandle (need excute kakao)");
}

void E_UtilTest::testConvertIconToBitmape()
{
	HICON icon = E_Util::getIconHandle(E_Global::getSingleton()->getKakaoWindow()->GetSafeHwnd());
	if (icon){
		int width = E_Util::getSystemSmallIconSize();
		HBITMAP handle = E_Util::ConvertIconToBitmap(icon, width, width);
		
		CWnd* cwnd = E_Global::getSingleton()->getKakaoWindow();

		//Åõ¸í
		//window->setTransparent();
		//window->setOpaque();
		//cwnd->ShowWindow(SW_RESTORE);
		//

		//window->takeScreenshot();
		//delete window;

		CWnd* testwin = new CWnd();

		CBrush brush_window;
		brush_window.CreateSolidBrush(RGB(255, 255, 255));
		UINT nClassStyle = CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		CString szClassName_window = AfxRegisterWndClass(nClassStyle, 0, (HBRUSH)brush_window.GetSafeHandle(), 0);

		testwin->Create(szClassName_window, _T("¾È³ç"), WS_VISIBLE, CRect(0, 0, 1000, 1000), CWnd::GetDesktopWindow(), 1234);

		CPaintDC dc(testwin);
		
		//dc.BitBlt(200, 500, 100, 100, &cdc, 0, 0, SRCCOPY);
		testwin->ShowWindow(SW_SHOW);
		testwin->UpdateWindow();
		::SetWindowPos(testwin->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		//½ºÅ©¸°¼¦
		CDC cdc;
		cdc.CreateCompatibleDC(testwin->GetWindowDC());
		cdc.SelectObject(handle);

		dc.SetStretchBltMode(HALFTONE);
		dc.StretchBlt(100, 100, 400, 400, &cdc, 0, 0, 1000, 1000, SRCCOPY);

		TRACE_WIN32A("[OK] testConvertIconToBitmape (need excute kakao)");
	}
	else{
		TRACE_WIN32A("[FAIL] testConvertIconToBitmape (need excute kakao)");
	}
}
E_UtilTest::E_UtilTest()
{
}


E_UtilTest::~E_UtilTest()
{
}
