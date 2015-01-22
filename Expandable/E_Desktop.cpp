#include "stdafx.h"
#include "E_Desktop.h"


E_Desktop::E_Desktop(int index){
	this->index = index;
}

E_Desktop::~E_Desktop()
{
	
}

//OnUpdate()와 동기화 테스트 통과
list<E_Window*> E_Desktop::getWindowList()
{
	lock_guard<mutex> updateMutext(E_Mutex::updateMutex);
	//TRACE_WIN32A("[E_Desktop::getWindowList] enter the thread");
	return windowList;
}

void E_Desktop::setAllIconInvisible()
{
	for (list<E_Window*>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		if (!(*itr)->dock)
		{
			(*itr)->setIconInvisible((*itr)->getWindow());
		}
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
	for (list<E_Window*>::reverse_iterator ritr = windowList.rbegin(); ritr != windowList.rend(); ritr++)
	{
		if (!(*ritr)->dock)
			(*ritr)->setIconVisible((*ritr)->getWindow());
	}
}

void E_Desktop::setAllTransParentExclude()
{
	for (list<E_Window*>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		if (!(*itr)->isAeroPossible()){
			(*itr)->setTransparent();	//투명
			//윈도우 상태 저장

			//(*itr)->setNormal();
		}
	}
}


void E_Desktop::setAllNormalExclude()
{

	for (list<E_Window*>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		if (!(*itr)->isAeroPossible()){
			/*char pname[255] = { 0, };
			GetWindowTextA((*itr)->getWindow(), pname, 255);
			TRACE_WIN32A("E_Desktop::setAllNormalExclude->%s", pname);*/
			(*itr)->setNormal();
		}
	}
}

//잔상 이슈 해결 종속적
void E_Desktop::setAllNormalExcludeRemoveDirty(list<char*> exclude)
{
	char pname[255] = { 0, };
	char* subpname = NULL;
	for (list<E_Window*>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		//비주얼 스튜디오 제외
		if (!(*itr)->isAeroPossible()){

			GetWindowTextA((*itr)->getWindow(), pname, 255);
			for (list<char*>::iterator iter = exclude.begin(); iter != exclude.end(); iter++) {
				subpname = strstr(pname, (*iter));
				if (subpname == NULL){	//Visual Studio 제외
					(*itr)->setNormal();

					//잔상 이슈 해결
					LONG_PTR tt = GetWindowLongPtr((*itr)->getWindow(), GWL_STYLE);
					if (tt & DS_3DLOOK)
					{
						//현재위치 저장
						RECT rect;
						GetWindowRect((*itr)->getWindow(), &rect);
						WINDOWPLACEMENT placement;
						GetWindowPlacement((*itr)->getWindow(), &placement);
						placement.rcNormalPosition = rect;
						placement.showCmd = SW_MINIMIZE;	//최소화로 예상하고 해결
						(*itr)->saveRect(placement);

						MoveWindow((*itr)->getWindow(), E_EnvironmentManager::getSingleton()->getWidth(), E_EnvironmentManager::getSingleton()->getHeight(), rect.right - rect.left, rect.bottom - rect.top, FALSE);
					}

					break;
				}
			}
		}
	}
}

void E_Desktop::removeDirtyAfter()
{
	for (list<E_Window*>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		if ((*itr)->getTPMode()){

			//잔상 이슈 해결
			LONG_PTR tt = GetWindowLongPtr((*itr)->getWindow(), GWL_STYLE);
			if (tt & DS_3DLOOK)
			{
				WINDOWPLACEMENT placement;
				(*itr)->loadRect(placement);
				placement.showCmd = (*itr)->savedState;
				SetWindowPlacement((*itr)->getWindow(), &placement);
			}

		}
	}
}

void E_Desktop::setAllMinimizeTransparentRemoveDirty()
{
	for (list<E_Window*>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		if ((*itr)->getTPMode()){
			(*itr)->setMinimize();
			(*itr)->setOpaque();

			//잔상 이슈 해결
			LONG_PTR tt = GetWindowLongPtr((*itr)->getWindow(), GWL_STYLE);
			if (tt & DS_3DLOOK)
			{
				WINDOWPLACEMENT placement;
				(*itr)->loadRect(placement);
				SetWindowPlacement((*itr)->getWindow(), &placement);
			}

		}
	}
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
	for (list<E_Window*>::reverse_iterator itr = windowList.rbegin(); itr != windowList.rend(); itr++)
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


void E_Desktop::takeAllWindowScreenshot()
{
}


void E_Desktop::setAllMinimize()
{
	E_Window::SetMinimizeMaximizeAnimation(false);
	for (list<E_Window*>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		ShowWindow((*itr)->getWindow(), SW_MINIMIZE);
	}
	E_Window::SetMinimizeMaximizeAnimation(true);
}


void E_Desktop::setAllRestore()
{

	for (list<E_Window*>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		(*itr)->setRestore();
	}
}


void E_Desktop::setAllSaveShowState()
{
	for (list<E_Window*>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		(*itr)->saveShowState();
	}
}


void E_Desktop::setAllRestoreSavedShowState()
{
	for (list<E_Window*>::iterator itr = windowList.begin(); itr != windowList.end(); itr++)
	{
		(*itr)->restoreShowState();
	}
}
