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
	static long resWidth = envManager->getWidth();
	static long resHeight = envManager->getHeight();
	E_AeroPeekController* aeroManager = E_AeroPeekController::getSingleton();
	RECT r;
	r.top =0;
	r.left =0;
	r.right =100;
	r.bottom =100;
	HTHUMBNAIL thumbnail;
	CWnd* val = E_Global::getSingleton()->getBackgroundWindow();
	HWND a = val->GetSafeHwnd();
	aeroManager->registerAero(E_Global::getSingleton()->getBackgroundWindow()->GetSafeHwnd(), this->GetSafeHwnd(), r, thumbnail);
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
	static long resHeight = envManager->getHeight();
	//�׸��� ������ ���� ũ�⸦ �����Ͽ� ��� ������ ��
	static int top = resHeight / 4;
	static int left = resWidth / 4;
	static int width = resWidth / 2;
	static long height = resHeight / 2;
	this->SetWindowPos(NULL, left, top, width, height, SWP_NOZORDER | SWP_SHOWWINDOW);
	
	
	OutputDebugStringA("OnPaint()");
	// �׸��� �޽����� ���ؼ��� CWnd::OnPaint()��(��) ȣ������ ���ʽÿ�.
}
