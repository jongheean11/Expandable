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
	
END_MESSAGE_MAP()


// ShowMenu �޽��� ó�����Դϴ�.

