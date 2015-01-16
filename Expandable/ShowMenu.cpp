// ShowMenu.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Expandable.h"
#include "ShowMenu.h"
#include "afxdialogex.h"


// ShowMenu ��ȭ �����Դϴ�.

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


// ShowMenu �޽��� ó�����Դϴ�.


void ShowMenu::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CDialogEx::OnPaint()��(��) ȣ������ ���ʽÿ�.
	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, TEXT("res\a.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE
		| LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
	BITMAP bmInfo;
	::GetObject(hBitmap, sizeof(BITMAP), &bmInfo);
	CStatic* pCtrl = new CStatic;
	pCtrl->Create(TEXT(""), WS_CHILD | WS_VISIBLE | SS_BITMAP, CRect(20, 40, 20 + bmInfo.bmWidth, 40 +
		bmInfo.bmHeight), this, 1000);
	pCtrl->SetBitmap(hBitmap);



}
