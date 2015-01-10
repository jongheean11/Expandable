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
	list<E_Window*> windowList;	//가지고 있는 윈도우리스트
	//list<E_Window*> onWindowList;			//활성화된 윈도우
	void setAllIconInvisible();
	void setAllIconVisible();
	// 모든창 SW_SHOW
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