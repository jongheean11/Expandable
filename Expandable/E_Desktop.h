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
	list<E_Window*> windowList;	//가지고 있는 윈도우리스트
	//list<E_Window*> onWindowList;			//활성화된 윈도우
	void setAllIconInvisible();
	void setAllIconVisible();
	// 모든창 SW_SHOW
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


	// 미 구현
	void takeScreenshot();
	CBitmap getScreenshot();
	void takeAllWindowScreenshot();

	list<E_Window*> getWindowList();

	int getIndex();
	void setIndex(int newindex);


	E_Desktop(int index);
	~E_Desktop();
	// 윈도우를 리스트에 포함
	void setAllMinimize();
	void setAllRestore();
	void setAllSaveShowState();
	void setAllRestoreSavedShowState();
	void setAllNormalExclude();
};