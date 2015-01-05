#include "E_Desktop.h"
#include "E_GlobalUpdater.h"

#pragma once
class E_Global : E_GlobalUpdater
{
private:
	int selectedDesktopIndex;
	int aeroMode;
	int updateState;
	
public:
	list<E_Desktop> desktopList;
	list<E_Window> dockedWindowList;
	list<HWND> windowList;

	static double resolutionWidth, resolutionHeight;
	static double virtualWidth, virtualHeight;
	/*static void setresolutionWidth(int paramWidth);
	static double getresolutionWidth();
	static void setresolutionHeight(int paramHeight);
	static double getresolutionHeight();
	static void setVirtualWidth(int paramWidth);
	static double getVirtualWidth();
	static void setVirtualHeight(int paramHeight);
	static double getVirtualHeight();*/
	void setSelectedDesktopIndex(int paramIndex);
	int getSelectedDesktopIndex();
	void startUpdateSelectedDesktop();
	void stopUpdateSelectedDesktop();
	
	CWnd* getBackgroundWindow();
	CWnd* getTaskbarWindow();
	static list<HWND> getAllWindows(); 
	static BOOL CALLBACK EnumCallBack(HWND hwnd, LPARAM lParam);

	virtual void OnDualMonitorMode(bool result);
private:
	static E_Global* singleton;
	E_Global();
	~E_Global();
public:
	static E_Global* getSingleton();
	// 카카오톡 핸들
	static CWnd* getKakaoWindow();
};