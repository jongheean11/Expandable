// EnviromnmentDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Expandable.h"
#include "EnviromnmentDialog.h"
#include "afxdialogex.h"
#include "E_Global.h"
#include "E_Map.h"
// EnviromnmentDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(EnviromnmentDialog, CDialog)

EnviromnmentDialog::EnviromnmentDialog(CWnd* pParent /*=NULL*/)
	: CDialog(EnviromnmentDialog::IDD, pParent)
{
	
}

EnviromnmentDialog::~EnviromnmentDialog()
{
}

void EnviromnmentDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER3, minimapsize);
	DDX_Control(pDX, IDC_SLIDER2, minimaptransparent);
	DDX_Control(pDX, IDC_SLIDER4, minimapiconsize);
	DDX_Control(pDX, IDC_SLIDER5, minimaptimer);
}


BEGIN_MESSAGE_MAP(EnviromnmentDialog, CDialog)
	ON_COMMAND(ID_32775, &EnviromnmentDialog::On32775)
	ON_BN_CLICKED(IDOK, &EnviromnmentDialog::OnBnClickedOk)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER3, &EnviromnmentDialog::mapSIze)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, &EnviromnmentDialog::mapTransparent)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER4, &EnviromnmentDialog::setIconSize)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER5, &EnviromnmentDialog::mapTimer)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// EnviromnmentDialog �޽��� ó�����Դϴ�.


void EnviromnmentDialog::On32775()
{
	
	
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void EnviromnmentDialog::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDialog::OnOK();
}


void EnviromnmentDialog::mapSIze(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	
	*pResult = 0;
}


void EnviromnmentDialog::mapTransparent(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}


void EnviromnmentDialog::setIconSize(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}


void EnviromnmentDialog::mapTimer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	
	
	*pResult = 0;
}


BOOL EnviromnmentDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	E_Global* e_global = E_Global::getSingleton();
	E_Map* e_map = E_Map::getSingleton();
	minimapsize.SetRange(2, 9);	//%��
	minimaptransparent.SetRange(10, 255);
	minimapiconsize.SetRange(5, 15); // /10�ؼ����
	minimaptimer.SetRange(1, 10);

	minimapsize.SetRangeMin(2);
	minimaptransparent.SetRangeMin(10);
	minimapiconsize.SetRangeMin(5);
	minimaptimer.SetRangeMin(1);

	minimapsize.SetRangeMax(9);
	minimaptransparent.SetRangeMax(255);
	minimapiconsize.SetRangeMax(15);
	minimaptimer.SetRangeMax(10);

	minimapsize.SetPos(e_global->getMapsize()*100);
	minimaptransparent.SetPos(e_map->getTransparent());
	minimapiconsize.SetPos(e_global->getIconsize()*10);
	minimaptimer.SetPos(e_map->getTimer());

	minimapsize.SetLineSize(1);
	minimaptransparent.SetLineSize(1);
	minimapiconsize.SetLineSize(1);
	minimaptimer.SetLineSize(1);

	minimapsize.SetPageSize(10);
	minimaptransparent.SetPageSize(10);
	minimapiconsize.SetPageSize(10);
	minimaptimer.SetPageSize(10);
	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void EnviromnmentDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	int a= pScrollBar->GetScrollPos();
	a = 10;
	//pScrollBar-
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
