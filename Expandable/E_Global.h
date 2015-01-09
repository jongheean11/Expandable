#include "E_Desktop.h"
#include "E_GlobalUpdater.h"

/*
��ü ���� ��
init()�� �ʱ�ȭ �ʿ�.
*/
#pragma once
class E_Global : E_GlobalUpdater
{
private:
	int selectedIndex;	//����ũž �ε����� 1���� ����
	E_Desktop* selectedDesktop;
	int updateMode;
	list<HWND> windowList;
	static E_Global* singleton;
	
	E_Global();
	~E_Global();
	
public: 
	static const wchar_t* testFrameName;
	int desktopwidth, desktopheight;
	list<E_Desktop*> desktopList;
	list<E_Window*> dockedWindowList;
	bool ExpandableRunningFlag;
	
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
	int getSelectedIndex();
	//���� ���õ� �ε����� �ٲٸ� ���õ� ����ũž�� �ٲ��.
	void setSelectedIndex(int index);
	// �����ڿ��� �ʱ�ȭ ���� ���ϴ� �͵��� �ʱ�ȭ �ϴ� �Լ�
	void init();
};