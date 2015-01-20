#include "E_Window.h"
#include "E_Mutex.h"
using namespace Gdiplus;

#pragma once
class E_Desktop
{
private:
	int index;
	CBitmap screenshot;
	
public:
	list<E_Window*> windowList;	//������ �ִ� �����츮��Ʈ
	//list<E_Window*> onWindowList;			//Ȱ��ȭ�� ������
	void setAllIconInvisible();
	void setAllIconVisible();
	// ���â SW_SHOW
	void setAllShow();
	void setAllTransparent();
	void setAllTransParentExclude();
	void setAllMinimizeTransparent();
	void setAllOpaque();
	void setAllHide();


	void operator=(const E_Desktop &tmp);

	void insertWindow(E_Window* targetWindow);
	void removeWindow(E_Window* targetWindow);
	void excludeWindow(E_Window* targetWindow);
	void clearWindow();


	// �� ����
	void takeScreenshot();
	CBitmap getScreenshot();
	void takeAllWindowScreenshot();

	list<E_Window*> getWindowList();

	int getIndex();
	void setIndex(int newindex);


	E_Desktop(int index);
	~E_Desktop();
	// �����츦 ����Ʈ�� ����
	void setAllMinimize();
	void setAllRestore();
	void setAllSaveShowState();
	void setAllRestoreSavedShowState();
	void setAllNormalExclude();
};