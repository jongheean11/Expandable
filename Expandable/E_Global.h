#include "E_Desktop.h"
#include "E_GlobalUpdater.h"
#include "E_IGlobal.h"
#include "E_Mutex.h"

/*
��ü ���� ��
init()�� �ʱ�ȭ �ʿ�.
*/
#pragma once
class E_Global : E_GlobalUpdater, E_IGlobal
{
private:
	int selectedIndex;	//����ũž �ε����� 1���� ����
	E_Desktop* selectedDesktop;
	bool updateMode;
	list<HWND> windowList;			//�ӽ� ��� ������ ����Ʈ
	list<HWND> windowListForUpdate;	//������Ʈ�ÿ� ��� ������Ʈ �� �׻� �ʱ�ȭ �ʿ�
	static E_Global* singleton;
	E_Global();
	~E_Global();

	//������
	thread* currentThread;
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
	// īī���� �ڵ�
	static CWnd* getKakaoWindow();
	// ���� ����ũž ��ȯ
	E_Desktop* getSelectedDesktop();
	//���� ���õ� �ε����� �ٲٸ� ���õ� ����ũž�� �ٲ��.
	void setSelectedIndex(int index);
	int getSelectedIndex();
	// ����ũž�� �����´�.
	E_Desktop* getDesktop(int index);
	// �ֱ��� ������Ʈ�� ���� Ÿ�̸�
	void startTimer();
	void onTimer();
	// ������Ʈ ����
	bool startUpdate();
	// ������Ʈ ������
	void onUpdate();
	// ������Ʈ�� ���ߴ� �Լ� (������ �÷��׸� �ٲ���)
	bool stopUpdate();
	// �ǽð����� ������ ������Ʈ
	void loopUpdate();
	bool getUpdateMode();
	// ����Ʈ�� ���� ����ũž �߰�
	bool addOneDesktop();
	// ����ũž ����
	bool delDesktop();
	// ����ũž ���� ��ȯ
	int getDesktopCount();
	void setExpandableRunningFlag();
	bool getExpandableRunningFlag();
	// �����ڿ��� �ʱ�ȭ ���� ���ϴ� �͵��� �ʱ�ȭ �ϴ� �Լ�
	void init();
	int getDesktopWidth();
	int getDesktopHeight();

	void moveTopWindowLeft();
	// ž �����츦 ������ ����ũž���� ����
	void moveTopWindowRight();
	// ž �����츦 �Ʒ��� ����ũž���� ����
	void moveTopWindowDown();
	// ž �����츦 ���� ����ũž���� ����
	void moveTopWindowUp();
	// ���� ����ũ ž���� �̵�
	void moveDesktopLeft();
	// ������ ����ũž���� �̵�
	void moveDesktopRight();
	// ���� ����ũž���� �̵�
	void moveDesktopUp();
	// �Ʒ��� ����ũž���� �̵�
	void moveDesktopDown();
};