#include "stdafx.h"
#include "E_WindowSwitcher.h"

const COLORREF E_WindowSwitcher::backgroundColor = RGB(0x37, 0xb6, 0xeb);
const COLORREF E_WindowSwitcher::aeroColor = RGB(0x40, 0xc0, 0xef);
const COLORREF E_WindowSwitcher::aeroColorSelected = RGB(0x30, 0xb0, 0xdf);
const COLORREF E_WindowSwitcher::borderColor = RGB(0xdc, 0xdb, 0xdb);
const COLORREF E_WindowSwitcher::borderColorSelected = RGB(0xcc, 0xcc, 0xcc);

E_WindowSwitcher* E_WindowSwitcher::singleton = NULL;
const wchar_t* E_WindowSwitcher::caption = L"WindowSwitcher";

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
	E_Global* global = E_Global::getSingleton();
	RECT r={ 0, 0, 0, 0};

	HWND hwnd = NULL;
	HTHUMBNAIL hthumbnail = NULL;

	E_Desktop* desktop = global->getSelectedDesktop();
	std::list<E_Window*> winlist = desktop->getWindowList();
	for (std::list<E_Window*>::iterator iter = winlist.begin(); iter != winlist.end(); iter++) {
		hwnd = (*iter)->getWindow();
		if (SUCCEEDED(aeroManager->registerAero(hwnd, this->GetSafeHwnd(), r, hthumbnail)) && (*iter)->isAeroPossible()) {
			//thumb_list.push_back(hthumbnail);
			thumb_map.insert(unordered_map<HWND, HTHUMBNAIL>::value_type(hwnd, hthumbnail));
		}
	}
	
	//CDC* cdc = this->GetDC();
	//cdc->SetBkMode(TRANSPARENT);
	//cdc->SetBkColor(RGB(0x0, 0x0, 0x0));
	//test code
	/*aeroManager->registerAero(E_Global::getSingleton()->getKakaoWindow()->GetSafeHwnd(), this->GetSafeHwnd(), r, temp);
	aeroManager->registerAero(E_Global::getSingleton()->getKakaoWindow()->GetSafeHwnd(), this->GetSafeHwnd(), r, temp2);

	thumb_map.insert(unordered_map< HWND, HTHUMBNAIL>::value_type(E_Global::getSingleton()->getKakaoWindow()->GetSafeHwnd(), temp));
	thumb_map.insert(unordered_map< HWND, HTHUMBNAIL>::value_type(E_Global::getSingleton()->getKakaoWindow()->GetSafeHwnd(), temp2));*/

	this->ShowWindow(SW_SHOWMAXIMIZED);
}


// UI를 없에고 창을 가리는 함수
void E_WindowSwitcher::terminateSwitcher()
{
	//크리티컬 세션?
	E_AeroPeekController* aeroManager = E_AeroPeekController::getSingleton();
	HRESULT result;
	running = false;
	this->ShowWindow(SW_HIDE);
	for (unordered_map<HWND,HTHUMBNAIL>::iterator iter = thumb_map.begin(); iter != thumb_map.end(); iter++) {
		result = aeroManager->unregisterAero(iter->second);
		if (SUCCEEDED(result)) {
			//TRACE_WIN32A("[E_WindowSwitcher::terminateSwitcher] RELEASE OK");
		}else {
			//TRACE_WIN32A("[E_WindowSwitcher::terminateSwitcher] RELEASE FAIL");
		}
	}
	//thumb_list.clear();
	thumb_map.clear();
	rect_map.clear();
}


