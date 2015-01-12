#include "stdafx.h"
#include "E_DragAndDropSwitcher.h"
#include "E_Global.h"
#include "E_EnvironmentManager.h"

void E_DragAndDropSwitcher::updateSelectedDesktop()
{
	//������Ʈ�� �߻��� ��� �ڵ����� ȣ���
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
			nextDesktopIndex = (*itr_Desktop)->getIndex();		//�� ���ΰ���?? ȯ������Ʈ�ʿ�?
			break;
		}
		if (e_global->getSelectedIndex() == (*itr_Desktop)->getIndex())
			check = 1;			
		Create(szClassName, _T(""), WS_VISIBLE, CRect(enManager->getWidth()-100, 0, enManager->getWidth()*2-10, enManager->getHeight()), CWnd::GetDesktopWindow(), 1234); // nid ���� �� ã��.

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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CWnd::OnPaint()��(��) ȣ������ ���ʽÿ�.
}

int E_DragAndDropSwitcher::getNextDesktop()
{
	return nextDesktopIndex;
}