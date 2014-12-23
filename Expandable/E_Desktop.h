#include <gdiplus.h>
#include <afxwin.h>
#include "E_Window.h"
#include <list>
using namespace Gdiplus;


class E_Desktop
{
private:
	int index;
	list<E_Window> windowList;	//������ �ִ� �����츮��Ʈ
	list<E_Window> onWindowList;			//Ȱ��ȭ�� ������
	POSITION pos,old;
	E_Window* current;
	CBitmap screenshot;
	
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
	void takeScreenshot();
	int getIndex();
	void setIndex(int newindex);
	list<E_Window> getList();
	void setList();
	list<E_Window> getOnList();
	void setOnList();
	E_Desktop();
	E_Desktop(const E_Desktop &tmp);
	~E_Desktop();
};