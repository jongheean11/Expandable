#pragma once
#include "afxwin.h"
#include "E_EnvironmentManager.h"
class E_WindowSwitcher :
	public CWnd
{
private:
	static E_WindowSwitcher* singleton;
	E_WindowSwitcher();
	~E_WindowSwitcher();
	E_EnvironmentManager* envManager;
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
};

