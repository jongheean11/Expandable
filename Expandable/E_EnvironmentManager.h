#include "E_GlobalUpdater.h"

#pragma once
class E_EnvironmentManager : CWnd
{
private:
	static E_EnvironmentManager* singleton;
	E_EnvironmentManager(E_GlobalUpdater* updater);
	~E_EnvironmentManager();
	double width;
	double height;
	bool aeroPeekMode;
	bool dualMonitorMode;
	bool runningMainProcess;
	bool runningAutoHotkeyProcess;
	E_GlobalUpdater* globalUpdater;
public:
	static const wchar_t* ExeFile;
	void E_EnvironmentManager::notifyChangeResolution();
	void E_EnvironmentManager::notifyDualMonitorMode();
	// �̹� �����ϴ� Expandable�� �ִ���??
	static bool getRunningMainProcess();
	// �����ϴ� ��Ű ���α׷��� �ִ���?
	static bool getRunningAutoHotkey();
	// updater�� �Ű������� �޴� �̱��� �����Լ�
	static E_EnvironmentManager* getSingleton(E_GlobalUpdater* updater);
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
};