// ZOrderTop
void E_WindowSwitcher::setZOrderTop()
{
}
BEGIN_MESSAGE_MAP(E_WindowSwitcher, CWnd)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/*창을 새로 그리는 함수*/
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
				long tempAeroHeightCount = 1;	//임시 높이는 1개

				long maxWidthCount = 1;
				long maxHeightCount = 1;

				list<E_Window*> winlist = E_Global::getSingleton()->getSelectedDesktop()->getWindowList();

				int windowSize = winlist.size();
				if (windowSize >= 7)
					maxWidthCount = 7;
				else
					maxWidthCount = windowSize;
				
				if (windowSize >= 8)
					maxHeightCount = 2;
				else
					maxHeightCount = 1;
				
				static long switcherWidth = aeroWidth * maxWidthCount + paddingSize * 2;
				static long switcherHeight = aeroHeight * maxHeightCount + paddingSize * 2; //스위치 이름 높이 나중에 추가 필요
				static long switcherLeft = resWidth / 2 - switcherWidth / 2;
				static long switcherTop = resHeight / 2 - switcherHeight / 2;

				TRACE_WIN32A("[E_WindowSwitcher::OnPaint]데스크탑 계산 switcherWidth: %d switcherHeight: %d switcherLeft: %d, switcherTop: %d", switcherWidth, switcherHeight, switcherLeft, switcherTop);

				
				int count = 0;
				int widthCount = 0;//0~6 까지 반복
				int heightCount = 0;
				WINDOWPLACEMENT windowState;

				for (list<E_Window*>::reverse_iterator iter = winlist.rbegin(); iter != winlist.rend(); iter++){
					CWnd* cwnd = CWnd::FromHandle((*iter)->getWindow());
					cwnd->GetWindowPlacement(&windowState);
					
					//
					CString windowName;
					cwnd->GetWindowTextW(windowName);
					
					TRACE_WIN32(L"[E_WindowSwitcher::OnPaint] %s\t\t[state]: %d", windowName.GetBuffer(), windowState.showCmd);
					
					long aeroLeftoffset = paddingSize + (aeroWidth * widthCount); //윈도우 별로 위치가 달라짐!!!
					long aeroTopoffset = paddingSize + (aeroHeight * heightCount); //스위치 이름 높이 나중에 추가 필요 //윈도우 별로 위치가 달라짐!!!

					CBrush brush1;   // Must initialize!
					brush1.CreateSolidBrush(E_WindowSwitcher::aeroColor);   // Blue brush.
					//Aero 내부 사각형
					CRect temprect;
					temprect.top = aeroTopoffset;
					temprect.left = aeroLeftoffset;
					temprect.bottom = temprect.top + aeroHeight;
					temprect.right = temprect.left + aeroWidth;
					//dc.Rectangle(temprect);
					dc.FillRect(&temprect, &brush1);

					//경계선
					CPen pen;
					pen.CreatePen(PS_SOLID, 1, E_WindowSwitcher::borderColor);
					dc.SelectObject(pen);
					::InflateRect(temprect, 1, 1);
					dc.MoveTo(temprect.left, temprect.top);
					dc.LineTo(temprect.right, temprect.top);
					dc.MoveTo(temprect.right, temprect.top);
					dc.LineTo(temprect.right, temprect.bottom);
					dc.MoveTo(temprect.left, temprect.top);
					dc.LineTo(temprect.left, temprect.bottom);
					dc.MoveTo(temprect.left, temprect.bottom);
					dc.LineTo(temprect.right, temprect.bottom);
					pen.DeleteObject();
					/*
					CBrush brush;
					brush.CreateStockObject(NULL_BRUSH);
					dc.SelectObject(&brush);*/
					
					//dc.Rectangle(&temprect);
					//brush.DeleteObject();

					//aero 기준 오프셋
					static long previewLeftoffset = paddingSize;	//실제 aero 크기
					static long previewTopoffset = paddingSize;	//실제 aero 크기

					TRACE_WIN32A("[E_WindowSwitcher::OnPaint] aeroLeftoffset: %d aeroTopoffset: %d previewLeftoffset: %d previewTopoffset: %d", aeroLeftoffset, aeroTopoffset, previewLeftoffset, previewTopoffset);

					CRect crect = { 0, 0, 0,0 };

					//하나의 프리뷰 박스 기준 축소된 윈도우 크기
					long windowWidth = 0;
					long windowHeight = 0;
					long windowTopOffset = 0;
					long windowLeftOffset = 0;

					CBitmap* screenshot;
					BITMAP bmpinfo;             //비트맵은 높이와 크기가 달라서
					bool isAero = (*iter)->isAeroPossible();
					if (isAero)
						cwnd->GetWindowRect(crect);
					else{
						screenshot = (*iter)->getScreenshot();
						screenshot->GetBitmap(&bmpinfo);
						crect.SetRect(0,0, bmpinfo.bmWidth, bmpinfo.bmHeight);
					}
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
					
					//에어로 픽

					RECT rect;
					rect.top = aeroTopoffset + previewTopoffset + windowTopOffset;
					rect.left = aeroLeftoffset + previewLeftoffset + windowLeftOffset;
					rect.right = rect.left + windowWidth;
					rect.bottom = rect.top + windowHeight;
					rect_map.insert(unordered_map<HWND, RECT>::value_type(cwnd->GetSafeHwnd(), temprect));
					if (isAero) {
						E_AeroPeekController::getSingleton()->moveAero((thumb_map.find(cwnd->GetSafeHwnd()))->second, rect);
					} else{
						CDC cdc;
						cdc.CreateCompatibleDC(this->GetWindowDC());
						cdc.SelectObject(screenshot->GetSafeHandle());
						dc.SetStretchBltMode(COLORONCOLOR );
						dc.StretchBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, &cdc, 0, 0, bmpinfo.bmWidth, bmpinfo.bmHeight, SRCCOPY);
					}
					
					//아이콘
					CBitmap* icon = (*iter)->getIcon();
					BITMAP icon_info;
					if (icon->m_hObject != NULL){
						icon->GetBitmap(&icon_info);
						CDC cdc;
						cdc.CreateCompatibleDC(this->GetWindowDC());
						cdc.SelectObject((*iter)->getIcon());
						dc.SetStretchBltMode(COLORONCOLOR);
						dc.StretchBlt(rect.right - icon_info.bmWidth, rect.bottom - icon_info.bmHeight, icon_info.bmWidth, icon_info.bmHeight, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, SRCCOPY);
					}
					
					//DwmSetIconicLivePreviewBitmap(this->GetSafeHwnd(), *icon, 0, 0);

					//위치 조정
					widthCount = (count + 1) % 7; 

					count++;

					if (count >= 7)
						heightCount = 1; //0 ~ 1 사이

					//테스트 코드
					if (count >= 14)
						break;
				}

				////첫번째 창
				//{
				//	CWnd* cwnd = CWnd::FromHandle((*winlist.rbegin())->getWindow());
				//	static long aeroLeftoffset = paddingSize; //윈도우 별로 위치가 달라짐!!!
				//	static long aeroTopoffset = paddingSize; //스위치 이름 높이 나중에 추가 필요 //윈도우 별로 위치가 달라짐!!!

				//	//경계선
				//	CRect temprect;
				//	temprect.top = aeroTopoffset;
				//	temprect.left = aeroLeftoffset;
				//	temprect.bottom = temprect.top + aeroHeight;
				//	temprect.right = temprect.left + aeroWidth;
				//	dc.Rectangle(temprect);
				//	
				//	//aero 기준 오프셋
				//	static long previewLeftoffset = paddingSize;	//실제 aero 크기
				//	static long previewTopoffset = paddingSize;	//실제 aero 크기

				//	TRACE_WIN32A("[E_WindowSwitcher::OnPaint] 창 aeroLeftoffset: %d aeroTopoffset: %d previewLeftoffset: %d previewTopoffset: %d", aeroLeftoffset, aeroTopoffset, previewLeftoffset, previewTopoffset);

				//	CRect crect;

				//	//하나의 프리뷰 박스 기준 축소된 윈도우 크기
				//	long windowWidth = 0;
				//	long windowHeight = 0;
				//	long windowTopOffset = 0;
				//	long windowLeftOffset = 0;

				//	//E_Global::getSingleton()->getKakaoWindow()->GetWindowRect(crect);
				//	cwnd->GetWindowRect(crect);

				//	TRACE_WIN32A("[E_WindowSwitcher::OnPaint] CRECT top %d left %d bottom %d right %d", crect.top, crect.left, crect.bottom, crect.right);

				//	double ratio = 0;
				//	switch (getShape(crect.right - crect.left, crect.bottom - crect.top, resWidth, resHeight)) {
				//	case HORIZONTAL:
				//		ratio = (double)(crect.right - crect.left) / (double)(crect.bottom - crect.top); //비율
				//		//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] 수평");

				//		//수평으로 길때
				//		windowWidth = previewWidth;
				//		windowHeight = (int)(windowWidth / ratio);
				//		windowTopOffset = (previewHeight - windowHeight) / 2;
				//		windowLeftOffset = 0;
				//		break;
				//		
				//	case VERTICAL:
				//		ratio = (double)(crect.bottom - crect.top) / (double)(crect.right - crect.left); //비율
				//		//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] 수직");

				//		//수직으로 길때
				//		windowHeight = previewHeight;
				//		windowWidth = (int)(windowHeight / ratio);
				//		windowTopOffset = 0;
				//		windowLeftOffset = (previewWidth - windowWidth) / 2;
				//		break;

				//	}

				//	TRACE_WIN32A("[E_WindowSwitcher::OnPaint] 창 ratio: %lf windowWidth: %d windowHeight: %d windowTopOffset: %d windowLeftOffset: %d",ratio, windowWidth, windowHeight, windowTopOffset, windowLeftOffset);
				//	//위치 이동
				//	this->SetWindowPos(NULL
				//		, switcherLeft
				//		, switcherTop
				//		, switcherWidth, switcherHeight
				//		, SWP_NOZORDER | SWP_SHOWWINDOW);

				//	//테스트 
				//	RECT rect;
				//	rect.top = aeroTopoffset + previewTopoffset + windowTopOffset;
				//	rect.left = aeroLeftoffset + previewLeftoffset + windowLeftOffset;
				//	rect.right = rect.left + windowWidth;
				//	rect.bottom = rect.top + windowHeight;

				//	E_AeroPeekController::getSingleton()->moveAero((thumb_map.find(cwnd->GetSafeHwnd()))->second, rect);
				//}
				////두번째 창
				//{
				//	static long aeroLeftoffset = paddingSize; //윈도우 별로 위치가 달라짐!!!
				//	static long aeroTopoffset = paddingSize + aeroHeight ; //스위치 이름 높이 나중에 추가 필요 //윈도우 별로 위치가 달라짐!!!
				//	
				//	//경계선
				//	CRect temprect;
				//	temprect.top = aeroTopoffset;
				//	temprect.left = aeroLeftoffset;
				//	temprect.bottom = temprect.top + aeroHeight;
				//	temprect.right = temprect.left + aeroWidth;
				//	dc.Rectangle(temprect);

				//	//aero 기준 오프셋
				//	static long previewLeftoffset = paddingSize;	//실제 aero 크기
				//	static long previewTopoffset = paddingSize;	//실제 aero 크기

				//	TRACE_WIN32A("[E_WindowSwitcher::OnPaint] 창 aeroLeftoffset: %d aeroTopoffset: %d previewLeftoffset: %d previewTopoffset: %d", aeroLeftoffset, aeroTopoffset, previewLeftoffset, previewTopoffset);

				//	CRect crect;

				//	//하나의 프리뷰 박스 기준 축소된 윈도우 크기
				//	long windowWidth = 0;
				//	long windowHeight = 0;
				//	long windowTopOffset = 0;
				//	long windowLeftOffset = 0;

				//	E_Global::getSingleton()->getKakaoWindow()->GetWindowRect(crect);

				//	TRACE_WIN32A("[E_WindowSwitcher::OnPaint] CRECT top %d left %d bottom %d right %d", crect.top, crect.left, crect.bottom, crect.right);

				//	double ratio = 0;
				//	switch (getShape(crect.right - crect.left, crect.bottom - crect.top, resWidth, resHeight)) {
				//	case HORIZONTAL:
				//		ratio = (double)(crect.right - crect.left) / (double)(crect.bottom - crect.top); //비율
				//		//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] 수평");

				//		//수평으로 길때
				//		windowWidth = previewWidth;
				//		windowHeight = (int)(windowWidth / ratio);
				//		windowTopOffset = (previewHeight - windowHeight) / 2;
				//		windowLeftOffset = 0;
				//		break;

				//	case VERTICAL:
				//		ratio = (double)(crect.bottom - crect.top) / (double)(crect.right - crect.left); //비율
				//		//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] 수직");

				//		//수직으로 길때
				//		windowHeight = previewHeight;
				//		windowWidth = (int)(windowHeight / ratio);
				//		windowTopOffset = 0;
				//		windowLeftOffset = (previewWidth - windowWidth) / 2;
				//		break;

				//	}

				//	TRACE_WIN32A("[E_WindowSwitcher::OnPaint] 창 ratio: %lf windowWidth: %d windowHeight: %d windowTopOffset: %d windowLeftOffset: %d", ratio, windowWidth, windowHeight, windowTopOffset, windowLeftOffset);
				//	//위치 이동
				//	this->SetWindowPos(NULL
				//		, switcherLeft
				//		, switcherTop
				//		, switcherWidth, switcherHeight
				//		, SWP_NOZORDER | SWP_SHOWWINDOW);

				//	//테스트 
				//	RECT rect;
				//	rect.top = aeroTopoffset + previewTopoffset + windowTopOffset;
				//	rect.left = aeroLeftoffset + previewLeftoffset + windowLeftOffset;
				//	rect.right = rect.left + windowWidth;
				//	rect.bottom = rect.top + windowHeight;

				//	E_AeroPeekController::getSingleton()->moveAero(this->temp2, rect);
				//}
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

