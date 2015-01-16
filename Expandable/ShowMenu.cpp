// ShowMenu.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Expandable.h"
#include "ShowMenu.h"
#include "afxdialogex.h"


// ShowMenu 대화 상자입니다.

IMPLEMENT_DYNAMIC(ShowMenu, CDialogEx)

ShowMenu::ShowMenu(CWnd* pParent /*=NULL*/)
	: CDialogEx(ShowMenu::IDD, pParent)
{

}

ShowMenu::~ShowMenu()
{
}

void ShowMenu::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ShowMenu, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// ShowMenu 메시지 처리기입니다.


void ShowMenu::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, TEXT("res\a.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE
		| LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
	BITMAP bmInfo;
	::GetObject(hBitmap, sizeof(BITMAP), &bmInfo);
	CStatic* pCtrl = new CStatic;
	pCtrl->Create(TEXT(""), WS_CHILD | WS_VISIBLE | SS_BITMAP, CRect(20, 40, 20 + bmInfo.bmWidth, 40 +
		bmInfo.bmHeight), this, 1000);
	pCtrl->SetBitmap(hBitmap);



}
