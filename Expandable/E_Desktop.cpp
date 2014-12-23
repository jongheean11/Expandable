#include "stdafx.h"
#include"E_Desktop.h"

E_Desktop::E_Desktop(const E_Desktop &tmp)
{
	index = tmp.index;
	//windowList = tmp.windowList;
	//onWindowList = tmp.onWindowList;
	pos = tmp.pos;
	old = tmp.old;
	current = tmp.current;
}

void E_Desktop::operator=(const E_Desktop &tmp)
{
	index = tmp.index;
	//windowList = tmp.windowList;
	//onWindowList = tmp.onWindowList;
	pos = tmp.pos;
	old = tmp.old;
	current = tmp.current;;
}

list<E_Window> E_Desktop::getList()
{
	return windowList;
}

void E_Desktop::setList()
{

}

list<E_Window> E_Desktop::getOnList()
{
	return onWindowList;
}

void E_Desktop::setOnList()
{

}

void E_Desktop::setAllIconInvisible()
{
	for (list<E_Window>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		(*itr).setIconInvisible();
	}
	/*
	POSITION pos = windowList.GetHeadPosition();
	for (int i = 0; i < windowList.GetCount(); i++)
	{
		E_Window* current = &windowList.GetNext(pos);
		current->setIconInvisible();
	}*/
}

void E_Desktop::setAllIconVisible()
{
	for (list<E_Window>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		(*itr).setIconVisible();
	}
	/*
	POSITION pos = windowList.GetHeadPosition();
	for (int i = 0; i < windowList.GetCount(); i++)
	{
		E_Window* current = &windowList.GetNext(pos);
		current->setIconVisible();
	}*/
}

void E_Desktop::setAllTransparent()
{
	for (list<E_Window>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		(*itr).setTransparent();
	}
	/*POSITION pos = windowList.GetHeadPosition();
	for (int i = 0; i < windowList.GetCount(); i++)
	{
		E_Window* current = &windowList.GetNext(pos);
		current->setTransparent();
	}*/
}

void E_Desktop::setAllOpaque()
{
	for (list<E_Window>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		(*itr).setOpaque();
	}
	/*
	pos = windowList.GetHeadPosition();
	for (int i = 0; i < windowList.GetCount(); i++)
	{
		current = &windowList.GetNext(pos);
		current->setOpaque();
	}*/
}

void E_Desktop::insertWindow(E_Window* targetWindow)
{
	windowList.push_back(*targetWindow);
}

void E_Desktop::removeWindow(E_Window* targetWindow)
{
	for (list<E_Window>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		if ((*itr).getHWND() == targetWindow->getHWND())
		{
			windowList.remove(*itr);
			break;
		}
	}
	/*
	pos = windowList.GetHeadPosition();
	while (pos != NULL)
	{
		old = pos;
		current = &windowList.GetNext(pos);
		if (current->getHWND() == targetWindow->getHWND())
			windowList.RemoveAt(old);
	}*/	
}

void E_Desktop::drawAddedWindow(E_Window* selectedWindow, Rect* area)
{
	
	
}

void E_Desktop::drawRemoveWindow(E_Window* selectedWindow, Rect* area)
{
	
}

void E_Desktop::takeScreenshot()
{
	int nScreenWidth, nScreenHeight;

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
	if (screenshot.GetSafeHandle())
		screenshot.DeleteObject();

	// ĸ��� Bitmap Handle�� CBitmap Object�� Attach�Ѵ�.
	screenshot.Attach(hbitmap);
}

int E_Desktop::getIndex()
{
	return index;
}

void E_Desktop::setIndex(int newindex)
{
	index = newindex;
}

