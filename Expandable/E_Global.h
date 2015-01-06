#include "E_Desktop.h"
#include "E_GlobalUpdater.h"

#pragma once
class E_Global : E_GlobalUpdater
{
private:
	E_Desktop* selectedDesktop;
	int updateMode;
	list<HWND> windowList;
	static E_Global* singleton;
	E_Global();
	~E_Global();
	
public:
	list<E_Desktop> desktopList;
	list<E_Window> dockedWindowList;
	bool ExpandableRunningFlag;
	
	
	//static double resolutionWidth, resolutionHeight;
	//static double virtualWidth, virtualHeight;
	///*static void setresolutionWidth(int paramWidth);
	//static double getresolutionWidth();
	//static void setresolutionHeight(int paramHeight);
	//static double getresolutionHeight();
	//static void setVirtualWidth(int paramWidth);
	//static double getVirtualWidth();
	//static void setVirtualHeight(int paramHeight);
	//static double getVirtualHeight();*/
	//void setSelectedDesktopIndex(int paramIndex);
	//int getSelectedDesktopIndex();
	//void startUpdateSelectedDesktop();
	//void stopUpdateSelectedDesktop();
	
	CWnd* getBackgroundWindow();
	CWnd* getTaskbarWindow();
	static list<HWND> getAllWindows(); 
	static BOOL CALLBACK EnumCallBack(HWND hwnd, LPARAM lParam);

	virtual void OnDualMonitorMode(bool result);
public:
	static E_Global* getSingleton();
	// īī���� �ڵ�
	static CWnd* getKakaoWindow();
	//
	// ���� ����ũž ����
	bool setSelectedDesktop();
	// ���� ����ũž ��ȯ
	E_Desktop* getSelectedDesktop();
	// �ֱ��� ������Ʈ�� ���� Ÿ�̸�
	void startTimer();
	void onTimer();
	// ������Ʈ ����
	bool startUpdate();
	// ������Ʈ ������
	void onUpdate();
	// ������Ʈ�� ���ߴ� �Լ� (������ �÷��׸� �ٲ���)
	void stopUpdate();
	bool getUpdateMode();
	// ����Ʈ�� ���� ����ũž �߰�
	bool addOneDesktop();
	// ����ũž ����
	bool delDesktop();
	// ����ũž ���� ��ȯ
	int getDesktopCount();
	void setExpandableRunningFlag();
	bool getExpandableRunningFlag();
};