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
	E_AeroPeekController* aeroManager = E_AeroPeekController::getSingleton();
	RECT r;
	r.top =0;
	r.left =0;
	r.right =100;
	r.bottom =100;
	aeroManager->registerAero(E_Global::getSingleton()->getKakaoWindow()->GetSafeHwnd(), this->GetSafeHwnd(), r, temp);
	aeroManager->registerAero(E_Global::getSingleton()->getKakaoWindow()->GetSafeHwnd(), this->GetSafeHwnd(), r, temp2);
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
	
	TRACE_WIN32A("[E_WindowSwitcher::OnPaint]resWidth: %d, resHeight: %d", resWidth, resHeight);
	if (E_AeroPeekController::getSingleton()->isAeroPeekMode()) {

		//aero peek size...
		//��ü ����ũž ���� ����...
		{
			static long aeroWidth = getAeroWidthSize(resWidth);	//�е� ���� aeroũ��
			static long aeroHeight = getAeroHeightSize(resHeight); // �е� ���� aeroũ��
			static long paddingSize = getPaddingSize(resWidth);	// �е��� ũ��
			static long previewWidth = aeroWidth - paddingSize * 2;	//���� aero ũ��
			static long previewHeight = aeroHeight - paddingSize * 2;	//���� aero ũ��
			TRACE_WIN32A("[E_WindowSwitcher::OnPaint]��ü ����ũž ���뺯�� aeroWidth: %d paddingSize: %d, previewWidth: %d", aeroWidth, paddingSize, previewWidth);
			{
				//ù��° ����ũž ���
				long tempWindowCount = 1;	//�ְ� �ʺ�� 1�� �ʺ�� �ְ� 7��
				long tempAeroHeightCount = 2;	//�ӽ� ���̴� 1��

				static long switcherWidth = aeroWidth * tempWindowCount + paddingSize * 2;
				static long switcherHeight = aeroHeight * tempAeroHeightCount + paddingSize * 2; //����ġ �̸� ���� ���߿� �߰� �ʿ�
				static long switcherLeft = resWidth / 2 - switcherWidth / 2;
				static long switcherTop = resHeight / 2 - switcherHeight / 2;

				TRACE_WIN32A("[E_WindowSwitcher::OnPaint]����ũž ��� switcherWidth: %d switcherHeight: %d switcherLeft: %d, switcherTop: %d", switcherWidth, switcherHeight, switcherLeft, switcherTop);

				//ù��° â
				{
					static long aeroLeftoffset = paddingSize; //������ ���� ��ġ�� �޶���!!!
					static long aeroTopoffset = paddingSize; //����ġ �̸� ���� ���߿� �߰� �ʿ� //������ ���� ��ġ�� �޶���!!!

					//��輱
					CRect temprect;
					temprect.top = aeroTopoffset;
					temprect.left = aeroLeftoffset;
					temprect.bottom = temprect.top + aeroHeight;
					temprect.right = temprect.left + aeroWidth;
					dc.Rectangle(temprect);
					
					//aero ���� ������
					static long previewLeftoffset = paddingSize;	//���� aero ũ��
					static long previewTopoffset = paddingSize;	//���� aero ũ��

					TRACE_WIN32A("[E_WindowSwitcher::OnPaint] â aeroLeftoffset: %d aeroTopoffset: %d previewLeftoffset: %d previewTopoffset: %d", aeroLeftoffset, aeroTopoffset, previewLeftoffset, previewTopoffset);

					CRect crect;

					//�ϳ��� ������ �ڽ� ���� ��ҵ� ������ ũ��
					long windowWidth = 0;
					long windowHeight = 0;
					long windowTopOffset = 0;
					long windowLeftOffset = 0;

					E_Global::getSingleton()->getKakaoWindow()->GetWindowRect(crect);

					TRACE_WIN32A("[E_WindowSwitcher::OnPaint] CRECT top %d left %d bottom %d right %d", crect.top, crect.left, crect.bottom, crect.right);

					double ratio = 0;
					switch (getShape(crect.right - crect.left, crect.bottom - crect.top, resWidth, resHeight)) {
					case HORIZONTAL:
						ratio = (double)(crect.right - crect.left) / (double)(crect.bottom - crect.top); //����
						//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] ����");

						//�������� �涧
						windowWidth = previewWidth;
						windowHeight = (int)(windowWidth / ratio);
						windowTopOffset = (previewHeight - windowHeight) / 2;
						windowLeftOffset = 0;
						break;
						
					case VERTICAL:
						ratio = (double)(crect.bottom - crect.top) / (double)(crect.right - crect.left); //����
						//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] ����");

						//�������� �涧
						windowHeight = previewHeight;
						windowWidth = (int)(windowHeight / ratio);
						windowTopOffset = 0;
						windowLeftOffset = (previewWidth - windowWidth) / 2;
						break;

					}

					TRACE_WIN32A("[E_WindowSwitcher::OnPaint] â ratio: %lf windowWidth: %d windowHeight: %d windowTopOffset: %d windowLeftOffset: %d",ratio, windowWidth, windowHeight, windowTopOffset, windowLeftOffset);
					//��ġ �̵�
					this->SetWindowPos(NULL
						, switcherLeft
						, switcherTop
						, switcherWidth, switcherHeight
						, SWP_NOZORDER | SWP_SHOWWINDOW);

					//�׽�Ʈ 
					RECT rect;
					rect.top = aeroTopoffset + previewTopoffset + windowTopOffset;
					rect.left = aeroLeftoffset + previewLeftoffset + windowLeftOffset;
					rect.right = rect.left + windowWidth;
					rect.bottom = rect.top + windowHeight;

					E_AeroPeekController::getSingleton()->moveAero(this->temp, rect);
				}
				//�ι�° â
				{
					static long aeroLeftoffset = paddingSize ; //������ ���� ��ġ�� �޶���!!!
					static long aeroTopoffset = paddingSize + aeroHeight ; //����ġ �̸� ���� ���߿� �߰� �ʿ� //������ ���� ��ġ�� �޶���!!!

					//��輱
					CRect temprect;
					temprect.top = aeroTopoffset;
					temprect.left = aeroLeftoffset;
					temprect.bottom = temprect.top + aeroHeight;
					temprect.right = temprect.left + aeroWidth;
					dc.Rectangle(temprect);

					//aero ���� ������
					static long previewLeftoffset = paddingSize;	//���� aero ũ��
					static long previewTopoffset = paddingSize;	//���� aero ũ��

					TRACE_WIN32A("[E_WindowSwitcher::OnPaint] â aeroLeftoffset: %d aeroTopoffset: %d previewLeftoffset: %d previewTopoffset: %d", aeroLeftoffset, aeroTopoffset, previewLeftoffset, previewTopoffset);

					CRect crect;

					//�ϳ��� ������ �ڽ� ���� ��ҵ� ������ ũ��
					long windowWidth = 0;
					long windowHeight = 0;
					long windowTopOffset = 0;
					long windowLeftOffset = 0;

					E_Global::getSingleton()->getKakaoWindow()->GetWindowRect(crect);

					TRACE_WIN32A("[E_WindowSwitcher::OnPaint] CRECT top %d left %d bottom %d right %d", crect.top, crect.left, crect.bottom, crect.right);

					double ratio = 0;
					switch (getShape(crect.right - crect.left, crect.bottom - crect.top, resWidth, resHeight)) {
					case HORIZONTAL:
						ratio = (double)(crect.right - crect.left) / (double)(crect.bottom - crect.top); //����
						//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] ����");

						//�������� �涧
						windowWidth = previewWidth;
						windowHeight = (int)(windowWidth / ratio);
						windowTopOffset = (previewHeight - windowHeight) / 2;
						windowLeftOffset = 0;
						break;

					case VERTICAL:
						ratio = (double)(crect.bottom - crect.top) / (double)(crect.right - crect.left); //����
						//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] ����");

						//�������� �涧
						windowHeight = previewHeight;
						windowWidth = (int)(windowHeight / ratio);
						windowTopOffset = 0;
						windowLeftOffset = (previewWidth - windowWidth) / 2;
						break;

					}

					TRACE_WIN32A("[E_WindowSwitcher::OnPaint] â ratio: %lf windowWidth: %d windowHeight: %d windowTopOffset: %d windowLeftOffset: %d", ratio, windowWidth, windowHeight, windowTopOffset, windowLeftOffset);
					//��ġ �̵�
					this->SetWindowPos(NULL
						, switcherLeft
						, switcherTop
						, switcherWidth, switcherHeight
						, SWP_NOZORDER | SWP_SHOWWINDOW);

					//�׽�Ʈ 
					RECT rect;
					rect.top = aeroTopoffset + previewTopoffset + windowTopOffset;
					rect.left = aeroLeftoffset + previewLeftoffset + windowLeftOffset;
					rect.right = rect.left + windowWidth;
					rect.bottom = rect.top + windowHeight;

					E_AeroPeekController::getSingleton()->moveAero(this->temp2, rect);
				}
			}
		}
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
int E_WindowSwitcher::getAeroWidthSize(int res_width)
{
	//1280 : 143 = ȭ�� : ����� (1/8.95)	//�� �䷯��
	//1920 : 144 = ȭ�� : ����� (1/13.33)	//���� ��
	static const double ratio = 13.33;
	double doubleTemp = res_width / ratio;
	int intTemp = (int)doubleTemp;

	//�� �ø�
	if ((doubleTemp - (double)intTemp) > 0.5) {
		intTemp++;
	}
	
	return intTemp;
}


// �ػ󵵿� ����� ������� �ڽ� ũ�� 1/8.95��
int E_WindowSwitcher::getAeroHeightSize(int res_height)
{
	//1080 : 80 = ȭ�� : ����� (1/13.5)	//�� �䷯��
	static const double ratio = 13.5;
	double doubleTemp = res_height / ratio;
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


// â�� ����� �� �� �ִ�.
SHAPE E_WindowSwitcher::getShape(int width, int height, int res_width, int res_height)
{
	double ratio = (double)res_width / (double)res_height;
	double targetRatio = (double)width / (double)height;
	SHAPE shape;
	if (targetRatio > ratio) {
		shape = HORIZONTAL;
	} else {
		shape = VERTICAL;
	}
	return shape;
}
