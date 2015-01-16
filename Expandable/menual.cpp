// menual.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Expandable.h"
#include "menual.h"
#include "afxdialogex.h"


// menual 대화 상자입니다.

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


// menual 메시지 처리기입니다.
