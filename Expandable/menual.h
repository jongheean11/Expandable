#pragma once


// menual ��ȭ �����Դϴ�.

class menual : public CDialog
{
	DECLARE_DYNAMIC(menual)

public:
	menual(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~menual();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
