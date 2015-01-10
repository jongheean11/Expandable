#include "E_Desktop.h"
#include "E_GlobalUpdater.h"
#include "E_IGlobal.h"

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
	int updateMode;
	list<HWND> windowList;
	list<HWND> windowListForUpdate;	//������Ʈ�ÿ� ��� ������Ʈ �� �׻� �ʱ�ȭ �ʿ�
	static E_Global* singleton;
	int desktopwidth;
	int desktopheight;
	int desktopCount;
	E_Global();
	~E_Global();

public:
	static const wchar_t* testFrameName;

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
	int getDesktopWidth();
	int getDesktopHeight();
	// ����ũž�� �����´�.
	E_Desktop* getDesktop(int index);

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