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
	
END_MESSAGE_MAP()


// ShowMenu 메시지 처리기입니다.

