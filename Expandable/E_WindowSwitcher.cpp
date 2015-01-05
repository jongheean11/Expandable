#include "stdafx.h"
#include "E_WindowSwitcher.h"

enum SHAPE{VERTICAL, HORIZONTAL};

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
	E_AeroPeekController* aeroManager = E_AeroPeekController::getSingleton();
	RECT r;
	r.top =0;
	r.left =0;
	r.right =100;
	r.bottom =100;
	aeroManager->registerAero(E_Global::getSingleton()->getKakaoWindow()->GetSafeHwnd(), this->GetSafeHwnd(), r, temp);
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
	static int tempDesktopCount = 2;


	CPaintDC dc(this); // device context for painting
	static long resWidth = envManager->getWidth();
	static long resHeight = envManager->getHeight();
	if (E_AeroPeekController::getSingleton()->isAeroPeekMode()) {
		
		//aero peek size...
		//ù��° ����ũž...
		long aeroWidth = getAeroSize(resWidth);	//�е� ���� aeroũ��
		long paddingSize = getPaddingSize(resWidth);	// �е��� ũ��
		//����ũž ���
		long tempWindowCount = 1;	//�ְ� �ʺ�� 1�� �ʺ�� �ְ� 7��
		long tempAeroHeightCount = 1;	//�ӽ� ���̴� 1��
		static long switcherWidth = aeroWidth * tempWindowCount + paddingSize * 2;
		static long switcherHeight = aeroWidth * tempAeroHeightCount + paddingSize * 2;
		static long switcherLeft = resWidth / 2 - switcherWidth / 2;
		static long switcherTop = resHeight / 2 - switcherHeight / 2;

		static long aeroStartLeft =  paddingSize;	
		static long aeroStartTop = paddingSize;	//����ó �̸� ���� �߰� �ʿ�
		
		static long previewWidth = aeroWidth - paddingSize * 2;	//���� aero ũ��
		static long previewLeft = paddingSize * 2;	//���� aero ũ��
		static long previewTop = paddingSize * 2;	//���� aero ũ��
		
		//��ġ �̵�
		this->SetWindowPos(NULL
			, switcherLeft
			, switcherTop
			, switcherWidth, switcherHeight
			, SWP_NOZORDER | SWP_SHOWWINDOW);

		//�׽�Ʈ 
		RECT rect;
		rect.top = previewTop;
		rect.left = previewLeft;
		rect.right = rect.left + previewWidth;
		rect.bottom = rect.top + previewWidth;
		E_AeroPeekController::getSingleton()->moveAero(this->temp, rect);
		
	}
	else {
		//icon size...
	}

	//�׸��� ������ ���� ũ�⸦ �����Ͽ� ��� ������ ��
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


// ���̾ƿ����� ���Ǵ� �е��� ũ�⸦ ��ȯ���� 
int E_WindowSwitcher::getPaddingSize(int res_width)
{
	//1 : 192 = �е� : ����ȭ��
	
	return res_width/192;
}
