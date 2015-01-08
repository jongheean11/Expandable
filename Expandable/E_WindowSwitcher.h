#pragma once
#include "afxwin.h"
#include "E_EnvironmentManager.h"
#include "E_AeroPeekController.h"

enum SHAPE{ VERTICAL, HORIZONTAL };

class E_WindowSwitcher :
	public CWnd
{
private:
	static E_WindowSwitcher* singleton;
	E_WindowSwitcher();
	~E_WindowSwitcher();
	E_EnvironmentManager* envManager;
	bool running;
	list<HTHUMBNAIL> thumb_list;
	unordered_map<HWND, HTHUMBNAIL> thumb_map;
	
public:
	static const wchar_t* caption;
	static E_WindowSwitcher* getSingleton();
	// UI�� �����ְ� �Է��� �޴� â�� Ȱ��ȭ ��Ŵ
	void startSwitcher();
	// UI�� ������ â�� ������ �Լ�
	void terminateSwitcher();
	// ZOrderTop
	void setZOrderTop();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	// ���� Switcher�� �������ΰ�?
public:
	bool isRunning();
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
	HTHUMBNAIL temp;
	HTHUMBNAIL temp2;
	// â�� ����� �� �� �ִ�.
	SHAPE E_WindowSwitcher::getShape(int width, int height, int res_width, int res_height);
	void E_WindowSwitcher::drawIcon();
};

