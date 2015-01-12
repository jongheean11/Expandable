#include "stdafx.h"
#include "E_WindowSwitcher.h"

const COLORREF E_WindowSwitcher::backgroundColor = RGB(0x37, 0xb6, 0xeb);
const COLORREF E_WindowSwitcher::aeroColor = RGB(0x40, 0xc0, 0xef);
const COLORREF E_WindowSwitcher::aeroColorSelected = RGB(0x30, 0xb0, 0xdf);
const COLORREF E_WindowSwitcher::aeroColorSelectedMouse = RGB(0x35, 0xb5, 0xee);
const COLORREF E_WindowSwitcher::borderColor = RGB(0xdc, 0xdb, 0xdb);
const COLORREF E_WindowSwitcher::borderColorSelected = RGB(0xcc, 0xcc, 0xcc);
const COLORREF E_WindowSwitcher::borderColorSelectedMouse = RGB(0xcf, 0xdc, 0xdc);

E_WindowSwitcher* E_WindowSwitcher::singleton = NULL;
const wchar_t* E_WindowSwitcher::caption = L"WindowSwitcher";

void E_WindowSwitcher::updateSelectedDesktop()
{
	int a;
	//업데이트가 발생한 경우 자동으로 호출됨
	TRACE_WIN32A("[E_WindowSwitcher::updateSelectedDesktop()]");
}
E_WindowSwitcher::E_WindowSwitcher() : running(false)
{
	envManager = E_EnvironmentManager::getSingleton();

	CBrush brush_window;
	UINT nClassStyle_window = 0;// CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	CString szClassName_window = AfxRegisterWndClass(nClassStyle_window, 0, (HBRUSH)CreateSolidBrush(E_WindowSwitcher::backgroundColor), 0);
	this->CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_window, E_WindowSwitcher::caption, WS_VISIBLE | WS_POPUP, CRect(0, 0, 200, 400), CWnd::GetDesktopWindow(), 0);
	this->ShowWindow(SW_HIDE);
	this->UpdateWindow();
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
	E_AeroPeekController* aeroManager = E_AeroPeekController::getSingleton();
	E_Global* global = E_Global::getSingleton();
	global->startUpdate();
	running = true;

	RECT r={ 0, 0, 0, 0};
	HWND hwnd = NULL;
	HTHUMBNAIL hthumbnail = NULL;

	E_Desktop* desktop = global->getSelectedDesktop();
	for (list<E_Desktop*>::iterator iterDesktop = global->desktopList.begin(); iterDesktop != global->desktopList.end(); iterDesktop++){
			std::list<E_Window*> winlist = (*iterDesktop)->getWindowList();
			for (std::list<E_Window*>::iterator iter = winlist.begin(); iter != winlist.end(); iter++) {
				hwnd = (*iter)->getWindow();
				mode_map.insert(unordered_map<HWND, DRAWMODE>::value_type(hwnd, DRAW_NORMAL));
				if (SUCCEEDED(aeroManager->registerAero(hwnd, this->GetSafeHwnd(), r, hthumbnail)) && (*iter)->isAeroPossible()) {
					//thumb_list.push_back(hthumbnail);
					thumb_map.insert(unordered_map<HWND, HTHUMBNAIL>::value_type(hwnd, hthumbnail));
				}
			}
	}

	//CDC* cdc = this->GetDC();
	//cdc->SetBkMode(TRANSPARENT);
	//cdc->SetBkColor(RGB(0x0, 0x0, 0x0 ));
	//test code
	/*aeroManager->registerAero(E_Global::getSingleton()->getKakaoWindow()->GetSafeHwnd(), this->GetSafeHwnd(), r, temp);
	aeroManager->registerAero(E_Global::getSingleton()->getKakaoWindow()->GetSafeHwnd(), this->GetSafeHwnd(), r, temp2);

	thumb_map.insert(unordered_map< HWND, HTHUMBNAIL>::value_type(E_Global::getSingleton()->getKakaoWindow()->GetSafeHwnd(), temp));
	thumb_map.insert(unordered_map< HWND, HTHUMBNAIL>::value_type(E_Global::getSingleton()->getKakaoWindow()->GetSafeHwnd(), temp2));*/

	this->ShowWindow(SW_SHOW);
}


