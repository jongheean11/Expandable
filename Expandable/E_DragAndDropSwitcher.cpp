#include "stdafx.h"
#include "E_DragAndDropSwitcher.h"
#include "E_Global.h"
#include "E_EnvironmentManager.h"

void E_DragAndDropSwitcher::updateSelectedDesktop()
{
	//업데이트가 발생한 경우 자동으로 호출됨
	TRACE_WIN32A("[E_DragAndDropSwitcher::updateSelectedDesktop()]");
}

E_DragAndDropSwitcher::E_DragAndDropSwitcher()
{
}

E_DragAndDropSwitcher* E_DragAndDropSwitcher::singleton = NULL;

E_DragAndDropSwitcher::~E_DragAndDropSwitcher()
{
}

E_DragAndDropSwitcher* E_DragAndDropSwitcher::getSingleton()
{
	if (singleton == NULL) {
		singleton = new E_DragAndDropSwitcher();
	}
	return singleton;
}

void E_DragAndDropSwitcher::startSwitcher()
{
	E_Global* e_global = E_Global::getSingleton();
	E_EnvironmentManager* enManager = E_EnvironmentManager::getSingleton();
	CBrush m_oBkgndBrush;
	m_oBkgndBrush.CreateSolidBrush(RGB(255, 255, 255));
	UINT nClassStyle = CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	CString szClassName = AfxRegisterWndClass(nClassStyle, 0, (HBRUSH)m_oBkgndBrush.GetSafeHandle(), 0);

	std::list<E_Desktop*> desktopList = e_global->desktopList;
	int check = 0;
	for (std::list<E_Desktop*>::iterator itr_Desktop = desktopList.begin(); itr_Desktop != desktopList.end(); itr_Desktop++)
	{
		if (check)
		{
			nextDesktopIndex = (*itr_Desktop)->getIndex();		//맨 끝인경우는?? 환형리스트필요?
			break;
		}
		if (e_global->getSelectedIndex() == (*itr_Desktop)->getIndex())
			check = 1;			
		Create(szClassName, _T(""), WS_VISIBLE, CRect(enManager->getWidth()-100, 0, enManager->getWidth()*2-10, enManager->getHeight()), CWnd::GetDesktopWindow(), 1234); // nid 뭔지 꼭 찾기.

	}
	











}

void E_DragAndDropSwitcher::terminateSwitcher()
{

}

void E_DragAndDropSwitcher::OnResize()
{

}

void E_DragAndDropSwitcher::OnRMouseDown(int nx, int ny)
{

}

void E_DragAndDropSwitcher::OnRMouseMove(int nx, int ny)
{

}

void E_DragAndDropSwitcher::RmouseUp(int nx, int ny)
{

}
BEGIN_MESSAGE_MAP(E_DragAndDropSwitcher, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()


void E_DragAndDropSwitcher::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.
}

int E_DragAndDropSwitcher::getNextDesktop()
{
	return nextDesktopIndex;
}