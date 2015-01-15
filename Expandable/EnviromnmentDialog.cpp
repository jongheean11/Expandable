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
	mapSize = 0.06;
	iconSize = 1;
	transparent = 160;
	settingTimer = 5;
	swapflag = false;
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
	DDX_Control(pDX, IDC_COMBO1, mapwidth);
	DDX_Control(pDX, IDC_COMBO2, mapHeight);
}


BEGIN_MESSAGE_MAP(EnviromnmentDialog, CDialog)
	ON_COMMAND(ID_32775, &EnviromnmentDialog::On32775)
	ON_BN_CLICKED(IDOK, &EnviromnmentDialog::OnBnClickedOk)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER3, &EnviromnmentDialog::mapSIze)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, &EnviromnmentDialog::mapTransparent)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER4, &EnviromnmentDialog::setIconSize)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER5, &EnviromnmentDialog::mapTimer)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_COMBO2, &EnviromnmentDialog::mapHeightset)
	ON_CBN_SELCHANGE(IDC_COMBO1, &EnviromnmentDialog::mapWidthset)
END_MESSAGE_MAP()


// EnviromnmentDialog �޽��� ó�����Դϴ�.


void EnviromnmentDialog::On32775()
{
	
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void EnviromnmentDialog::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	E_Global* e_global = E_Global::getSingleton();
	E_Map* e_map = E_Map::getSingleton();
	e_global->setMapsize(mapSize);
	e_global->setTransparent(transparent);
	e_global->setTimer(settingTimer);
	e_global->setIconsize(iconSize);
	e_global->desktopwidth = mapwidth.GetCurSel() + 1;
	e_global->desktopheight = mapHeight.GetCurSel() + 1;
	if (swapflag)
	{
		e_global->desktopwidth = (mapwidth.GetCurSel() + 1);
		e_global->desktopheight = (mapHeight.GetCurSel() + 1);
		e_global->changeDesktop(pastdesktopnum, (mapwidth.GetCurSel() + 1) * (mapHeight.GetCurSel() + 1));
		e_map->terminateMap(); // �̰� �߾ȵ�
	}
	CDialog::OnOK();
}


void EnviromnmentDialog::mapSIze(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//E_Global* e_global = E_Global::getSingleton();
	//E_Map* e_map = E_Map::getSingleton();
	mapSize = minimapsize.GetPos();
	//e_global->setMapsize(mapSize);
	///::SendMessage(e_map->hwnd, WM_USER_EVENT, 0, 0);
	*pResult = 0;
}

void EnviromnmentDialog::mapTransparent(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	transparent = minimaptransparent.GetPos();

	*pResult = 0;
}


void EnviromnmentDialog::setIconSize(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	iconSize = minimapiconsize.GetPos();

	
	*pResult = 0;
}


void EnviromnmentDialog::mapTimer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	settingTimer = minimaptimer.GetPos();
	
	
	*pResult = 0;
}


BOOL EnviromnmentDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	E_Global* e_global = E_Global::getSingleton();
	E_Map* e_map = E_Map::getSingleton();
	
	pastdesktopnum = e_global->desktopwidth * e_global->desktopheight;

	mapwidth.AddString(_T("1"));
	mapwidth.AddString(_T("2"));
	mapwidth.AddString(_T("3"));
	mapwidth.AddString(_T("4"));
	mapHeight.AddString(_T("1"));
	mapHeight.AddString(_T("2"));
	mapHeight.AddString(_T("3"));
	mapHeight.AddString(_T("4"));

	mapwidth.SetCurSel(e_global->desktopwidth - 1);
	mapHeight.SetCurSel(e_global->desktopheight - 1);
	
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

	minimapsize.SetPos(e_global->getMapsize() * 100);
	minimaptransparent.SetPos(e_global->getTransparent());
	minimapiconsize.SetPos(e_global->getIconsize() * 10);
	minimaptimer.SetPos(e_global->getTimer());

	minimapsize.SetLineSize(1);
	minimaptransparent.SetLineSize(1);
	minimapiconsize.SetLineSize(1);
	minimaptimer.SetLineSize(1);

	minimapsize.SetPageSize(1);
	minimaptransparent.SetPageSize(20);
	minimapiconsize.SetPageSize(1);
	minimaptimer.SetPageSize(1);
	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

//
//void EnviromnmentDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
//{
//	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
//	int a= pScrollBar->GetScrollPos();
//	a = 10;
//	//pScrollBar-
//	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
//}

void EnviromnmentDialog::mapHeightset()
{
	E_Global* e_global = E_Global::getSingleton();
	swapflag = true;
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void EnviromnmentDialog::mapWidthset()
{
	E_Global* e_global = E_Global::getSingleton();
	swapflag = true;
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}