// UI를 없에고 창을 가리는 함수
void E_WindowSwitcher::terminateSwitcher()
{
	//크리티컬 세션?
	E_AeroPeekController* aeroManager = E_AeroPeekController::getSingleton();
	E_Global::getSingleton()->stopUpdate();

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
	mode_map.clear();
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
	CPaintDC dc(this); // device context for painting
	static long resWidth = envManager->getWidth();
	static long resHeight = envManager->getHeight();
	
	//TRACE_WIN32A("[E_WindowSwitcher::OnPaint]resWidth: %d, resHeight: %d", resWidth, resHeight);
	if (E_AeroPeekController::getSingleton()->isAeroPeekMode()) {
		
		//aero peek size...
		//전체 데스크탑 공용 변수...
		{
			static long aeroWidth = getAeroWidthSize(resWidth);	//패딩 포함 aero크기
			static long aeroHeight = getAeroHeightSize(resHeight); // 패딩 포함 aero크기
			static long paddingSize = getPaddingSize(resWidth);	// 패딩의 크기
			static long previewWidth = aeroWidth - paddingSize * 2;	//실제 aero 크기
			static long previewHeight = aeroHeight - paddingSize * 2;	//실제 aero 크기
			//TRACE_WIN32A("[E_WindowSwitcher::OnPaint]전체 데스크탑 공용변수 aeroWidth: %d paddingSize: %d, previewWidth: %d", aeroWidth, paddingSize, previewWidth);
			{
				// 더블 버퍼링을 위한 코드
				//메모리 DC를 생성한다. (버퍼 메모리 할당)
				CDC memDC;
				//그릴 Bitmap을 생성한다. (한번에 그릴 도화지 정도 개념)
				CBitmap bmp;
				//메모리 DC를 위의 CPaintDC인 dc에 호환되게 만들어 준다.
				memDC.CreateCompatibleDC(&dc);
				//주어진 dc에 호환하는 비트맵을 생성한다.
				bmp.CreateCompatibleBitmap(&dc, resWidth, resHeight);
				//이제 memDC에 생성된 비트맵을 연결한다.
				memDC.SelectObject(bmp);

				CBrush brush;   // Must initialize!
				brush.CreateSolidBrush(E_WindowSwitcher::aeroColor);   // Blue brush.

				RECT rect;
				rect.left = 0;
				rect.top = 0;
				rect.right = resWidth;
				rect.bottom = resHeight;
				memDC.FillRect(&rect, &brush);

				brush.DeleteObject();

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

				//TRACE_WIN32A("[E_WindowSwitcher::OnPaint]데스크탑 계산 switcherWidth: %d switcherHeight: %d switcherLeft: %d, switcherTop: %d", switcherWidth, switcherHeight, switcherLeft, switcherTop);

				//크기 계산을 위한 카운트 변수
				int count = 0;
				int widthCount = 0;//0~6 까지 반복
				int heightCount = 0;
				WINDOWPLACEMENT windowState;

				for (list<E_Window*>::reverse_iterator iter = winlist.rbegin(); iter != winlist.rend(); iter++){
					//mode_map.key
					if (mode_map.find((*iter)->getWindow()) == mode_map.end()){
						continue;
					}
					unordered_map<HWND, DRAWMODE>::iterator modeIter = mode_map.find((*iter)->getWindow());
					DRAWMODE mode = modeIter->second;
					CWnd* cwnd = CWnd::FromHandle((*iter)->getWindow());
					cwnd->GetWindowPlacement(&windowState);
					
					//
					CString windowName;
					cwnd->GetWindowTextW(windowName);
					
					//TRACE_WIN32(L"[E_WindowSwitcher::OnPaint] %s\t\t[state]: %d", windowName.GetBuffer(), windowState.showCmd);

					long aeroLeftoffset = paddingSize + (aeroWidth * widthCount); //윈도우 별로 위치가 달라짐!!!
					long aeroTopoffset = paddingSize + (aeroHeight * heightCount); //스위치 이름 높이 나중에 추가 필요 //윈도우 별로 위치가 달라짐!!!
					
					
					//Aero 내부 사각형 브러쉬
					CBrush brush1;   // Must initialize!
					if (mode == DRAW_NORMAL)
						brush1.CreateSolidBrush(E_WindowSwitcher::aeroColor);   // Blue brush.
					else if (mode == UPDATE_TAB)
						brush1.CreateSolidBrush(E_WindowSwitcher::aeroColorSelected);   // Blue brush.
					else if (mode == UPDATE_TOUCH)
						brush1.CreateSolidBrush(E_WindowSwitcher::aeroColorSelectedMouse);   // Blue brush.
						
					CRect temprect;
					temprect.top = aeroTopoffset;
					temprect.left = aeroLeftoffset;
					temprect.bottom = temprect.top + aeroHeight;
					temprect.right = temprect.left + aeroWidth;
				
					//경계선 브러쉬
					CPen pen;
					if (mode == DRAW_NORMAL)
						pen.CreatePen(PS_SOLID, 1, E_WindowSwitcher::borderColor);
					else if (mode == UPDATE_TAB)
						pen.CreatePen(PS_SOLID, 1, E_WindowSwitcher::borderColorSelected);
					else if (mode == UPDATE_TOUCH)
						pen.CreatePen(PS_SOLID, 1, E_WindowSwitcher::borderColorSelectedMouse);

					//배경 그리기
					//dc.Rectangle(temprect);
					memDC.FillRect(&temprect, &brush1);
					
					//경계선 그리기

					if (mode == UPDATE_TAB || mode == UPDATE_TOUCH) {
						memDC.SelectObject(pen);
						::InflateRect(temprect, 1, 1);
						int padding = 2;
						memDC.MoveTo(temprect.left + padding, temprect.top + padding);
						memDC.LineTo(temprect.right - padding, temprect.top + padding);
						memDC.MoveTo(temprect.right - padding, temprect.top + padding);
						memDC.LineTo(temprect.right - padding, temprect.bottom - padding);
						memDC.MoveTo(temprect.left + padding, temprect.top + padding);
						memDC.LineTo(temprect.left + padding, temprect.bottom - padding);
						memDC.MoveTo(temprect.left + padding, temprect.bottom - padding);
						memDC.LineTo(temprect.right - padding, temprect.bottom - padding);
					}

					//aero 기준 오프셋
					static long previewLeftoffset = paddingSize;	//실제 aero 크기
					static long previewTopoffset = paddingSize;	//실제 aero 크기

					//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] aeroLeftoffset: %d aeroTopoffset: %d previewLeftoffset: %d previewTopoffset: %d", aeroLeftoffset, aeroTopoffset, previewLeftoffset, previewTopoffset);

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
					//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] CRECT top %d left %d bottom %d right %d", crect.top, crect.left, crect.bottom, crect.right);

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

					//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] 창 ratio: %lf windowWidth: %d windowHeight: %d windowTopOffset: %d windowLeftOffset: %d", ratio, windowWidth, windowHeight, windowTopOffset, windowLeftOffset);
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
						cdc.SetBkMode(1);
						cdc.SetBkColor(E_WindowSwitcher::backgroundColor);
						memDC.SetStretchBltMode(COLORONCOLOR);
						memDC.StretchBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, &cdc, 0, 0, bmpinfo.bmWidth, bmpinfo.bmHeight, SRCCOPY);
						cdc.DeleteDC();
					}
					
					//아이콘
					CBitmap* icon = (*iter)->getIcon();
					BITMAP icon_info;
					if (icon->m_hObject != NULL){
						icon->GetBitmap(&icon_info);
						CDC cdc;
						cdc.CreateCompatibleDC(this->GetWindowDC());
						cdc.SelectObject(icon);
						memDC.SetStretchBltMode(COLORONCOLOR);
						memDC.StretchBlt(rect.right - icon_info.bmWidth, rect.bottom - icon_info.bmHeight, icon_info.bmWidth, icon_info.bmHeight, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, SRCCOPY);
						cdc.DeleteDC();
					}


					//업데이트 플래그
					mode_map.find((*iter)->getWindow())->second = DRAW_NORMAL;

					//위치 조정
					widthCount = (count + 1) % 7; 
					count++;
					if (count >= 7)
						heightCount = 1; //0 ~ 1 사이
					//테스트 코드
					if (count >= 14)
						break;

					
					brush1.DeleteObject();
					pen.DeleteObject();
				}

				//버퍼에 있는 내용 한번에 그리기
				dc.StretchBlt(0, 0, resWidth, resHeight, &memDC, 0, 0, resWidth, resHeight, SRCCOPY);

				//dc 해제
				memDC.DeleteDC();
				bmp.DeleteObject();
			}
		}
	}
	else {
		//icon size...
	}
	//항목의 개수에 따라 크기를 조절하여 가운데 정렬할 것
	//OutputDebugStringA("OnPaint()");
	// 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.
}

/*
test code*/
//void E_WindowSwitcher::drawIcon(){
//
//	CPaintDC dc(this); // device context for painting
//	E_Window * temp = new E_Window(E_Global::getSingleton()->getKakaoWindow()->GetSafeHwnd());
//	
//	CBitmap* icon = temp->getIcon();
//	BITMAP icon_info;
//	icon->GetBitmap(&icon_info);
//	CDC cdc;
//	cdc.CreateCompatibleDC(this->GetWindowDC());
//	cdc.SelectObject(icon);
//	dc.SetStretchBltMode(COLORONCOLOR);
//	dc.StretchBlt(0,0, icon_info.bmWidth, icon_info.bmHeight, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, SRCCOPY);
//}

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
			mode_map.find(itr->first)->second = UPDATE_TOUCH;
		}
	}
	Invalidate(0);
	CWnd::OnMouseMove(nFlags, point);
}
