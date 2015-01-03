#include "E_GlobalUpdater.h"

#pragma once
class E_EnvironmentManager : CWnd
{
private:
	static E_EnvironmentManager* singleton;
	E_EnvironmentManager();
	~E_EnvironmentManager();
	long width;
	long height;
	bool aeroPeekMode;
	bool dualMonitorMode;
	bool runningMainProcess;
	bool runningAutoHotkeyProcess;
	E_GlobalUpdater* globalUpdater;
public:
	static const wchar_t* ExeFileName;
	long getWidth();
	long getHeight();
	void setGlobalUpdater(E_GlobalUpdater* updater);
	// �̹� �����ϴ� Expandable�� �ִ���??
	static bool getRunningMainProcess();
	// �����ϴ� ��Ű ���α׷��� �ִ���?
	static bool getRunningAutoHotkey();
	// updater�� �Ű������� �޴� �̱��� �����Լ�
	static E_EnvironmentManager* getSingleton();
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
};

