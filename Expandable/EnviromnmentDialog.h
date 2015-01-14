#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// EnviromnmentDialog 대화 상자입니다.
#define WM_USER_EVENT (WM_USER + 2)
#define WM_TRAY_ICON_NOTIFY_MESSAGE ( WM_USER+1)
class EnviromnmentDialog : public CDialog
{
	DECLARE_DYNAMIC(EnviromnmentDialog)

public:
	EnviromnmentDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~EnviromnmentDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG1 };
	NOTIFYICONDATA notify_icon_data;
	int transparent;
	double iconSize;
	double mapSize;
	int settingTimer;
	bool swapflag;
	int pastdesktopnum;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void On32775();
	afx_msg void OnBnClickedOk();
	afx_msg void mapSIze(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void mapTransparent(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void setIconSize(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void mapTimer(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl minimapsize;
	CSliderCtrl minimaptransparent;
	CSliderCtrl minimapiconsize;
	CSliderCtrl minimaptimer;
	virtual BOOL OnInitDialog();
	//afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CComboBox mapwidth;
	CComboBox mapHeight;
	afx_msg void mapHeightset();
	afx_msg void mapWidthset();
};
