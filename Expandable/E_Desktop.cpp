#include "stdafx.h"
#include "E_Desktop.h"


E_Desktop::E_Desktop(int index){
	this->index = index;
}

E_Desktop::~E_Desktop()
{
	
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

list<E_Window*> E_Desktop::getWindowList()
{
	return windowList;
}

void E_Desktop::setAllIconInvisible()
{
	for (list<E_Window*>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		(*itr)->setIconInvisible((*itr)->getWindow());
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
	for (list<E_Window*>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		(*itr)->setIconVisible((*itr)->getWindow());
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
	for (list<E_Window*>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		(*itr)->setTransparent();
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
	for (list<E_Window*>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		(*itr)->setOpaque();
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
	windowList.push_back(targetWindow);
}

void E_Desktop::removeWindow(E_Window* targetWindow)
{
	for (list<E_Window*>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		if ((*itr)->getWindow() == targetWindow->getWindow())
		{
			delete (*itr);
			windowList.remove(*itr);
			break;
		}
	}
}


void E_Desktop::excludeWindow(E_Window* targetWindow)
{
	for (list<E_Window*>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		if ((*itr)->getWindow() == targetWindow->getWindow())
		{
			windowList.remove(*itr);
			break;
		}
	}
}

void E_Desktop::takeScreenshot()
{
	int nScreenWidth, nScreenHeight;

	// 스크린의 넓이와 높이를 받아오는 부분.
	nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Desktop의 DC를 얻어오는 곳이다.
	HDC hScreen = ::GetDC(NULL);
	// 스크린의 내용을 그릴 메모리 DC를 생성한다.
	HDC hMem = ::CreateCompatibleDC(hScreen);

	// 입력받은 DC의 Compatible DC에 이미지를 주어진 넓이와 높이로 생성한다.
	HBITMAP hbitmap = CreateCompatibleBitmap(hScreen,
		nScreenWidth,
		nScreenHeight);

	// 메모리 DC에 이미지를 그리기 위해 Bitmap Gdi Object를 선택한다.
	HGDIOBJ oldBitmap = ::SelectObject(hMem, hbitmap);

	// 메모리 DC에 스크린 이미지를 그린다.
	::BitBlt(hMem, 0, 0, nScreenWidth, nScreenHeight, hScreen, 0, 0, SRCCOPY);

	// 이 뒷부분은 단지 리소스를 해지하기 위한 소스코드이다.
	::SelectObject(hMem, oldBitmap);
	::ReleaseDC(NULL, hScreen);
	::DeleteDC(hMem);

	// 이전의 Bitmap 객체를 없엔다.
	if (screenshot.GetSafeHandle())
		screenshot.DeleteObject();

	// 캡춰된 Bitmap Handle을 CBitmap Object에 Attach한다.
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

// 모든창 SW_SHOW
void E_Desktop::setAllShow()
{
	for (list<E_Window*>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		(*itr)->setShow();
	}
}


CBitmap E_Desktop::getScreenshot()
{
	return CBitmap();
}


void E_Desktop::setAllHide()
{
	for (list<E_Window*>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		(*itr)->setHide();
	}
}


void E_Desktop::clearWindow()
{
	//객체 해제
	for (list<E_Window*>::iterator iter = windowList.begin(); iter != windowList.end(); iter++) {
		delete *iter;
	}
	windowList.clear();
}


