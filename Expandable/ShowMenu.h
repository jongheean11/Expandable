#pragma once


// ShowMenu ��ȭ �����Դϴ�.

class ShowMenu : public CDialogEx
{
	DECLARE_DYNAMIC(ShowMenu)

public:
	ShowMenu(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~ShowMenu();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	
};
