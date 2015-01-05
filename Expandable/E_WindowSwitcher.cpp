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


// UI를 보여주고 입력을 받는 창을 활성화 시킴
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


// UI를 없에고 창을 가리는 함수
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
		//전체 데스크탑 공용 변수...
		{
			static long aeroWidth = getAeroWidthSize(resWidth);	//패딩 포함 aero크기
			static long aeroHeight = getAeroHeightSize(resHeight); // 패딩 포함 aero크기
			static long paddingSize = getPaddingSize(resWidth);	// 패딩의 크기
			static long previewWidth = aeroWidth - paddingSize * 2;	//실제 aero 크기
			static long previewHeight = aeroHeight - paddingSize * 2;	//실제 aero 크기
			TRACE_WIN32A("[E_WindowSwitcher::OnPaint]전체 데스크탑 공용변수 aeroWidth: %d paddingSize: %d, previewWidth: %d", aeroWidth, paddingSize, previewWidth);
			{
				//첫번째 데스크탑 계산
				long tempWindowCount = 1;	//최고 너비는 1개 너비는 최고 7개
				long tempAeroHeightCount = 2;	//임시 높이는 1개

				static long switcherWidth = aeroWidth * tempWindowCount + paddingSize * 2;
				static long switcherHeight = aeroHeight * tempAeroHeightCount + paddingSize * 2; //스위치 이름 높이 나중에 추가 필요
				static long switcherLeft = resWidth / 2 - switcherWidth / 2;
				static long switcherTop = resHeight / 2 - switcherHeight / 2;

				TRACE_WIN32A("[E_WindowSwitcher::OnPaint]데스크탑 계산 switcherWidth: %d switcherHeight: %d switcherLeft: %d, switcherTop: %d", switcherWidth, switcherHeight, switcherLeft, switcherTop);

				//첫번째 창
				{
					static long aeroLeftoffset = paddingSize; //윈도우 별로 위치가 달라짐!!!
					static long aeroTopoffset = paddingSize; //스위치 이름 높이 나중에 추가 필요 //윈도우 별로 위치가 달라짐!!!

					//경계선
					CRect temprect;
					temprect.top = aeroTopoffset;
					temprect.left = aeroLeftoffset;
					temprect.bottom = temprect.top + aeroHeight;
					temprect.right = temprect.left + aeroWidth;
					dc.Rectangle(temprect);
					
					//aero 기준 오프셋
					static long previewLeftoffset = paddingSize;	//실제 aero 크기
					static long previewTopoffset = paddingSize;	//실제 aero 크기

					TRACE_WIN32A("[E_WindowSwitcher::OnPaint] 창 aeroLeftoffset: %d aeroTopoffset: %d previewLeftoffset: %d previewTopoffset: %d", aeroLeftoffset, aeroTopoffset, previewLeftoffset, previewTopoffset);

					CRect crect;

					//하나의 프리뷰 박스 기준 축소된 윈도우 크기
					long windowWidth = 0;
					long windowHeight = 0;
					long windowTopOffset = 0;
					long windowLeftOffset = 0;

					E_Global::getSingleton()->getKakaoWindow()->GetWindowRect(crect);

					TRACE_WIN32A("[E_WindowSwitcher::OnPaint] CRECT top %d left %d bottom %d right %d", crect.top, crect.left, crect.bottom, crect.right);

					double ratio = 0;
					switch (getShape(crect.right - crect.left, crect.bottom - crect.top, resWidth, resHeight)) {
					case HORIZONTAL:
						ratio = (double)(crect.right - crect.left) / (double)(crect.bottom - crect.top); //비율
						//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] 수평");

						//수평으로 길때
						windowWidth = previewWidth;
						windowHeight = (int)(windowWidth / ratio);
						windowTopOffset = (previewHeight - windowHeight) / 2;
						windowLeftOffset = 0;
						break;
						
					case VERTICAL:
						ratio = (double)(crect.bottom - crect.top) / (double)(crect.right - crect.left); //비율
						//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] 수직");

						//수직으로 길때
						windowHeight = previewHeight;
						windowWidth = (int)(windowHeight / ratio);
						windowTopOffset = 0;
						windowLeftOffset = (previewWidth - windowWidth) / 2;
						break;

					}

					TRACE_WIN32A("[E_WindowSwitcher::OnPaint] 창 ratio: %lf windowWidth: %d windowHeight: %d windowTopOffset: %d windowLeftOffset: %d",ratio, windowWidth, windowHeight, windowTopOffset, windowLeftOffset);
					//위치 이동
					this->SetWindowPos(NULL
						, switcherLeft
						, switcherTop
						, switcherWidth, switcherHeight
						, SWP_NOZORDER | SWP_SHOWWINDOW);

					//테스트 
					RECT rect;
					rect.top = aeroTopoffset + previewTopoffset + windowTopOffset;
					rect.left = aeroLeftoffset + previewLeftoffset + windowLeftOffset;
					rect.right = rect.left + windowWidth;
					rect.bottom = rect.top + windowHeight;

					E_AeroPeekController::getSingleton()->moveAero(this->temp, rect);
				}
				//두번째 창
				{
					static long aeroLeftoffset = paddingSize ; //윈도우 별로 위치가 달라짐!!!
					static long aeroTopoffset = paddingSize + aeroHeight ; //스위치 이름 높이 나중에 추가 필요 //윈도우 별로 위치가 달라짐!!!

					//경계선
					CRect temprect;
					temprect.top = aeroTopoffset;
					temprect.left = aeroLeftoffset;
					temprect.bottom = temprect.top + aeroHeight;
					temprect.right = temprect.left + aeroWidth;
					dc.Rectangle(temprect);

					//aero 기준 오프셋
					static long previewLeftoffset = paddingSize;	//실제 aero 크기
					static long previewTopoffset = paddingSize;	//실제 aero 크기

					TRACE_WIN32A("[E_WindowSwitcher::OnPaint] 창 aeroLeftoffset: %d aeroTopoffset: %d previewLeftoffset: %d previewTopoffset: %d", aeroLeftoffset, aeroTopoffset, previewLeftoffset, previewTopoffset);

					CRect crect;

					//하나의 프리뷰 박스 기준 축소된 윈도우 크기
					long windowWidth = 0;
					long windowHeight = 0;
					long windowTopOffset = 0;
					long windowLeftOffset = 0;

					E_Global::getSingleton()->getKakaoWindow()->GetWindowRect(crect);

					TRACE_WIN32A("[E_WindowSwitcher::OnPaint] CRECT top %d left %d bottom %d right %d", crect.top, crect.left, crect.bottom, crect.right);

					double ratio = 0;
					switch (getShape(crect.right - crect.left, crect.bottom - crect.top, resWidth, resHeight)) {
					case HORIZONTAL:
						ratio = (double)(crect.right - crect.left) / (double)(crect.bottom - crect.top); //비율
						//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] 수평");

						//수평으로 길때
						windowWidth = previewWidth;
						windowHeight = (int)(windowWidth / ratio);
						windowTopOffset = (previewHeight - windowHeight) / 2;
						windowLeftOffset = 0;
						break;

					case VERTICAL:
						ratio = (double)(crect.bottom - crect.top) / (double)(crect.right - crect.left); //비율
						//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] 수직");

						//수직으로 길때
						windowHeight = previewHeight;
						windowWidth = (int)(windowHeight / ratio);
						windowTopOffset = 0;
						windowLeftOffset = (previewWidth - windowWidth) / 2;
						break;

					}

					TRACE_WIN32A("[E_WindowSwitcher::OnPaint] 창 ratio: %lf windowWidth: %d windowHeight: %d windowTopOffset: %d windowLeftOffset: %d", ratio, windowWidth, windowHeight, windowTopOffset, windowLeftOffset);
					//위치 이동
					this->SetWindowPos(NULL
						, switcherLeft
						, switcherTop
						, switcherWidth, switcherHeight
						, SWP_NOZORDER | SWP_SHOWWINDOW);

					//테스트 
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

	//항목의 개수에 따라 크기를 조절하여 가운데 정렬할 것
	OutputDebugStringA("OnPaint()");
	// 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.
}


// 현재 Switcher가 동작중인가?
bool E_WindowSwitcher::isRunning()
{
	return running;
}


// 해상도에 대비한 아이콘의 크기를 반환 1/26.6배
int E_WindowSwitcher::getIconSize(int res_width)
{
	//1280 : 48 = 화면 : 아이콘 (1/26.6배)
	
	static const double ratio = 26.6;
	double doubleTemp = res_width / ratio;
	int intTemp = (int)doubleTemp;
	
	//반 올림
	if ((doubleTemp - (double)intTemp) > 0.5) {
		intTemp++;
	}
	
	return intTemp;
}


// 해상도에 비례한 에어로픽 박스 크기 1/8.95배
int E_WindowSwitcher::getAeroWidthSize(int res_width)
{
	//1280 : 143 = 화면 : 에어로 (1/8.95)	//맥 페러럴
	//1920 : 144 = 화면 : 에어로 (1/13.33)	//실제 컴
	static const double ratio = 13.33;
	double doubleTemp = res_width / ratio;
	int intTemp = (int)doubleTemp;

	//반 올림
	if ((doubleTemp - (double)intTemp) > 0.5) {
		intTemp++;
	}
	
	return intTemp;
}


// 해상도에 비례한 에어로픽 박스 크기 1/8.95배
int E_WindowSwitcher::getAeroHeightSize(int res_height)
{
	//1080 : 80 = 화면 : 에어로 (1/13.5)	//맥 페러럴
	static const double ratio = 13.5;
	double doubleTemp = res_height / ratio;
	int intTemp = (int)doubleTemp;

	//반 올림
	if ((doubleTemp - (double)intTemp) > 0.5) {
		intTemp++;
	}

	return intTemp;
}


// 박스 개수와 박스 크기를 이용한 전체 Window너비 반환
int E_WindowSwitcher::getAeroWindowWidth(int maxBoxCount, int boxwidth)
{
	return 0;
}


// 박스 개수와 너비를 이용해 높이 산출
int E_WindowSwitcher::getAeroWindowHeight(int maxBoxCount, int boxwidth)
{
	return 0;
}


// 레이아웃에서 사용되는 패딩의 크기를 반환받음 
int E_WindowSwitcher::getPaddingSize(int res_width)
{
	//1 : 192 = 패딩 : 바탕화면
	
	return res_width/192;
}


// 창의 모양을 알 수 있다.
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
