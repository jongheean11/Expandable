#pragma once
#include "afxwin.h"
class E_WindowSwitcher :
	public CWnd
{
private:
	static E_WindowSwitcher* singleton;
	E_WindowSwitcher();
	~E_WindowSwitcher();
public:
	static E_WindowSwitcher* getSingleton();
	// UI�� �����ְ� �Է��� �޴� â�� Ȱ��ȭ ��Ŵ
	void startSwitcher();
	// UI�� ������ â�� ������ �Լ�
	void terminateSwitcher();
	// ZOrderTop
	void setZOrderTop();
};

