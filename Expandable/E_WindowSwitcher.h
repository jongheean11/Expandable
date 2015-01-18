#pragma once
#include "afxwin.h"
#include "E_EnvironmentManager.h"
#include "E_AeroPeekController.h"
#include "E_ISwitcherUpdator.h"

enum SHAPE{VERTICAL, HORIZONTAL};
enum DRAWMODE{DRAW_NORMAL, UPDATE_TOUCH, UPDATE_TAB, DONT_DRAW};
enum GROUP2{SELECTEDDESKTOP, OTHERDESKTOP};
class E_WindowSwitcher : public CWnd, public E_ISwitcherUpdator
{
private:
	static E_WindowSwitcher* singleton;
	E_WindowSwitcher();
	~E_WindowSwitcher();
	E_EnvironmentManager* envManager;
	bool running;
	bool updateFlag;
	int tabIndex;
	list<HTHUMBNAIL> thumb_list;
	unordered_map<HWND, HTHUMBNAIL> thumb_map;	// 썸네일
	unordered_map<HWND, RECT> rect_map;	//	rect
	unordered_map<HWND, DRAWMODE> mode_map;	//	그리기 모드 ( 업데이트 인지? )
	unordered_map<HWND, GROUP2> group_map;	//	속해 있는 그룹
	unordered_map<HWND, E_Desktop*> desktop_map;	//	속해있는 데스크탑

protected:
	

public:
	virtual void updateSelectedDesktop();
	const static COLORREF backgroundColor;
	const static COLORREF aeroColor;
	const static COLORREF aeroColorSelected;
	const static COLORREF aeroColorSelectedMouse;
	const static COLORREF borderColor;
	const static COLORREF borderColorSelected;
	const static COLORREF borderColorSelectedMouse;
	static const wchar_t* caption;

	static E_WindowSwitcher* getSingleton();
	// UI를 보여주고 입력을 받는 창을 활성화 시킴
	void startSwitcher();
	// UI를 없에고 창을 가리는 함수
	void terminateSwitcher();
	// ZOrderTop
	void setZOrderTop();

	// 해상도에 대비한 아이콘의 크기를 반환
	int getIconSize(int res_width);
	// 해상도에 비례한 에어로픽 너비 크기
	int getAeroWidthSize(int res_width);
	// 해상도에 비례한 에어로픽 높이 크기
	int getAeroHeightSize(int res_width);
	// 박스 개수와 박스 크기를 이용한 전체 Window너비 반환
	int getAeroWindowWidth(int maxBoxCount, int boxwidth);
	// 박스 개수와 너비를 이용해 높이 산출
	int getAeroWindowHeight(int maxBoxCount, int boxwidth);
	// 레이아웃에서 사용되는 패딩의 크기를 반환받음
	int getPaddingSize(int res_width);
	// 창의 모양을 알 수 있다.
	SHAPE E_WindowSwitcher::getShape(int width, int height, int res_width, int res_height);
	//void E_WindowSwitcher::drawIcon();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	// 현재 Switcher가 동작중인가?
public:
	bool isRunning();
	HTHUMBNAIL temp;
	HTHUMBNAIL temp2;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	// 스위처를 재시작하는 함수
	void restartSwitcher();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};

