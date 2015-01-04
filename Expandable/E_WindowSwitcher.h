#pragma once
#include "afxwin.h"
#include "E_EnvironmentManager.h"
#include "E_AeroPeekController.h"
class E_WindowSwitcher :
	public CWnd
{
private:
	static E_WindowSwitcher* singleton;
	E_WindowSwitcher();
	~E_WindowSwitcher();
	E_EnvironmentManager* envManager;
	bool running;
public:
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
	// �ػ󵵿� ����� ������� �ڽ� ũ��
	int getAeroSize(int res_width);
	// �ڽ� ������ �ڽ� ũ�⸦ �̿��� ��ü Window�ʺ� ��ȯ
	int getAeroWindowWidth(int maxBoxCount, int boxwidth);
	// �ڽ� ������ �ʺ� �̿��� ���� ����
	int getAeroWindowHeight(int maxBoxCount, int boxwidth);
};

