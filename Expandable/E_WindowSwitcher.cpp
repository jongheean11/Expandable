#include "stdafx.h"
#include "E_WindowSwitcher.h"


E_WindowSwitcher* E_WindowSwitcher::singleton = NULL;

E_WindowSwitcher::E_WindowSwitcher()
{
	envManager = E_EnvironmentManager::getSingleton();
}


E_WindowSwitcher::~E_WindowSwitcher()
{

}


E_WindowSwitcher* E_WindowSwitcher::getSingleton()
{
	if (singleton == NULL)
		singleton = new E_WindowSwitcher();
	return singleton;
}


// UI�� �����ְ� �Է��� �޴� â�� Ȱ��ȭ ��Ŵ
void E_WindowSwitcher::startSwitcher()
{
	this->ShowWindow(SW_SHOWMAXIMIZED);
}


// UI�� ������ â�� ������ �Լ�
void E_WindowSwitcher::terminateSwitcher()
{
	this->ShowWindow(SW_HIDE);
}


// ZOrderTop
void E_WindowSwitcher::setZOrderTop()
{
}
BEGIN_MESSAGE_MAP(E_WindowSwitcher, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()


void E_WindowSwitcher::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	static long resWidth = envManager->getWidth();
	static long resHeight = envManager->getWidth();
	//�׸��� ������ ���� ũ�⸦ �����Ͽ� ��� ������ ��
	static int top = resWidth / 4;
	static int left = resHeight / 4;
	static int width = resWidth / 4;
	static long height = resWidth / 4;
	this->SetWindowPos(NULL, top, left, width, height, SWP_NOZORDER | SWP_SHOWWINDOW);
	
	// �׸��� �޽����� ���ؼ��� CWnd::OnPaint()��(��) ȣ������ ���ʽÿ�.
}
