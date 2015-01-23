#pragma once
#include "E_AeroPeekController.h"
#include "E_Util.h"
using namespace Gdiplus;
using namespace std;

class E_Window
{
private:
	HWND window;
	CBitmap icon;
	CBitmap screenshot;
	char windowName[255];

	bool tpmode;	//���� ����ΰ�?
	WINDOWPLACEMENT savedRect;	//���� ��嶧 ������ ��ġ ����
public:
	unsigned int savedState;	//saveShowState�� ������ ����

	bool dock;
	E_Window(HWND);
	~E_Window();
	bool takeScreenshot();
	//�ּ�/�ִ�ȭ �ִϸ��̼� ���� �� Ȱ��ȭ
	static void SetMinimizeMaximizeAnimation(bool status);

	static bool setIconInvisible(HWND hwnd);
	static bool setIconVisible(HWND hwnd);
	void setTransparent();
	void setOpaque();
	void setHide();
	void setShow();
	void setNormal();
	void setNormal(RECT& rect);
	void setMinimize();
	// ������ ����
	void setWindow(HWND window);
	HWND getWindow();
	char* getWindowName();

	void operator=(const E_Window &tmp);
	bool operator==(const E_Window &tmp);
	CBitmap* getScreenshot();

	CBitmap* getIcon();
	// ������ Show ���¸� ������
	UINT getShowState();
	// ����ΰ� �������� �ƴ� �Լ�.
	bool isAeroPossible();
	bool getTPMode();
	// ������ �� ����
	void saveShowState();
	// ����� ���� ����
	void restoreShowState();
	void setRestore();
	// ���� �˻�� ��� (�Ϲ������� �ִ�ȭ ���� �˻�)
	bool isMaximized();

	void doTake(int state);
	void saveRect(WINDOWPLACEMENT& rect);
	void loadRect(WINDOWPLACEMENT& rect);
};