/*
test code*/
void E_WindowSwitcher::drawIcon(){

	CPaintDC dc(this); // device context for painting
	E_Window * temp = new E_Window(E_Global::getSingleton()->getKakaoWindow()->GetSafeHwnd());
	
	CBitmap* icon = temp->getIcon();
	BITMAP icon_info;
	icon->GetBitmap(&icon_info);
	CDC cdc;
	cdc.CreateCompatibleDC(this->GetWindowDC());
	cdc.SelectObject(icon);
	dc.SetStretchBltMode(COLORONCOLOR);
	dc.StretchBlt(0,0, icon_info.bmWidth, icon_info.bmHeight, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, SRCCOPY);
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


HBRUSH E_WindowSwitcher::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}


void E_WindowSwitcher::OnLButtonDown(UINT nFlags, CPoint point)
{
	TRACE_WIN32A("x: %d, y: %d", point.x, point.y);
	for (unordered_map<HWND, RECT>::iterator itr = rect_map.begin(); itr != rect_map.end(); itr++){
		RECT rect = itr->second;
		if (rect.left < point.x && rect.right > point.x && rect.top < point.y && rect.bottom > point.y) {
			if (IsWindow(itr->first)){
				//HWND hwnd = ::SetFocus(itr->first);
				WINDOWPLACEMENT windowState;

				char title[255] = { 0, }; 
				::GetWindowTextA(itr->first, title, 255);
				::GetWindowPlacement(itr->first, &windowState);
				TRACE_WIN32A("[OnLButtonDown] title: %s showCmd: %d", title, windowState.showCmd);
				
				if (windowState.showCmd != SW_MAXIMIZE )
					::ShowWindow(itr->first, SW_RESTORE);
				::BringWindowToTop(itr->first);
				terminateSwitcher();
				break;
			}
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}


void E_WindowSwitcher::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	for (unordered_map<HWND, RECT>::iterator itr = rect_map.begin(); itr != rect_map.end(); itr++){
		RECT rect = itr->second;
		if (rect.left < point.x && rect.right > point.x && rect.top < point.y && rect.bottom > point.y) {
			
		}
	}
	CWnd::OnMouseMove(nFlags, point);
}
