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
	// 카카오톡 핸들
	static CWnd* getKakaoWindow();
	//
	// 현재 데스크탑 저장
	bool setSelectedDesktop();
	// 현재 데스크탑 반환
	E_Desktop* getSelectedDesktop();
	// 주기적 업데이트를 위한 타이머
	void startTimer();
	void onTimer();
	// 업데이트 시작
	bool startUpdate();
	// 업데이트 쓰레드
	void onUpdate();
	// 업데이트를 멈추는 함수 (스레드 플래그를 바꿔줌)
	void stopUpdate();
	bool getUpdateMode();
	// 리스트의 끝에 데스크탑 추가
	bool addOneDesktop();
	// 데스크탑 제거
	bool delDesktop();
	// 데스크탑 개수 반환
	int getDesktopCount();
	void setExpandableRunningFlag();
	bool getExpandableRunningFlag();
};