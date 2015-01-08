#include "E_Util.h"

using namespace Gdiplus;
using namespace std;

#pragma once
class E_Window
{
private:
	HWND window;
	CBitmap icon;
	CBitmap screenshot;
	
	int zIndex;
	int windowState;
	int dockState;
	int alertState;

public:

	bool takeScreenshot();
	E_Window(HWND);
	~E_Window();

	static bool setIconInvisible(HWND hwnd);
	static bool setIconVisible(HWND hwnd);
	void setTransparent();
	void setOpaque();
	void setHide();
	void setShow();
	// ������ ����
	void setWindow(HWND window);
	HWND getWindow();

	void operator=(const E_Window &tmp);
	bool operator==(const E_Window &tmp);
	//�ּ�/�ִ�ȭ �ִϸ��̼� ���� �� Ȱ��ȭ
	void SetMinimizeMaximizeAnimation(bool status);
	CBitmap* getScreenshot();

	CBitmap* getIcon();
	// ������ Show ���¸� ������
	UINT getShowState();
	// ����ΰ� �������� �ƴ� �Լ�.
	bool isAeroPossible();
};