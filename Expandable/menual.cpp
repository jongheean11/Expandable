// menual.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Expandable.h"
#include "menual.h"
#include "afxdialogex.h"


// menual ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(menual, CDialog)

menual::menual(CWnd* pParent /*=NULL*/)
	: CDialog(menual::IDD, pParent)
{

}

menual::~menual()
{
}

void menual::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(menual, CDialog)
END_MESSAGE_MAP()


// menual �޽��� ó�����Դϴ�.
