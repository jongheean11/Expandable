#include "E_Desktop.h"
#include "E_GlobalUpdater.h"
#include "E_IGlobal.h"
#include "E_Mutex.h"
#include "E_ISwitcherUpdator.h"
#include <hash_map>
/*
객체 생성 후
init()로 초기화 필요.
*/
#pragma once
#include "E_SettingManager.h"
class E_Global : E_GlobalUpdater, E_IGlobal
{
private:
	int selectedIndex;	//데스크탑 인덱스는 1부터 시작
	E_Desktop* selectedDesktop;
	bool updateMode;
	list<HWND> windowList;			//임시 사용 윈도우 리스트
	list<HWND> windowListForUpdate;	//업데이트시에 사용 업데이트 전 항상 초기화 필요
	list<HWND> windowListForZIndex;	//업데이트시에 사용 업데이트 전 항상 초기화 필요
	static E_Global* singleton;
	E_Global();
	~E_Global();
	//스레드
	thread* currentThread;
	std::unordered_map<std::thread::id, bool> thread_map;
	std::unordered_map<std::thread::id, bool> thread_inner_map;
	std::unordered_map<std::thread::id, std::thread*> thread_pmap;

	bool hotkeyinvalidate;
	DWORD parnetpid;
	//제외 윈도우
	static wchar_t* const excludeWindows[];
	
	//setting variables
	double mapsize;//
	double iconsize;//
	int transparent;//
	int settingTimer;//
public:

	int dockcount;
	DWORD pidforhideNotify;
	CWnd* notifyHwnd[5];
	int notifyAblePos[5];
	E_Window* twForHide;
	int nowActiveNotify;
	int nextActiveNotify;
	bool mapopen ;
	HWND hwnd_frame;
	bool gethotkey();
	void sethotkey(bool value);
	CWnd* hwnd_cwnd;


	void changeDesktop(int pastvalue, int newvalue);
	static const wchar_t* testFrameName;
	void setTimer(int value);
	int getTimer();
	void setTransparent(int value);
	int getTransparent();
	E_Window* backgroundWindow;

	list<E_Desktop*> desktopList;
	list<HWND> dockedWindowList;

	std::hash_map<HWND, int> hwnd_desk;


	bool ExpandableRunningFlag;

	//setting variables
	int desktopwidth;//
	int desktopheight;//
	int desktopCount;

	CWnd* getBackgroundWindow();
	CWnd* getTaskbarWindow();
	static list<HWND> getAllWindows();
	static list<HWND> getAllWindowsForZIndex();
	static BOOL CALLBACK EnumCallBack(HWND hwnd, LPARAM lParam);
	static BOOL CALLBACK EnumCallBackForZIndex(HWND hwnd, LPARAM lParam);
	static BOOL CALLBACK EnumHide(HWND hwnd, LPARAM lParam);
	static const int DLLINJECTIONTIMER;


	virtual void OnDualMonitorMode(bool result);
	//인터페이스 호출을 위한 함수
	E_ISwitcherUpdator* desktopSwitcher;
	E_ISwitcherUpdator* mapSwitcher;
	E_ISwitcherUpdator* dragSwitcher;
	E_ISwitcherUpdator* windowSwitcher;
public:

	static BOOL CALLBACK EnumShow(HWND hwnd, LPARAM lParam);
	double getIconsize();
	void setIconsize(double value);
	void setMapsize(double value);
	double getMapsize();
	static E_Global* getSingleton();
	// 카카오톡 핸들
	static CWnd* getKakaoWindow();
	// 현재 데스크탑 반환
	E_Desktop* getSelectedDesktop();
	//현재 선택된 인덱스를 바꾸면 선택된 데스크탑도 바뀐다.
	void setSelectedIndex(int index);
	int getSelectedIndex();
	// 데스크탑을 가져온다.
	E_Desktop* getDesktop(int index);
	// 주기적 업데이트를 위한 타이머
	void startTimer();
	void onTimer();
	// 업데이트 시작
	bool startUpdate();
	// 업데이트 쓰레드
	bool onUpdate();
	// 업데이트를 멈추는 함수 (스레드 플래그를 바꿔줌)
	bool stopUpdate();
	// 실시간으로 윈도우 업데이트
	void loopUpdate();
	bool getUpdateMode();
	// 리스트의 끝에 데스크탑 추가
	bool addOneDesktop();
	// 데스크탑 제거
	bool delDesktop();
	// 데스크탑 개수 반환
	int getDesktopCount();
	void setExpandableRunningFlag();
	bool getExpandableRunningFlag();

	// 생성자에서 초기화 하지 못하는 것들을 초기화 하는 함수
	void E_Global::init(E_ISwitcherUpdator* desktop, E_ISwitcherUpdator* map, E_ISwitcherUpdator* drag, E_ISwitcherUpdator* window);
	int getDesktopWidth();
	int getDesktopHeight();

	void moveTopWindowLeft();
	// 탑 윈도우를 오른쪽 데스크탑으로 전달
	void moveTopWindowRight();
	// 탑 윈도우를 아래로 데스크탑으로 전달
	void moveTopWindowDown();
	// 탑 윈도우를 위쪽 데스크탑으로 전달
	void moveTopWindowUp();
	// 왼쪽 데스크 탑으로 이동
	void moveDesktopLeft();
	// 오른쪽 데스크탑으로 이동
	void moveDesktopRight();
	// 위쪽 데스크탑으로 이동
	void moveDesktopUp();
	// 아래쪽 데스크탑으로 이동
	void moveDesktopDown();
	// 리스트를 Z-Index로 정렬해서 반환하는 함수
	void E_Global::sortZIndexWindowList(list<E_Window*>& target);
	void moveDesktop(int index);
};