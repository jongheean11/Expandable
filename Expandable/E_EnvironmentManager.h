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
	static const wchar_t* ExeFile;
	long getWidth();
	long getHeight();
	void E_EnvironmentManager::notifyChangeResolution();
	void E_EnvironmentManager::notifyDualMonitorMode();
	void setGlobalUpdater(E_GlobalUpdater* updater);
	// 이미 동작하는 Expandable이 있는지??
	static bool getRunningMainProcess();
	// 동작하는 핫키 프로그램이 있는지?
	static bool getRunningAutoHotkey();
	// updater를 매개변수로 받는 싱글톤 생성함수
	static E_EnvironmentManager* getSingleton();
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
};

