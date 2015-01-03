#include "E_Desktop.h"
#include "E_GlobalUpdater.h"

#pragma once
class E_Global : E_GlobalUpdater
{
private:
	int selectedDesktopIndex;
	int aeroMode;
	int updateState;
	list<E_Desktop> desktopList;
	list<E_Window> dockedWindowList;
public:
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

	virtual void OnDualMonitorMode(bool result);
private:
	static E_Global* singleton;
	E_Global();
	~E_Global();
public:
	static E_Global* getSingleton();
};