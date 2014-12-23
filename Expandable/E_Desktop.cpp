#include "stdafx.h"
#include"E_Desktop.h"


void E_Desktop::setAllIconInvisible()
{
	POSITION pos = windowList.GetHeadPosition();
	for (int i = 0; i < windowList.GetCount(); i++)
	{
		E_Window* current = &windowList.GetNext(pos);
		current->setIconInvisible();
	}
}

void E_Desktop::setAllIconVisible()
{
	POSITION pos = windowList.GetHeadPosition();
	for (int i = 0; i < windowList.GetCount(); i++)
	{
		E_Window* current = &windowList.GetNext(pos);
		current->setIconVisible();
	}
}

void E_Desktop::setAllTransparent()
{
	POSITION pos = windowList.GetHeadPosition();
	for (int i = 0; i < windowList.GetCount(); i++)
	{
		E_Window* current = &windowList.GetNext(pos);
		current->setTransparent();
	}
}

void E_Desktop::setAllOpaque()
{
	pos = windowList.GetHeadPosition();
	for (int i = 0; i < windowList.GetCount(); i++)
	{
		current = &windowList.GetNext(pos);
		current->setOpaque();
	}
}

void E_Desktop::insertWindow(E_Window* targetWindow)
{
	windowList.AddTail(*targetWindow);
	
}

void E_Desktop::removeWindow(E_Window* targetWindow)
{
	pos = windowList.GetHeadPosition();
	while (pos != NULL)
	{
		old = pos;
		current = &windowList.GetNext(pos);
		if (current->getHWND() == targetWindow->getHWND())
			windowList.RemoveAt(old);
	}	
}
void E_Desktop::drawAddedWindow(E_Window* selectedWindow, Rect* area)
{
	
	
}

void E_Desktop::drawRemoveWindow(E_Window* selectedWindow, Rect* area)
{
	
}

CBitmap E_Desktop::takeScreenshot()				//???????????????????????
{
	int nScreenWidth, nScreenHeight;
	CBitmap screenShot;

	// ��ũ���� ���̿� ���̸� �޾ƿ��� �κ�.
	nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Desktop�� DC�� ������ ���̴�.
	HDC hScreen = ::GetDC(NULL);
	// ��ũ���� ������ �׸� �޸� DC�� �����Ѵ�.
	HDC hMem = ::CreateCompatibleDC(hScreen);

	// �Է¹��� DC�� Compatible DC�� �̹����� �־��� ���̿� ���̷� �����Ѵ�.
	HBITMAP hbitmap = CreateCompatibleBitmap(hScreen,
		nScreenWidth,
		nScreenHeight);

	// �޸� DC�� �̹����� �׸��� ���� Bitmap Gdi Object�� �����Ѵ�.
	HGDIOBJ oldBitmap = ::SelectObject(hMem, hbitmap);

	// �޸� DC�� ��ũ�� �̹����� �׸���.
	::BitBlt(hMem, 0, 0, nScreenWidth, nScreenHeight, hScreen, 0, 0, SRCCOPY);

	// �� �޺κ��� ���� ���ҽ��� �����ϱ� ���� �ҽ��ڵ��̴�.
	::SelectObject(hMem, oldBitmap);
	::ReleaseDC(NULL, hScreen);
	::DeleteDC(hMem);

	// ������ Bitmap ��ü�� ������.
	if (screenShot.GetSafeHandle())
		screenShot.DeleteObject();

	// ĸ��� Bitmap Handle�� CBitmap Object�� Attach�Ѵ�.
	screenShot.Attach(hbitmap);
	return screenShot;
}

int E_Desktop::getIndex()
{
	return index;
}

void E_Desktop::setIndex(int newindex)
{
	index = newindex;
}
/*
CList<E_Window> E_Desktop::getList()
{
	return windowList;
}*/

void E_Desktop::setList()
{

}
/*
E_Desktop::E_Desktop(const E_Desktop &tmp)
{
	index = tmp.index;
	//windowList = tmp.windowList;
	//onWindowList = tmp.onWindowList;
	pos = tmp.pos;
	old = tmp.old;
	current = tmp.current;
}*/
/*
void E_Desktop::operator=(const E_Desktop &tmp)
{
	index = tmp.index;
	//windowList = tmp.windowList;
	//onWindowList = tmp.onWindowList;
	pos = tmp.pos;
	old = tmp.old;
	current = tmp.current;;
}
*/