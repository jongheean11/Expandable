#include "E_Desktop.h"
#include "E_GlobalUpdater.h"
#include "E_IGlobal.h"

/*
객체 생성 후
init()로 초기화 필요.
*/
#pragma once
class E_Global : E_GlobalUpdater, E_IGlobal
{
private:
	int selectedIndex;	//데스크탑 인덱스는 1부터 시작
	E_Desktop* selectedDesktop;
	bool updateMode;
	list<HWND> windowList;			//임시 사용 윈도우 리스트
	list<HWND> windowListForUpdate;	//업데이트시에 사용 업데이트 전 항상 초기화 필요
	static E_Global* singleton;
	E_Global();
	~E_Global();

public:
	static const wchar_t* testFrameName;

	list<E_Desktop*> desktopList;
	list<E_Window*> dockedWindowList;
	bool ExpandableRunningFlag;
	int desktopwidth;
	int desktopheight;
	int desktopCount;

	CWnd* getBackgroundWindow();
	CWnd* getTaskbarWindow();
	static list<HWND> getAllWindows();
	static BOOL CALLBACK EnumCallBack(HWND hwnd, LPARAM lParam);

	virtual void OnDualMonitorMode(bool result);
public:
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
	// 생성자에서 초기화 하지 못하는 것들을 초기화 하는 함수
	void init();
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
};