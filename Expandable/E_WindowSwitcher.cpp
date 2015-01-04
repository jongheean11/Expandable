#include "stdafx.h"
#include "E_WindowSwitcher.h"


E_WindowSwitcher* E_WindowSwitcher::singleton = NULL;

E_WindowSwitcher::E_WindowSwitcher() : running(false)
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
	running = true;
	static long resWidth = envManager->getWidth();
	static long resHeight = envManager->getHeight();
	if (E_AeroPeekController::getSingleton()->isAeroPeekMode()) {
		//aero peek size...
		
	}
	else {
		//icon size...
		
	}
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
	running = false;
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


// ���� Switcher�� �������ΰ�?
bool E_WindowSwitcher::isRunning()
{
	return running;
}


// �ػ󵵿� ����� �������� ũ�⸦ ��ȯ 1/26.6��
int E_WindowSwitcher::getIconSize(int res_width)
{
	//1280 : 48 = ȭ�� : ������ (1/26.6��)
	static const double ratio = 26.6;
	double doubleTemp = res_width / ratio;
	int intTemp = (int)doubleTemp;
	
	//�� �ø�
	if ((doubleTemp - (double)intTemp) > 0.5) {
		intTemp++;
	}
	
	return intTemp;
}


// �ػ󵵿� ����� ������� �ڽ� ũ�� 1/8.95��
int E_WindowSwitcher::getAeroSize(int res_width)
{
	//1280 : 143 = ȭ�� : ����� (1/8.95)
	static const double ratio = 8.95;
	double doubleTemp = res_width / ratio;
	int intTemp = (int)doubleTemp;

	//�� �ø�
	if ((doubleTemp - (double)intTemp) > 0.5) {
		intTemp++;
	}
	
	return intTemp;
}


// �ڽ� ������ �ڽ� ũ�⸦ �̿��� ��ü Window�ʺ� ��ȯ
int E_WindowSwitcher::getAeroWindowWidth(int maxBoxCount, int boxwidth)
{
	return 0;
}


// �ڽ� ������ �ʺ� �̿��� ���� ����
int E_WindowSwitcher::getAeroWindowHeight(int maxBoxCount, int boxwidth)
{
	return 0;
}
