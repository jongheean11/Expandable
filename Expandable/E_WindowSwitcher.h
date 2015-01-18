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
	unordered_map<HWND, HTHUMBNAIL> thumb_map;	// �����
	unordered_map<HWND, RECT> rect_map;	//	rect
	unordered_map<HWND, DRAWMODE> mode_map;	//	�׸��� ��� ( ������Ʈ ����? )
	unordered_map<HWND, GROUP2> group_map;	//	���� �ִ� �׷�
	unordered_map<HWND, E_Desktop*> desktop_map;	//	�����ִ� ����ũž

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
	// UI�� �����ְ� �Է��� �޴� â�� Ȱ��ȭ ��Ŵ
	void startSwitcher();
	// UI�� ������ â�� ������ �Լ�
	void terminateSwitcher();
	// ZOrderTop
	void setZOrderTop();

	// �ػ󵵿� ����� �������� ũ�⸦ ��ȯ
	int getIconSize(int res_width);
	// �ػ󵵿� ����� ������� �ʺ� ũ��
	int getAeroWidthSize(int res_width);
	// �ػ󵵿� ����� ������� ���� ũ��
	int getAeroHeightSize(int res_width);
	// �ڽ� ������ �ڽ� ũ�⸦ �̿��� ��ü Window�ʺ� ��ȯ
	int getAeroWindowWidth(int maxBoxCount, int boxwidth);
	// �ڽ� ������ �ʺ� �̿��� ���� ����
	int getAeroWindowHeight(int maxBoxCount, int boxwidth);
	// ���̾ƿ����� ���Ǵ� �е��� ũ�⸦ ��ȯ����
	int getPaddingSize(int res_width);
	// â�� ����� �� �� �ִ�.
	SHAPE E_WindowSwitcher::getShape(int width, int height, int res_width, int res_height);
	//void E_WindowSwitcher::drawIcon();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	// ���� Switcher�� �������ΰ�?
public:
	bool isRunning();
	HTHUMBNAIL temp;
	HTHUMBNAIL temp2;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	// ����ó�� ������ϴ� �Լ�
	void restartSwitcher();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};

