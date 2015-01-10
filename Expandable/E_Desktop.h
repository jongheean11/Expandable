#include "E_Window.h"
using namespace Gdiplus;

#pragma once
class E_Desktop
{
private:
	int index;
	POSITION pos,old;
	E_Window* current;
	CBitmap screenshot;
	
public:
	list<E_Window*> windowList;	//������ �ִ� �����츮��Ʈ
	//list<E_Window*> onWindowList;			//Ȱ��ȭ�� ������
	void setAllIconInvisible();
	void setAllIconVisible();
	// ���â SW_SHOW
	void setAllShow();
	void setAllTransparent();
	void setAllOpaque();

	void operator=(const E_Desktop &tmp);

	void insertWindow(E_Window* targetWindow);
	void removeWindow(E_Window* targetWindow);
	void excludeWindow(E_Window* targetWindow);
	void clearWindow();

	void takeScreenshot();

	int getIndex();
	void setIndex(int newindex);

	list<E_Window*> getWindowList();

	E_Desktop(int index);
	~E_Desktop();
	CBitmap getScreenshot();
	void setAllHide();
};