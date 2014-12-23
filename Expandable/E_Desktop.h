#include<gdiplus.h>
#include<afxtempl.h>
#include<afxwin.h>
#include"E_Window.h"
using namespace Gdiplus;


class E_Desktop
{
private:
	int index;
	CList<E_Window> windowList;				//가지고 있는 윈도우리스트
	CList<E_Window> onWindowList;			//활성화된 윈도우
	POSITION pos,old;
	E_Window* current;
	
public:
	void setAllIconInvisible();
	void setAllIconVisible();
	void setAllTransparent();
	void setAllOpaque();
	void operator=(const E_Desktop &tmp);
	void insertWindow(E_Window* targetWindow);
	void removeWindow(E_Window* targetWindow);
	void drawAddedWindow(E_Window* selectedWindow, Rect* area);
	void drawRemoveWindow(E_Window* selectedWindow, Rect* area);
	CBitmap takeScreenshot();
	int getIndex();
	void setIndex(int newindex);
	CList<E_Window> getList();
	void setList();
	E_Desktop();
	//E_Desktop(const E_Desktop &tmp);
	~E_Desktop();

};