#include "stdafx.h"
#include "E_WindowSwitcher.h"
#define WM_TRAY_EVENT (WM_USER + 3)
const COLORREF E_WindowSwitcher::backgroundColor = RGB(0x37, 0xb6, 0xeb);
const COLORREF E_WindowSwitcher::aeroColor = RGB(0x40, 0xc0, 0xef);
const COLORREF E_WindowSwitcher::aeroColorSelected = RGB(0x30, 0xb0, 0xe5);
const COLORREF E_WindowSwitcher::aeroColorSelectedMouse = RGB(0x30, 0xb0, 0xee);
const COLORREF E_WindowSwitcher::borderColor = RGB(120, 120, 120);
const COLORREF E_WindowSwitcher::borderColorSelected = RGB(0xcc, 0xcc, 0xcc);
const COLORREF E_WindowSwitcher::borderColorSelectedMouse = RGB(60,60,60);

E_WindowSwitcher* E_WindowSwitcher::singleton = NULL;
const wchar_t* E_WindowSwitcher::caption = L"E_WindowSwitcher";

void E_WindowSwitcher::updateSelectedDesktop()
{
	if (running){
		this->SetWindowPos(NULL	//투명 윈도우 잔상 제거
			, 0
			, 0
			, 1, 1
			, SWP_NOZORDER);
		restartSwitcher();
		Invalidate(0);
	}
	//업데이트가 발생한 경우 자동으로 호출됨
	//TRACE_WIN32A("[E_WindowSwitcher::updateSelectedDesktop()]");
}

E_WindowSwitcher::E_WindowSwitcher() : running(false), updateFlag(false), tabMode(SELECTEDDESKTOP), isfocus(false)
{
	envManager = E_EnvironmentManager::getSingleton();

	UINT nClassStyle_window = 0;// CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	
	CBrush brush_map;
	//////brush_map.CreateSolidBrush(E_WindowSwitcher::backgroundColor);
	brush_map.CreateStockObject(NULL_BRUSH);
	CString szClassName_window = AfxRegisterWndClass(nClassStyle_window, 0, (HBRUSH)brush_map.GetSafeHandle(), 0);
	//CString szClassName_window = AfxRegisterWndClass(nClassStyle_window, 0, fillBrush, 0);
	this->CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_window, E_WindowSwitcher::caption, WS_VISIBLE | WS_POPUP, CRect(0, 0, 1, 1), CWnd::GetDesktopWindow(), 0);
	this->ShowWindow(SW_HIDE);
	this->UpdateWindow();
	//brush_map.DeleteObject();
	//  selectedIndex = 0;
	startTaboffset = 0;

	//예외 리스트
	aero_exclude_winlist.push_back("Microsoft Visual Studio");
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
	//global->startUpdate();

	RECT r = { 0, 0, 0, 0 };
	HWND hwnd = NULL;
	HTHUMBNAIL hthumbnail = NULL;

	//정보 업데이트
	global->onUpdate();

	//AERO모드
	startTPMode();

	//각종 정보 등록
	E_Desktop* desktop = global->getSelectedDesktop();
	for (list<E_Desktop*>::iterator iterDesktop = global->desktopList.begin(); iterDesktop != global->desktopList.end(); iterDesktop++){
		std::list<E_Window*> winlist = (*iterDesktop)->getWindowList();
		for (std::list<E_Window*>::iterator iter = winlist.begin(); iter != winlist.end(); iter++) {

			hwnd = (*iter)->getWindow();

			icon_map.insert(unordered_map<HWND, CWnd*>::value_type(hwnd, createChild()));
			mode_map.insert(unordered_map<HWND, DRAWMODE>::value_type(hwnd, DRAW_NORMAL));
			if (SUCCEEDED(aeroManager->registerAero(hwnd, this->GetSafeHwnd(), r, hthumbnail))){
				//thumb_list.push_back(hthumbnail);
				thumb_map.insert(unordered_map<HWND, HTHUMBNAIL>::value_type(hwnd, hthumbnail));
			}
			if ((*iterDesktop) == desktop){
				group_map.insert(unordered_map<HWND, GROUP2>::value_type(hwnd, SELECTEDDESKTOP));
			}
			else{
				group_map.insert(unordered_map<HWND, GROUP2>::value_type(hwnd, OTHERDESKTOP));
			}
			desktop_map.insert(unordered_map<HWND, E_Desktop*>::value_type(hwnd, *iterDesktop));
		}
	}
	
	//스위처 상태 초기화
	tabMode = SELECTEDDESKTOP;	//선택한 탭
	startTaboffset = 0;	//윈도우 리스트에서 시작 위치
	tabIndex = 1;	//선택한 윈도우 인덱스

	//데스크탑
	E_Window* desktopWindow = global->backgroundWindow;
	mode_map.insert(unordered_map<HWND, DRAWMODE>::value_type(desktopWindow->getWindow(), DRAW_NORMAL));
	if (SUCCEEDED(aeroManager->registerAero(desktopWindow->getWindow(), this->GetSafeHwnd(), r, hthumbnail)) && desktopWindow->isAeroPossible()) {
		thumb_map.insert(unordered_map<HWND, HTHUMBNAIL>::value_type(desktopWindow->getWindow(), hthumbnail));
		group_map.insert(unordered_map<HWND, GROUP2>::value_type(desktopWindow->getWindow(), SELECTEDDESKTOP));
	}

	SetTimer(1, 50, NULL);
}


// UI를 없에고 창을 가리는 함수
void E_WindowSwitcher::terminateSwitcher()
{
	if (running) {
		stopTPMode();
		isfocus = false;	//포커스 플래그 초기화
		running = false;
		//크리티컬 세션?
		E_AeroPeekController* aeroManager = E_AeroPeekController::getSingleton();
		//E_Global::getSingleton()->stopUpdate();

		HRESULT result;
		this->ShowWindow(SW_HIDE);

		//자원 정리
		for (unordered_map<HWND, HTHUMBNAIL>::iterator iter = thumb_map.begin(); iter != thumb_map.end(); iter++) {
			result = aeroManager->unregisterAero(iter->second);
			if (SUCCEEDED(result)) {
				//TRACE_WIN32A("[E_WindowSwitcher::terminateSwitcher] RELEASE OK");
			}
			else {
				//TRACE_WIN32A("[E_WindowSwitcher::terminateSwitcher] RELEASE FAIL");
			}
		}
		//아이콘 해제
		for (unordered_map<HWND, CWnd*>::iterator iter = icon_map.begin(); iter != icon_map.end(); iter++) {
			if (::IsWindow(iter->second->GetSafeHwnd())){
				iter->second->DestroyWindow();
				delete iter->second;
			}
		}
		icon_map.clear();

		thumb_map.clear();
		rect_map.clear();
		mode_map.clear();
		group_map.clear();
		desktop_map.clear();

		this->SetWindowPos(NULL	//투명 윈도우 잔상 제거
			, 0
			, 0
			, 1, 1
			, SWP_NOZORDER);
	}
}

// 스위처를 재시작하는 함수
void E_WindowSwitcher::restartSwitcher()
{
	//투명 해제 모드
	stopTPMode();
	
	HRESULT result;
	E_AeroPeekController* aeroManager = E_AeroPeekController::getSingleton();
	E_Global* global = E_Global::getSingleton();
	//자원 정리
	for (unordered_map<HWND, HTHUMBNAIL>::iterator iter = thumb_map.begin(); iter != thumb_map.end(); iter++) {
		result = aeroManager->unregisterAero(iter->second);
		if (SUCCEEDED(result)) {
			//TRACE_WIN32A("[E_WindowSwitcher::terminateSwitcher] RELEASE OK");
		}
		else {
			//TRACE_WIN32A("[E_WindowSwitcher::terminateSwitcher] RELEASE FAIL");
		}
	}

	//아이콘 해제
	for (unordered_map<HWND, CWnd*>::iterator iter = icon_map.begin(); iter != icon_map.end(); iter++) {
		if (::IsWindow(iter->second->GetSafeHwnd()))
			delete iter->second;
	}
	icon_map.clear();

	thumb_map.clear();
	rect_map.clear();
	mode_map.clear();
	group_map.clear();
	desktop_map.clear();

	//재등록하는 부분

	//정보 업데이트
	global->onUpdate();

	RECT r = { 0, 0, 0, 0 };
	HWND hwnd = NULL;
	HTHUMBNAIL hthumbnail = NULL;

	//투명 해제 모드
	startTPMode();

	E_Desktop* desktop = global->getSelectedDesktop();
	for (list<E_Desktop*>::iterator iterDesktop = global->desktopList.begin(); iterDesktop != global->desktopList.end(); iterDesktop++){
		std::list<E_Window*> winlist = (*iterDesktop)->getWindowList();
		for (std::list<E_Window*>::iterator iter = winlist.begin(); iter != winlist.end(); iter++) {
			hwnd = (*iter)->getWindow();

			icon_map.insert(unordered_map<HWND, CWnd*>::value_type(hwnd, createChild()));
			mode_map.insert(unordered_map<HWND, DRAWMODE>::value_type(hwnd, DRAW_NORMAL));
			if (SUCCEEDED(aeroManager->registerAero(hwnd, this->GetSafeHwnd(), r, hthumbnail))){ 
				//thumb_list.push_back(hthumbnail);
				thumb_map.insert(unordered_map<HWND, HTHUMBNAIL>::value_type(hwnd, hthumbnail));
			}
			if ((*iterDesktop) == desktop){
				group_map.insert(unordered_map<HWND, GROUP2>::value_type(hwnd, SELECTEDDESKTOP));
			}
			else{
				group_map.insert(unordered_map<HWND, GROUP2>::value_type(hwnd, OTHERDESKTOP));
			}
			desktop_map.insert(unordered_map<HWND, E_Desktop*>::value_type(hwnd, *iterDesktop));
		}
	}

	//데스크탑
	E_Window* desktopWindow = global->backgroundWindow;
	mode_map.insert(unordered_map<HWND, DRAWMODE>::value_type(desktopWindow->getWindow(), DRAW_NORMAL));
	if (SUCCEEDED(aeroManager->registerAero(desktopWindow->getWindow(), this->GetSafeHwnd(), r, hthumbnail)) && desktopWindow->isAeroPossible()) {
		thumb_map.insert(unordered_map<HWND, HTHUMBNAIL>::value_type(desktopWindow->getWindow(), hthumbnail));
		group_map.insert(unordered_map<HWND, GROUP2>::value_type(desktopWindow->getWindow(), SELECTEDDESKTOP));
	}

}


BEGIN_MESSAGE_MAP(E_WindowSwitcher, CWnd)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_KILLFOCUS()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()

/*창을 새로 그리는 함수*/
void E_WindowSwitcher::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	static long resWidth = envManager->getWidth();
	static long resHeight = envManager->getHeight();

	E_Global* global = E_Global::getSingleton();
	//TRACE_WIN32A("[E_WindowSwitcher::OnPaint]resWidth: %d, resHeight: %d", resWidth, resHeight);
	if (E_AeroPeekController::getSingleton()->isAeroPeekMode() || true ){	//에어로픽 모드일때만 동작하지 않도록   ) {

		//aero peek size...
		//전체 데스크탑 공용 변수...
		{
			long aeroWidth = getAeroWidthSize(resWidth);	//패딩 포함 aero크기
			long aeroHeight = getAeroHeightSize(resHeight); // 패딩 포함 aero크기
			long paddingSize = getPaddingSize(resWidth);	// 패딩의 크기
			long titleSize = paddingSize * 3;				// 타이틀바 크기
			long titleFontSize = paddingSize * 2;				// 글씨 높이 크기
			long previewWidth = aeroWidth - paddingSize * 2;	//실제 aero 크기
			long previewHeight = aeroHeight - paddingSize * 2;	//실제 aero 크기
			long bottomPadding = paddingSize * 6;
			rect_map.clear();//그릴때 마다 터치 좌표 초기화

			//TRACE_WIN32A("[E_WindowSwitcher::OnPaint]전체 데스크탑 공용변수 aeroWidth: %d paddingSize: %d, previewWidth: %d", aeroWidth, paddingSize, previewWidth);
			{
				//타이틀 바
				//CFont* pOldFont;
				CFont font;
				LOGFONT lf;
				::ZeroMemory(&lf, sizeof(lf));
				wsprintf(lf.lfFaceName, TEXT("%s"), TEXT("Arial"));
				lf.lfHeight = titleFontSize;
				//lf.lfWeight = 550;
				//폰트 생성
				font.CreateFontIndirectW(&lf);

				//첫번째 데스크탑 윈도우 스위처 계산 //
				long maxWidthCount = 1;
				long maxHeightCount = 1;

				list<E_Window*> winlist = E_Global::getSingleton()->getSelectedDesktop()->getWindowList();
				winlist.push_front(global->backgroundWindow);	//바탕화면 포함

				int windowSize = winlist.size();
					
				if (windowSize >= 7)
					maxWidthCount = 7;
				else
					maxWidthCount = windowSize;

				if (windowSize >= 8)
					maxHeightCount = 2;
				else
					maxHeightCount = 1;

				long switcherWidth = aeroWidth * maxWidthCount + paddingSize * 2;
				long switcherHeight = titleSize + aeroHeight * maxHeightCount + paddingSize * 2; //스위치 이름 높이 나중에 추가 필요
				long switcherLeft = 0;				//윈도우 기준 오프셋				
				long switcherTop = 0;				//CDC기준으로 항상 0

				if (windowSize == 0){ switcherWidth = 0; switcherHeight = 0;}
				//if (windowSize == 1){ tabIndex = 0; }

				//크기 계산을 위한 카운트 변수
				int count = 0;
				int widthCount = 0;//0~6 까지 반복
				int heightCount = 0;//0~1 까지 반복
				WINDOWPLACEMENT windowState;


				//두번째 데스크탑 윈도우 스위처 계산 //
				list<E_Window*> secondWinlist;
				E_Desktop* desktop = global->getSelectedDesktop();
				for (list<E_Desktop*>::iterator iterDesktop = global->desktopList.begin(); iterDesktop != global->desktopList.end(); iterDesktop++){
					if (*iterDesktop != desktop){
						std::list<E_Window*> winlist = (*iterDesktop)->getWindowList();
						for (std::list<E_Window*>::reverse_iterator iter = winlist.rbegin(); iter != winlist.rend(); iter++) {
							secondWinlist.push_back(*iter);
						}
					}
				}
				//정렬
				global->sortZIndexWindowList(secondWinlist);

				long secondMaxWidthCount = 1;
				long secondMaxHeightCount = 1;

				int secondWindowSize = secondWinlist.size();
				if (secondWindowSize >= 7)
					secondMaxWidthCount = 7;
				else
					secondMaxWidthCount = secondWindowSize;

				if (secondWindowSize >= 8)
					secondMaxHeightCount = 2;
				else
					secondMaxHeightCount = 1;

				long secondSwitcherWidth = aeroWidth * secondMaxWidthCount + paddingSize * 2;;
				long secondSwitcherHeight = titleSize + aeroHeight * secondMaxHeightCount + paddingSize * 2; //스위치 이름 높이 나중에 추가 필요
				long secondSwitcherLeft = 0;				//윈도우 기준 오프셋	
				long secondSwitcherTop = 0;					//CDC기준으로 항상 0
				
				//크기 계산을 위한 카운트 변수
				int secondCount = 0;
				int secondWidthCount = 0;//0~6 까지 반복
				int secondHeightCount = 0;//0~1 까지 반복

				if (secondWindowSize == 0){ secondSwitcherWidth = 0; secondSwitcherHeight = 0; }

				//전체 데스크탑 스위처 계산
				long allSwitcherWidth = secondSwitcherWidth > switcherWidth ? secondSwitcherWidth : switcherWidth;	//더 큰 스위처의 비율을 따라간다.
				long allSwitcherHeight = switcherHeight + bottomPadding + secondSwitcherHeight;
				long allswitcherLeft = resWidth / 2 - allSwitcherWidth / 2;
				long allswitcherTop = resHeight / 2 - switcherHeight / 2;


				//모든 스위처 왼쪽 오프셋 초기화
				switcherLeft = allSwitcherWidth / 2 - switcherWidth / 2;
				secondSwitcherLeft = allSwitcherWidth / 2 - secondSwitcherWidth / 2;
				
				//스위처 위치 이동
				this->SetWindowPos(NULL
					, allswitcherLeft
					, allswitcherTop
					, allSwitcherWidth, allSwitcherHeight
					, SWP_NOZORDER | SWP_SHOWWINDOW);

				// 더블 버퍼링을 위한 코드
				//메모리 DC를 생성한다. (버퍼 메모리 할당)
				CDC memDC;
				//그릴 Bitmap을 생성한다. (한번에 그릴 도화지 정도 개념)
				CBitmap bmp;
				//메모리 DC를 위의 CPaintDC인 dc에 호환되게 만들어 준다.
				memDC.CreateCompatibleDC(&dc);
				//주어진 dc에 호환하는 비트맵을 생성한다.
				bmp.CreateCompatibleBitmap(&dc, switcherWidth, switcherHeight);
				//이제 memDC에 생성된 비트맵을 연결한다.
				memDC.SelectObject(bmp);

				
				// 더블 버퍼링을 위한 코드
				//메모리 DC를 생성한다. (버퍼 메모리 할당)
				CDC secondMemDC;
				//그릴 Bitmap을 생성한다. (한번에 그릴 도화지 정도 개념)
				CBitmap secondBmp;
				//메모리 DC를 위의 CPaintDC인 dc에 호환되게 만들어 준다.
				secondMemDC.CreateCompatibleDC(&dc);
				//주어진 dc에 호환하는 비트맵을 생성한다.
				secondBmp.CreateCompatibleBitmap(&dc, secondSwitcherWidth, secondSwitcherHeight);
				//이제 memDC에 생성된 비트맵을 연결한다.
				secondMemDC.SelectObject(secondBmp);

				//현재 데스크탑 배경 색
				{
					CBitmap *cbm = getBackgroundCBitmap(switcherWidth, switcherHeight);
					CBrush brush;
					//brush.CreateSolidBrush(E_WindowSwitcher::aeroColor);
					brush.CreatePatternBrush(cbm);
					RECT rect;
					rect.left = 0;
					rect.top = 0;
					rect.right = switcherWidth;
					rect.bottom = switcherHeight;
					memDC.FillRect(&rect, &brush);
					brush.DeleteObject();
					cbm->DeleteObject();
				}
				
				//다른 데스크탑 배경
				{
					if (secondSwitcherWidth != 0 && secondSwitcherHeight != 0){
						CBitmap *cbm = getBackgroundCBitmap(secondSwitcherWidth, secondSwitcherHeight);
						CBrush brush;
						//brush.CreateSolidBrush(E_WindowSwitcher::aeroColor);
						brush.CreatePatternBrush(cbm);
						RECT rect;
						rect.left = 0;
						rect.top = 0;
						rect.right = secondSwitcherWidth;
						rect.bottom = secondSwitcherHeight;
						secondMemDC.FillRect(&rect, &brush);
						brush.DeleteObject();
						cbm->DeleteObject();
					}
				}
				  
				//첫번째 데스크탑에 업데이트가 존재하는지?
				int existUpdateMode = 0;
				for (list<E_Window*>::reverse_iterator iter = winlist.rbegin(); iter != winlist.rend(); iter++){
					//mode_map.key
					if (mode_map.find((*iter)->getWindow()) == mode_map.end() || (IsWindow((*iter)->getWindow()) == FALSE)){
						continue;
					}
					unordered_map<HWND, DRAWMODE>::iterator modeIter = mode_map.find((*iter)->getWindow());
					DRAWMODE mode = modeIter->second;
					if (mode == UPDATE_TOUCH){
						existUpdateMode = 1;
					}
				}


				//첫번째 데스크탑에 업데이트가 존재하는지?
				int secondExistUpdateMode = 0;
				for (list<E_Window*>::reverse_iterator iter = secondWinlist.rbegin(); iter != secondWinlist.rend(); iter++){
					//mode_map.key
					if (mode_map.find((*iter)->getWindow()) == mode_map.end() || (IsWindow((*iter)->getWindow()) == FALSE)){
						continue;
					}
					unordered_map<HWND, DRAWMODE>::iterator modeIter = mode_map.find((*iter)->getWindow());
					DRAWMODE mode = modeIter->second;
					if (mode == UPDATE_TOUCH){
						secondExistUpdateMode = 1;
					}
				}
				
				int emptyforIndex = 0; 

				//첫번째 데스크탑
				for (list<E_Window*>::reverse_iterator iter = winlist.rbegin(); iter != winlist.rend(); iter++){
					//mode_map.key
					if (mode_map.find((*iter)->getWindow()) == mode_map.end() || (IsWindow((*iter)->getWindow()) == FALSE)){
						TRACE_WIN32A("[invalid window]");
						continue;
					}
					//윈도우 개수가 14개 이상일 때 제외
					if (emptyforIndex++ < startTaboffset && tabMode == SELECTEDDESKTOP){
						continue;
					}
					
					unordered_map<HWND, DRAWMODE>::iterator modeIter = mode_map.find((*iter)->getWindow());
					DRAWMODE mode = modeIter->second;
					CWnd* cwnd = CWnd::FromHandle((*iter)->getWindow());
					cwnd->GetWindowPlacement(&windowState);

					CString windowName;
					cwnd->GetWindowTextW(windowName);
					if (wcscmp(windowName.GetBuffer(), L"Program Manager") == 0)
						windowName = L"바탕화면"; 

					//TRACE_WIN32(L"[E_WindowSwitcher::OnPaint] %s\t\t[state]: %d", windowName.GetBuffer(), windowState.showCmd);

					long aeroLeftoffset = paddingSize + (aeroWidth * widthCount); //윈도우 별로 위치가 달라짐!!!
					long aeroTopoffset = titleSize + paddingSize + (aeroHeight * heightCount); //스위치 이름 높이 나중에 추가 필요 //윈도우 별로 위치가 달라짐!!!

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
						pen.CreatePen(PS_SOLID, 2, E_WindowSwitcher::borderColor);
					else if (mode == UPDATE_TAB)
						pen.CreatePen(PS_SOLID, 2, E_WindowSwitcher::borderColorSelected);
					else if (mode == UPDATE_TOUCH)
						pen.CreatePen(PS_SOLID, 2, E_WindowSwitcher::borderColorSelectedMouse);

					//배경 그리기
					//dc.Rectangle(temprect);
					//memDC.FillRect(&temprect, &brush1);

					//탭 경계선 그리기
					if (tabIndex == count && tabMode == SELECTEDDESKTOP || windowSize == 1) {	//바탕 화면만 있을 경우도 고려
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

					//터치 경계선 그리기
					if (mode == UPDATE_TOUCH) {
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

					//타이틀 그리기
					if (tabIndex == count && existUpdateMode == 0 && tabMode == SELECTEDDESKTOP || windowSize == 1) {	//바탕 화면만 있을 경우도 고려
						RECT fontRect;
						fontRect.top = paddingSize;
						fontRect.left = paddingSize;
						fontRect.right = switcherWidth - fontRect.left * 2;
						fontRect.bottom = fontRect.top + titleFontSize;
						SetBkMode(memDC, TRANSPARENT);
						memDC.SelectObject(font);
						memDC.DrawText(windowName, &fontRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
					}
					else if (mode == UPDATE_TOUCH && existUpdateMode == 1) {
						RECT fontRect;
						fontRect.top = paddingSize;
						fontRect.left = paddingSize;
						fontRect.right = switcherWidth - fontRect.left * 2;
						fontRect.bottom = fontRect.top + titleFontSize;
						SetBkMode(memDC, TRANSPARENT);
						memDC.SelectObject(font);
						memDC.DrawText(windowName, &fontRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
						//SetBkMode(memDC, OPAQUE);
					}

					//aero 기준 오프셋
					static long previewLeftoffset = paddingSize;	//실제 aero 크기 (aero box와 preview box 사이 크기)
					static long previewTopoffset = paddingSize;	//실제 aero 크기 (aero box와 preview box 사이 크기)

					//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] aeroLeftoffset: %d aeroTopoffset: %d previewLeftoffset: %d previewTopoffset: %d", aeroLeftoffset, aeroTopoffset, previewLeftoffset, previewTopoffset);

					CRect crect = { 0, 0, 0, 0 };

					//하나의 프리뷰 박스 기준 축소된 윈도우 크기
					long windowWidth = 0;
					long windowHeight = 0;
					long windowTopOffset = 0;
					long windowLeftOffset = 0;

					CBitmap* screenshot;
					BITMAP bmpinfo;             //비트맵은 높이와 크기가 달라서
					bool isAero = (*iter)->isAeroPossible();	//모두 AERO 모드
					unordered_map<HWND, HTHUMBNAIL>::iterator validKey = thumb_map.find(cwnd->GetSafeHwnd());
					if (isAero && (validKey != thumb_map.end()))
						cwnd->GetWindowRect(crect);
					else{
						screenshot = (*iter)->getScreenshot();
						screenshot->GetBitmap(&bmpinfo);
						crect.SetRect(0, 0, bmpinfo.bmWidth, bmpinfo.bmHeight);
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


					//스크린샷 좌표 (cdc 좌표계)
					RECT rect;
					rect.top = aeroTopoffset + previewTopoffset + windowTopOffset;
					rect.left = aeroLeftoffset + previewLeftoffset + windowLeftOffset;
					rect.right = rect.left + windowWidth;
					rect.bottom = rect.top + windowHeight;

					//에어로 좌표 (윈도우 좌표계)
					RECT rectForAero;
					rectForAero.left = aeroLeftoffset + previewLeftoffset + windowLeftOffset + switcherLeft;
					rectForAero.top = aeroTopoffset + previewTopoffset + windowTopOffset;
					rectForAero.right = rectForAero.left + windowWidth;
					rectForAero.bottom = rectForAero.top + windowHeight;
					
					//전체 스위처 기준 이벤트 좌표 (윈도우 좌표계)
					CRect rectForEvent;
					rectForEvent.top = aeroTopoffset;
					rectForEvent.left = aeroLeftoffset + switcherLeft;	//왼쪽 오프셋
					rectForEvent.bottom = temprect.top + aeroHeight;
					rectForEvent.right = temprect.left + aeroWidth + switcherLeft; //왼쪽 오프셋

					rect_map.insert(unordered_map<HWND, RECT>::value_type(cwnd->GetSafeHwnd(), rectForEvent));
					
					//에어로픽이나 스크린샷
					if (isAero && (validKey != thumb_map.end())) {
						//에어로가 가능할때
						E_AeroPeekController::getSingleton()->moveAero(validKey->second, rectForAero);
					}
					else{
						CDC cdc;
						cdc.CreateCompatibleDC(&dc);
						cdc.SelectObject(screenshot->GetSafeHandle());
						/*cdc.SetBkMode(1);
						cdc.SetBkColor(E_WindowSwitcher::backgroundColor);*/
						memDC.SetStretchBltMode(COLORONCOLOR);
						memDC.StretchBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, &cdc, 0, 0, bmpinfo.bmWidth, bmpinfo.bmHeight, SRCCOPY);
						cdc.DeleteDC();
					}

					//아이콘
					CWnd* iconCWnd = NULL;
					if (icon_map.find(cwnd->GetSafeHwnd()) != icon_map.end()){
						iconCWnd = icon_map.find(cwnd->GetSafeHwnd())->second;
						CBitmap* icon = (*iter)->getIcon();
						BITMAP icon_info;
						int iconpadding = 2;
						if (icon->m_hObject != NULL){
							icon->GetBitmap(&icon_info);

							RECT windowRect;

							windowRect.left = allswitcherLeft + rectForAero.right - icon_info.bmWidth;
							windowRect.top = allswitcherTop + rectForAero.bottom - icon_info.bmHeight;
							windowRect.bottom = allswitcherTop + rectForAero.bottom - iconpadding;
							windowRect.right = allswitcherLeft + rectForAero.right - iconpadding;
							iconCWnd->MoveWindow(&windowRect);
							CDC* iconDC = iconCWnd->GetDC();
							/*SetBkColor(*iconDC, RGB(255, 255, 255));
							SetBkMode(*iconDC, TRANSPARENT);*/
							
							CDC cdc; 
							cdc.CreateCompatibleDC(iconDC);
							cdc.SelectObject(icon);
							//iconDC->SetStretchBltMode(COLORONCOLOR);
							//iconDC->bitbit
							iconDC->TransparentBlt(0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, 0xffffffff);
							//iconDC->StretchBlt(0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, SRCCOPY);
							cdc.DeleteDC();
							ReleaseDC(iconDC);
						}
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
				
				emptyforIndex = 0;

				//두번째 데스크탑
				for (list<E_Window*>::reverse_iterator iter = secondWinlist.rbegin(); iter != secondWinlist.rend(); iter++){
					//mode_map.key
					if (mode_map.find((*iter)->getWindow()) == mode_map.end() || (IsWindow((*iter)->getWindow()) == FALSE)){
						continue;
					}

					//윈도우 개수가 14개 이상일 때 제외
					if (emptyforIndex++ < startTaboffset && tabMode == SELECTEDDESKTOP){
						continue;
					}

					unordered_map<HWND, DRAWMODE>::iterator modeIter = mode_map.find((*iter)->getWindow());
					DRAWMODE mode = modeIter->second;
					CWnd* cwnd = CWnd::FromHandle((*iter)->getWindow());
					cwnd->GetWindowPlacement(&windowState);
					
					CString title;
					wchar_t windowName[255] = { 0, };
					cwnd->GetWindowTextW(windowName, 255);

					title.Format(L"%s #%d", windowName, desktop_map.find((*iter)->getWindow())->second->getIndex());

					//TRACE_WIN32(L"[E_WindowSwitcher::OnPaint] %s\t\t[state]: %d", windowName, windowState.showCmd);

					long aeroLeftoffset = paddingSize + (aeroWidth * secondWidthCount); //윈도우 별로 위치가 달라짐!!!
					long aeroTopoffset = titleSize + paddingSize + (aeroHeight * secondHeightCount); //스위치 이름 높이 나중에 추가 필요 //윈도우 별로 위치가 달라짐!!!


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
						pen.CreatePen(PS_SOLID, 2, E_WindowSwitcher::borderColor);
					else if (mode == UPDATE_TAB)
						pen.CreatePen(PS_SOLID, 2, E_WindowSwitcher::borderColorSelected);
					else if (mode == UPDATE_TOUCH)
						pen.CreatePen(PS_SOLID, 2, E_WindowSwitcher::borderColorSelectedMouse);
					
					//배경 그리기
					//dc.Rectangle(temprect);
					//secondMemDC.FillRect(&temprect, &brush1);

					//탭 경계선 그리기
					if (tabIndex == secondCount && tabMode == OTHERDESKTOP) {
						secondMemDC.SelectObject(pen);
						::InflateRect(temprect, 1, 1);
						int padding = 2;
						secondMemDC.MoveTo(temprect.left + padding, temprect.top + padding);
						secondMemDC.LineTo(temprect.right - padding, temprect.top + padding);
						secondMemDC.MoveTo(temprect.right - padding, temprect.top + padding);
						secondMemDC.LineTo(temprect.right - padding, temprect.bottom - padding);
						secondMemDC.MoveTo(temprect.left + padding, temprect.top + padding);
						secondMemDC.LineTo(temprect.left + padding, temprect.bottom - padding);
						secondMemDC.MoveTo(temprect.left + padding, temprect.bottom - padding);
						secondMemDC.LineTo(temprect.right - padding, temprect.bottom - padding);
					}

					//경계선 그리기
					if (mode == UPDATE_TAB || mode == UPDATE_TOUCH) {
						secondMemDC.SelectObject(pen);
						::InflateRect(temprect, 1, 1);
						int padding = 2;
						secondMemDC.MoveTo(temprect.left + padding, temprect.top + padding);
						secondMemDC.LineTo(temprect.right - padding, temprect.top + padding);
						secondMemDC.MoveTo(temprect.right - padding, temprect.top + padding);
						secondMemDC.LineTo(temprect.right - padding, temprect.bottom - padding);
						secondMemDC.MoveTo(temprect.left + padding, temprect.top + padding);
						secondMemDC.LineTo(temprect.left + padding, temprect.bottom - padding);
						secondMemDC.MoveTo(temprect.left + padding, temprect.bottom - padding);
						secondMemDC.LineTo(temprect.right - padding, temprect.bottom - padding);
					}


					//타이틀 그리기
					if (tabIndex == secondCount && secondExistUpdateMode == 0 && tabMode == OTHERDESKTOP){
						RECT fontRect;
						fontRect.top = paddingSize;
						fontRect.left = paddingSize;
						fontRect.right = secondSwitcherWidth - fontRect.left * 2;
						fontRect.bottom = fontRect.top + titleFontSize;
						SetBkMode(secondMemDC, TRANSPARENT);
						secondMemDC.SelectObject(font);
						secondMemDC.DrawText(windowName, &fontRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
						//SetBkMode(memDC, OPAQUE);
					}
					else if (mode == UPDATE_TOUCH && secondExistUpdateMode == 1) {
						RECT fontRect;
						fontRect.top = paddingSize;
						fontRect.left = paddingSize;
						fontRect.right = secondSwitcherWidth - fontRect.left * 2;
						fontRect.bottom = fontRect.top + titleFontSize;
						SetBkMode(secondMemDC, TRANSPARENT);
						secondMemDC.SelectObject(font);
						secondMemDC.DrawText(windowName, &fontRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
						//SetBkMode(memDC, OPAQUE);
					}

					//aero 기준 오프셋
					static long previewLeftoffset = paddingSize;	//실제 aero 크기 (aero box와 preview box 사이 패딩)
					static long previewTopoffset = paddingSize;	//실제 aero 크기 (aero box와 preview box 사이 패딩)

					//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] aeroLeftoffset: %d aeroTopoffset: %d previewLeftoffset: %d previewTopoffset: %d", aeroLeftoffset, aeroTopoffset, previewLeftoffset, previewTopoffset);

					CRect crect = { 0, 0, 0, 0 };

					//하나의 프리뷰 박스 기준 축소된 윈도우 크기
					long windowWidth = 0;
					long windowHeight = 0;
					long windowTopOffset = 0;
					long windowLeftOffset = 0;

					CBitmap* screenshot;
					BITMAP bmpinfo;             //비트맵은 높이와 크기가 달라서
					bool isAero = true; //(*iter)->isAeroPossible(); //에어로 모드
					unordered_map<HWND, HTHUMBNAIL>::iterator validKey = thumb_map.find(cwnd->GetSafeHwnd());
					if (isAero && (validKey != thumb_map.end()))
						cwnd->GetWindowRect(crect);
					else{
						screenshot = (*iter)->getScreenshot();
						screenshot->GetBitmap(&bmpinfo);
						crect.SetRect(0, 0, bmpinfo.bmWidth, bmpinfo.bmHeight);
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
					
					//스크린샷 좌표 (cdc 좌표계)
					RECT rect;
					rect.top = aeroTopoffset + previewTopoffset + windowTopOffset;
					rect.left = aeroLeftoffset + previewLeftoffset + windowLeftOffset;
					rect.right = rect.left + windowWidth;
					rect.bottom = rect.top + windowHeight;

					//에어로 좌표 (윈도우 좌표계)
					RECT rectForAero;
					rectForAero.top = aeroTopoffset + previewTopoffset + windowTopOffset + switcherHeight + bottomPadding;	//위 스위처로 부터 떨어져있음
					rectForAero.left = aeroLeftoffset + previewLeftoffset + windowLeftOffset + secondSwitcherLeft;
					rectForAero.right = rectForAero.left + windowWidth;
					rectForAero.bottom = rectForAero.top + windowHeight;


					//전체 스위처 기준 이벤트 좌표 (윈도우 좌표계)
					CRect rectForEvent;
					rectForEvent.top = aeroTopoffset + switcherHeight + bottomPadding;
					rectForEvent.left = aeroLeftoffset + secondSwitcherLeft;	//왼쪽 오프셋
					rectForEvent.bottom = rectForEvent.top + aeroHeight;
					rectForEvent.right = rectForEvent.left + aeroWidth; //왼쪽 오프셋

					rect_map.insert(unordered_map<HWND, RECT>::value_type(cwnd->GetSafeHwnd(), rectForEvent));
					
					//에어로픽이나 스크린샷
					if (isAero && (validKey != thumb_map.end())) {
						//에어로가 가능할때
						E_AeroPeekController::getSingleton()->moveAero(validKey->second, rectForAero);
					}
					else{
						CDC cdc;
						cdc.CreateCompatibleDC(&dc);
						cdc.SelectObject(screenshot->GetSafeHandle());
						/*cdc.SetBkMode(1);
						cdc.SetBkColor(E_WindowSwitcher::backgroundColor);*/
						secondMemDC.SetStretchBltMode(COLORONCOLOR);
						secondMemDC.StretchBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, &cdc, 0, 0, bmpinfo.bmWidth, bmpinfo.bmHeight, SRCCOPY);
						cdc.DeleteDC();
					}

					//아이콘
					/*CBitmap* icon = (*iter)->getIcon();
					BITMAP icon_info;
					if (icon->m_hObject != NULL){
						icon->GetBitmap(&icon_info);
						CDC cdc;
						cdc.CreateCompatibleDC(&dc);
						cdc.SelectObject(icon);
						secondMemDC.SetStretchBltMode(COLORONCOLOR);
						secondMemDC.TransparentBlt(rect.right - icon_info.bmWidth, rect.bottom - icon_info.bmHeight, icon_info.bmWidth, icon_info.bmHeight, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, 0xffffffff);
						cdc.DeleteDC();
					}*/


					//아이콘
					int iconSize = getIconSize(E_EnvironmentManager::getSingleton()->getWidth());
					CWnd* iconCWnd = NULL;
					if (icon_map.find(cwnd->GetSafeHwnd()) != icon_map.end()){
						iconCWnd = icon_map.find(cwnd->GetSafeHwnd())->second;
						//CPaintDC iconDC(iconCWnd);
						CBitmap* icon = (*iter)->getIcon();
						BITMAP icon_info;
						int iconpadding = 2;
						if (icon->m_hObject != NULL){
							icon->GetBitmap(&icon_info);

							RECT windowRect;
							windowRect.left = allswitcherLeft + rectForAero.right - icon_info.bmWidth;
							windowRect.top = allswitcherTop + rectForAero.bottom - icon_info.bmHeight;
							windowRect.bottom = allswitcherTop + rectForAero.bottom - iconpadding;
							windowRect.right = allswitcherLeft + rectForAero.right - iconpadding;
							iconCWnd->MoveWindow(&windowRect);

							CDC* iconDC = iconCWnd->GetDC();
						/*	SetBkColor(*iconDC, RGB(255, 255, 255));
							SetBkMode(*iconDC, TRANSPARENT);*/

							CDC cdc;
							cdc.CreateCompatibleDC(iconDC);
							cdc.SelectObject(icon);
							iconDC->SetStretchBltMode(COLORONCOLOR);
							//iconDC->StretchBlt(0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, &cdc,0,0, icon_info.bmWidth, icon_info.bmHeight, SRCCOPY);

							iconDC->TransparentBlt(0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, 0xffffffff);
							cdc.DeleteDC();
							iconCWnd->ShowWindow(SW_SHOW);
							//iconDC->DeleteDC();
						}
					}

					//업데이트 플래그
					mode_map.find((*iter)->getWindow())->second = DRAW_NORMAL;

					//위치 조정
					secondWidthCount = (secondCount + 1) % 7;
					secondCount++;
					if (secondCount >= 7)
						secondHeightCount = 1; //0 ~ 1 사이
					//테스트 코드
					if (secondCount >= 14)
						break;


					brush1.DeleteObject();
					pen.DeleteObject();
				}
				
				//버퍼에 있는 내용 한번에 그리기
				dc.StretchBlt(switcherLeft, 0, switcherWidth, switcherHeight, &memDC, 0, 0, switcherWidth, switcherHeight, SRCCOPY);
				dc.StretchBlt(secondSwitcherLeft, switcherHeight + bottomPadding, secondSwitcherWidth, secondSwitcherHeight
					, &secondMemDC
					, 0, 0, secondSwitcherWidth, secondSwitcherHeight
					, SRCCOPY); 
				
				//dc 해제
				font.DeleteObject();
				memDC.DeleteDC();
				bmp.DeleteObject();

				secondMemDC.DeleteDC();
				secondBmp.DeleteObject();

				//윈도우 리스트
				temp_windowlist.clear();
				temp_secondwindowlist.clear();
				temp_windowlist = winlist;
				temp_secondwindowlist = secondWinlist;
			}
		}
	}
	else {
		//icon size...
	}
	//update 일 경우 처리
	/*if (updateFlag == true){
		restartSwitcher();
		Invalidate(0);
		updateFlag = false;
		}*/
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

	return res_width / 192;
}


// 창의 모양을 알 수 있다.
SHAPE E_WindowSwitcher::getShape(int width, int height, int res_width, int res_height)
{
	double ratio = (double)res_width / (double)res_height;
	double targetRatio = (double)width / (double)height;
	SHAPE shape;
	if (targetRatio > ratio) {
		shape = HORIZONTAL;
	}
	else {
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
	//TRACE_WIN32A("x: %d, y: %d", point.x, point.y);
	for (unordered_map<HWND, RECT>::iterator itr = rect_map.begin(); itr != rect_map.end(); itr++){
		RECT rect = itr->second;
		HWND hwnd = itr->first;
		if (rect.left < point.x && rect.right > point.x && rect.top < point.y && rect.bottom > point.y) {
			if (IsWindow(itr->first) && group_map.find(itr->first) != group_map.end()){
				stopTPMode();
				if (group_map.find(itr->first)->second == SELECTEDDESKTOP){
					WINDOWPLACEMENT windowState;

					char title[255] = { 0, };
					::GetWindowTextA(itr->first, title, 255);
					::GetWindowPlacement(itr->first, &windowState);
					//TRACE_WIN32A("[OnLButtonDown] title: %s showCmd: %d", title, windowState.showCmd);

					hwnd = itr->first;	//지역 변수 사용

					//LOSS FOCUS
					if (strcmp(title, "Program Manager") == 0){
						E_Global::getSingleton()->getSelectedDesktop()->setAllMinimize();
					}
					else{
						if (::IsIconic(hwnd) == TRUE)
							::ShowWindow(hwnd, SW_RESTORE);
						
							::BringWindowToTop(hwnd);

						focushwnd = hwnd;
						SetTimer(2, 50, NULL);
					}
					TRACE_WIN32A("[E_WindowSwitcher::OnLButtonDown] SELECTEDDESKTOP");
				}
				else if (group_map.find(itr->first)->second == OTHERDESKTOP) {
					//소속 데스크탑
					E_Desktop* desktop = desktop_map.find(hwnd)->second;
					
					//그 데스크탑으로 이동
					int index = 0;
					E_Global* global = E_Global::getSingleton();

					hwnd = itr->first;	//지역 변수 사용
					
					//LOSS FOCUS
					global->moveDesktop(desktop->getIndex());

					WINDOWPLACEMENT windowState;

					char title[255] = { 0, };
					::GetWindowTextA(hwnd, title, 255);
					::GetWindowPlacement(hwnd, &windowState);
					//TRACE_WIN32A("[OnLButtonDown] title: %s showCmd: %d", title, windowState.showCmd);

					if (::IsIconic(hwnd) == TRUE)
						::ShowWindow(hwnd, SW_RESTORE);

					::BringWindowToTop(hwnd);

					focushwnd = hwnd;
					SetTimer(2, 5, NULL);
					//::SendMessage(E_Global::getSingleton()->hwnd_frame, WM_TRAY_EVENT, desktop->getIndex(), 0);
				}
				terminateSwitcher();
				TRACE_WIN32A("[E_WindowSwitcher::OnLButtonDown] terminateSwitcher()");
				break;
			}
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}


void E_WindowSwitcher::selectTabWindow()
{
	//TRACE_WIN32A("[GROUP] %d", group_map.size());
	HWND hwnd = NULL;
	int loop = tabIndex + startTaboffset;
	int offset = 0;
	if (tabMode == SELECTEDDESKTOP){
		for (list<E_Window*>::reverse_iterator iter = temp_windowlist.rbegin(); iter != temp_windowlist.rend(); iter++){
			if (loop == offset){
				hwnd = (*iter)->getWindow();
				break;
			}
			offset++;
		}
	}
	else{
		for (list<E_Window*>::reverse_iterator iter = temp_secondwindowlist.rbegin(); iter != temp_secondwindowlist.rend(); iter++){
			if (loop == offset){
				hwnd = (*iter)->getWindow();
				break;
			}
			offset++;
		}
	}
	//TRACE_WIN32A("[GROUP BEFORE] %d", group_map.size());
	if (IsWindow(hwnd) && (group_map.find(hwnd) != group_map.end())){
		stopTPMode();
		//TRACE_WIN32A("[GROUP AFTER] %d", group_map.size());
		if (group_map.find(hwnd)->second == SELECTEDDESKTOP){
			WINDOWPLACEMENT windowState;

			char title[255] = { 0, };
			::GetWindowTextA(hwnd, title, 255);
			::GetWindowPlacement(hwnd, &windowState);
			//TRACE_WIN32A("[OnLButtonDown] title: %s showCmd: %d", title, windowState.showCmd);

			hwnd = hwnd;	//지역 변수 사용

			//LOSS FOCUS
			if (strcmp(title, "Program Manager") == 0){
				E_Global::getSingleton()->getSelectedDesktop()->setAllMinimize();
			}
			else{

				if (::IsIconic(hwnd) == TRUE)
					::ShowWindow(hwnd, SW_RESTORE);
				::BringWindowToTop(hwnd);

				focushwnd = hwnd;
				SetTimer(2, 50, NULL);
			}
		}
		else if (group_map.find(hwnd)->second == OTHERDESKTOP) {
			//소속 데스크탑
			E_Desktop* desktop = desktop_map.find(hwnd)->second;

			//그 데스크탑으로 이동
			int index = 0;
			E_Global* global = E_Global::getSingleton();

			hwnd = hwnd;	//지역 변수 사용

			//LOSS FOCUS
			global->moveDesktop(desktop->getIndex());

			WINDOWPLACEMENT windowState;

			char title[255] = { 0, };
			::GetWindowTextA(hwnd, title, 255);
			::GetWindowPlacement(hwnd, &windowState);
			//TRACE_WIN32A("[OnLButtonDown] title: %s showCmd: %d", title, windowState.showCmd);

			if (::IsIconic(hwnd) == TRUE)
				::ShowWindow(hwnd, SW_RESTORE);
			::BringWindowToTop(hwnd);
			
			::SendMessage(E_Global::getSingleton()->hwnd_frame, WM_TRAY_EVENT, desktop->getIndex(), 0);
			focushwnd = hwnd;
			SetTimer(2, 50, NULL);
		}
	}
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



BOOL E_WindowSwitcher::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return __super::OnEraseBkgnd(pDC);
}


void E_WindowSwitcher::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//TRACE_WIN32A("OnKeyDown %c",nChar);

	if (isRunning() == true){
		switch (nChar){
		case VK_RIGHT:
		{
						 selectNextWindow();
		}
			break;
		case VK_LEFT:
		{

						selectPrevWindow();
		}
			break;
		case  VK_TAB:
		{
						bool shift = GetKeyState(VK_LSHIFT) < 0 ? true : false;
						if (shift == false){
							selectNextWindow();
						}
						else{
							selectPrevWindow();

						}
		}
			break;
		case VK_ESCAPE:
		{
						  terminateSwitcher();
		}
			break;
		case VK_RETURN:
		{
						  //스위처가 동작중이 아닐 때
						  selectTabWindow();
						  terminateSwitcher();
		}
			break;
		case VK_OEM_3:
			//토글
			selectOtherDesktop();
		}
	}

	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}


void E_WindowSwitcher::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}


void E_WindowSwitcher::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus(pNewWnd);

	lock_guard<std::mutex> lock(E_Mutex::windowSwitcherEvent);
	if (running==true)
		terminateSwitcher();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void E_WindowSwitcher::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//TRACE_WIN32A("E_WindowSwitcher::OnSysKeyDown %d", nChar);

	if (nChar == 192 && running){
		selectOtherDesktop();
	}
	__super::OnSysKeyDown(nChar, nRepCnt, nFlags);
}


void E_WindowSwitcher::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//TRACE_WIN32A("E_WindowSwitcher::OnSysKeyUp %d", nChar);
	
	
	__super::OnSysKeyUp(nChar, nRepCnt, nFlags);
}


// 다음 윈도우로 이동
void E_WindowSwitcher::selectNextWindow()
{
	//에어로
	E_Global* global = E_Global::getSingleton();
	E_AeroPeekController* aero = E_AeroPeekController::getSingleton();

	RECT winRect;
	winRect.left = 0;
	winRect.top = 0;
	winRect.bottom = 1;
	winRect.right = 1;
	

	//두번째 데스크탑 윈도우 스위처 계산 //
	list<E_Window*> secondWinlist;
	E_Desktop* desktop = global->getSelectedDesktop();
	for (list<E_Desktop*>::iterator iterDesktop = global->desktopList.begin(); iterDesktop != global->desktopList.end(); iterDesktop++){
		if (*iterDesktop != desktop){
			std::list<E_Window*> winlist = (*iterDesktop)->getWindowList();
			for (std::list<E_Window*>::reverse_iterator iter = winlist.rbegin(); iter != winlist.rend(); iter++) {
				secondWinlist.push_back(*iter);
				//icon_map.find((*iter)->getWindow())->second->MoveWindow(&winRect,0);
			}
		}
	}
	list<E_Window*> winlist =global->getSelectedDesktop()->getWindowList();
	winlist.push_front(E_Global::getSingleton()->backgroundWindow);

	int selectedSize = winlist.size();//데스크탑 포함
	int otherSize = secondWinlist.size();

	//현재 데스크탑
	if (tabMode == SELECTEDDESKTOP){
		TRACE_WIN32A("[SELECTEDDESKTOP BEFORE tabIndex] %d", tabIndex);
		tabIndex++;
		
		if (tabIndex + startTaboffset >= selectedSize){
			tabIndex = 0;
			startTaboffset = 0;
			resetIconcwndAndAero();
		}
		else if (tabIndex >= 14){
			startTaboffset++; //tabIndex - 14 + 1;
			tabIndex = 13;
			resetIconcwndAndAero();
		}
		TRACE_WIN32A("[SELECTEDDESKTOP tabIndex] %d", tabIndex);
	}
	
	//다른 데스크탑
	else if (tabMode == OTHERDESKTOP){
		TRACE_WIN32A("[OTHERDESKTOP BEFORE tabIndex] %d", tabIndex);
		tabIndex++;

		if (tabIndex + startTaboffset >= otherSize){
			TRACE_WIN32A("[OTHERDESKTOP ING tabIndex] %d", tabIndex);
			tabIndex = 0;
			startTaboffset = 0;
			resetIconcwndAndAero();
		}
		else if (tabIndex >= 14){
			startTaboffset++; //tabIndex - 14 + 1;
			tabIndex = 13;
			resetIconcwndAndAero();
		}
		TRACE_WIN32A("[OTHERDESKTOP AFTER tabIndex] %d", tabIndex);
	}
	Invalidate(0);
	if (startTaboffset > 0){
		Invalidate(1);
	}
}

// 이전 윈도우로 이동
void E_WindowSwitcher::selectPrevWindow()
{
	E_Global* global = E_Global::getSingleton();

	//두번째 데스크탑 윈도우 스위처 계산 //
	list<E_Window*> secondWinlist;
	E_Desktop* desktop = global->getSelectedDesktop();
	for (list<E_Desktop*>::iterator iterDesktop = global->desktopList.begin(); iterDesktop != global->desktopList.end(); iterDesktop++){
		if (*iterDesktop != desktop){
			std::list<E_Window*> winlist = (*iterDesktop)->getWindowList();
			for (std::list<E_Window*>::reverse_iterator iter = winlist.rbegin(); iter != winlist.rend(); iter++) {
				secondWinlist.push_back(*iter);
			}
		}
	}

	int selectedSize = global->getSelectedDesktop()->getWindowList().size() + 1;
	int otherSize = secondWinlist.size();

	//현재 데스크탑
	if (tabMode == SELECTEDDESKTOP){
		tabIndex--;
		
		if (tabIndex < 0 && startTaboffset == 0){
			if (selectedSize>14)
				tabIndex = 13;
			else
				tabIndex = selectedSize - 1;//selectedSize - 1;
			if (selectedSize > 14)
				startTaboffset = selectedSize - 14;
			else
				startTaboffset = 0;
			resetIconcwndAndAero();
		}
		else if (tabIndex < 0){
			tabIndex = 0;
			startTaboffset--;
			resetIconcwndAndAero();
		}
	}

	//다른 데스크탑
	else if (tabMode == OTHERDESKTOP){
		tabIndex--;

		if (tabIndex < 0 && startTaboffset == 0){
			if (otherSize>14)
				tabIndex = 13;
			else
				tabIndex = otherSize - 1;//selectedSize - 1;
			if (selectedSize > 14)
				startTaboffset = otherSize - 14;
			else
				startTaboffset = 0;
			resetIconcwndAndAero();
		}
		else if (tabIndex < 0){
			tabIndex = 0;
			startTaboffset--;
			resetIconcwndAndAero();
		}
	}
	Invalidate(0);
}

// 다른 데스크탑으로 토글
void E_WindowSwitcher::selectOtherDesktop()
{
	if (tabMode == SELECTEDDESKTOP && temp_secondwindowlist.size() != 0){
		tabMode = OTHERDESKTOP;
		tabIndex = 0;
		startTaboffset = 0;
	}
	else if (tabMode == OTHERDESKTOP){
		tabMode = SELECTEDDESKTOP;
		tabIndex = 1;
		startTaboffset = 0;
	}
	Invalidate(0);
}


void E_WindowSwitcher::startTPMode() 
{
	if (!tpmode && E_AeroPeekController::getSingleton()->isAeroPeekMode()){
		E_Window::SetMinimizeMaximizeAnimation(false);
		E_Global* global = E_Global::getSingleton();
		E_Desktop* selectedDesktop = global->getSelectedDesktop();

		for (list<E_Desktop*>::iterator iterDesktop = global->desktopList.begin(); iterDesktop != global->desktopList.end(); iterDesktop++){
			if (selectedDesktop == *iterDesktop){
				(*iterDesktop)->setAllTransParentExclude();	//tp 모드로 만듬
				//모두 보여줌 (SW_NORMAL(원래의 상태로 보여줌))
				(*iterDesktop)->setAllNormalExcludeRemoveDirty(aero_exclude_winlist);	//비주얼 제외
				continue;
			}
			(*iterDesktop)->setAllIconInvisible();
			//투명
			(*iterDesktop)->setAllTransParentExclude();

			//창 모양 유지용
			(*iterDesktop)->setAllShow();	//창 위치 복구
			(*iterDesktop)->setAllSaveShowState();	//창 위치 저장
			(*iterDesktop)->setAllHide();			//창 위치 숨기기

			//모두 보여줌 (SW_NORMAL(원래의 상태로 보여줌))
			(*iterDesktop)->setAllNormalExcludeRemoveDirty(aero_exclude_winlist);	//비주얼 제외

			//현재 창 포커스
			//::SetFocus((*selectedDesktop->getWindowList().rbegin())->getWindow());
		}
		E_Window::SetMinimizeMaximizeAnimation(true);
		tpmode = true;
	}
}


void E_WindowSwitcher::stopTPMode()
{
	if (tpmode && E_AeroPeekController::getSingleton()->isAeroPeekMode()){
		bool saveState = running;	//순간적으로 포커스 잃는 문제 때문에 처리
		running = false; // 투명을 제거할때는 잠시 오프
		E_Window::SetMinimizeMaximizeAnimation(false);
		E_Global* global = E_Global::getSingleton();

		//최소화
		E_Desktop* selectedDesktop = global->getSelectedDesktop();
		std::list<E_Window*> winlist = selectedDesktop->getWindowList();
		selectedDesktop->setAllMinimizeTransparentRemoveDirty();	//tp모드 해제
		
		for (list<E_Desktop*>::iterator iterDesktop = global->desktopList.begin(); iterDesktop != global->desktopList.end(); iterDesktop++){
			if (*iterDesktop == selectedDesktop)
				continue;


			//창 모양 유지용
			(*iterDesktop)->setAllRestoreSavedShowState(); //원래 창 위치 복구 후 
			(*iterDesktop)->removeDirtyAfter();			//잔상 제거 후 처리
			(*iterDesktop)->setAllHide();					//숨김
			(*iterDesktop)->setAllIconVisible();			//아이콘 보여 줌

			//창 숨기기
			(*iterDesktop)->setAllOpaque();
		}
		E_Window::SetMinimizeMaximizeAnimation(true);
		running = saveState;
		tpmode = false;
	}
}


CWnd* E_WindowSwitcher::createChild()
{
	int iconsize = getIconSize(E_EnvironmentManager::getSingleton()->getWidth());
	RECT winRect;
	winRect.left = 0;
	winRect.top = 0;
	winRect.bottom = 1;
	winRect.right = 1;
	
	CBrush brush_map;
	//brush_map.CreateStockObject(NULL_BRUSH);
	brush_map.CreateSolidBrush(this->backgroundColor);

	CWnd* cwnd = new CWnd;
	UINT nClassStyle_window = 0;
	CString szClassName_window = AfxRegisterWndClass(nClassStyle_window, 0, (HBRUSH)brush_map.GetSafeHandle(), 0);
	cwnd->CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_window, L"icon", WS_VISIBLE | WS_POPUP, winRect, this, 0, NULL);
	cwnd->UpdateWindow();
	
	//CREATE
	brush_map.DeleteObject();

	return cwnd;
}

void E_WindowSwitcher::stealFocus(HWND hwnd)
{
	DWORD dwCurrentThread = GetCurrentThreadId();
	DWORD dwFGThread = GetWindowThreadProcessId(::GetForegroundWindow(), NULL);

	AttachThreadInput(dwCurrentThread, dwFGThread, TRUE);
	if (hwnd == NULL)
		HWND hwnd = E_WindowSwitcher::getSingleton()->m_hWnd;
	// Possible actions you may wan to bring the window into focus.
	::SetForegroundWindow(hwnd);
	::SetCapture(hwnd);
	::SetFocus(hwnd);
	::SetActiveWindow(hwnd);
	::EnableWindow(hwnd, TRUE);

	AttachThreadInput(dwCurrentThread, dwFGThread, FALSE);
}


void E_WindowSwitcher::stealFocus2(HWND parm_dest_wnd)
{
	if (NULL != parm_dest_wnd && ::IsWindow(parm_dest_wnd)){
		CWnd *p_prev_wnd = CWnd::FromHandle(parm_dest_wnd);

		if (NULL != p_prev_wnd){
			p_prev_wnd->BringWindowToTop();

			CWnd *p_child_wnd = p_prev_wnd->GetLastActivePopup();
			if (p_prev_wnd->IsIconic() == TRUE) p_prev_wnd->ShowWindow(SW_RESTORE);
		//	p_prev_wnd->ShowWindow(SW_SHOWNORMAL);

			if (p_child_wnd != NULL && p_prev_wnd != p_child_wnd){
				p_child_wnd->BringWindowToTop();
				p_child_wnd->SetForegroundWindow();
			}
			else{
				p_prev_wnd->BringWindowToTop();
				p_prev_wnd->SetForegroundWindow();
			}
		}
	}
}

// 아이콘 윈도우와 에어로를 초기화 하는 함수,
void E_WindowSwitcher::resetIconcwndAndAero()
{
	//에어로
	E_Global* global = E_Global::getSingleton();
	E_AeroPeekController* aero = E_AeroPeekController::getSingleton();

	RECT winRect;
	winRect.left = 0;
	winRect.top = 0;
	winRect.bottom = 1;
	winRect.right = 1;

	list<E_Window*> winlist = global->getSelectedDesktop()->getWindowList();
	winlist.push_front(E_Global::getSingleton()->backgroundWindow);


	//두번째 데스크탑 윈도우 스위처 계산 //
	list<E_Window*> secondWinlist;
	E_Desktop* desktop = global->getSelectedDesktop();
	for (list<E_Desktop*>::iterator iterDesktop = global->desktopList.begin(); iterDesktop != global->desktopList.end(); iterDesktop++){
		if (*iterDesktop != desktop){
			std::list<E_Window*> winlist = (*iterDesktop)->getWindowList();
			for (std::list<E_Window*>::reverse_iterator iter = winlist.rbegin(); iter != winlist.rend(); iter++) {
				secondWinlist.push_back(*iter);
				icon_map.find((*iter)->getWindow())->second->MoveWindow(&winRect, 0);
			}
		}
	}

	//아이콘 리셋 / 에어로 리셋
	for (std::list<E_Window*>::reverse_iterator iter = winlist.rbegin(); iter != winlist.rend(); iter++){
		unordered_map<HWND, CWnd*>::iterator iconIter = icon_map.find((*iter)->getWindow());
		if (iconIter != icon_map.end()){
			iconIter->second->MoveWindow(&winRect, 0);
		}
		unordered_map<HWND, HTHUMBNAIL>::iterator thumbIter = thumb_map.find((*iter)->getWindow());
		if (thumbIter != thumb_map.end()){
			HTHUMBNAIL handle = thumbIter->second;
			aero->moveAero(handle, winRect);
		}
	}
	for (std::list<E_Window*>::reverse_iterator iter = secondWinlist.rbegin(); iter != secondWinlist.rend(); iter++) {
		unordered_map<HWND, CWnd*>::iterator iconIter = icon_map.find((*iter)->getWindow());
		if (iconIter != icon_map.end()){
			iconIter->second->MoveWindow(&winRect, 0);
		}
		unordered_map<HWND, HTHUMBNAIL>::iterator thumbIter = thumb_map.find((*iter)->getWindow());
		if (thumbIter != thumb_map.end()){
			HTHUMBNAIL handle = thumbIter->second;
			aero->moveAero(handle, winRect);
		}
	}
}

CBitmap* E_WindowSwitcher::getBackgroundCBitmap(long width, long height)
{
	HBITMAP hbmOrig;
	bool squeezed = width / height > 5 ? TRUE : FALSE;
	if (squeezed)
	{
		hbmOrig = (HBITMAP)LoadImage(NULL, __T("WindowSwitcher_background3.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	}
	else
	{
		hbmOrig = (HBITMAP)LoadImage(NULL, __T("WindowSwitcher_background3.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	}
	
	BITMAP bm = { 0 };
	GetObject(hbmOrig, sizeof(BITMAP), &bm);

	HDC dc = ::GetDC(::GetDesktopWindow());
	HDC memdc = CreateCompatibleDC(dc);
	HDC memdc2 = CreateCompatibleDC(dc);
	HBITMAP hbm = CreateCompatibleBitmap(dc, width, height);
	HGDIOBJ hOld = SelectObject(memdc, hbm);
	SelectObject(memdc2, hbmOrig);
	StretchBlt(memdc, 0, 0, width, height, memdc2, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	
	SelectObject(memdc, hOld);
	DeleteDC(memdc);
	DeleteDC(memdc2);
	
	::ReleaseDC(::GetDesktopWindow(), dc);

	DeleteObject(hbmOrig);	//비트맵 해제


	return CBitmap::FromHandle(hbm);
}


void E_WindowSwitcher::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//TRACE_WIN32A("E_WindowSwitcher::OnTimer()");
	if (nIDEvent == 1)
	{
		if (!isfocus)
		{
			isfocus = true;
			
			//동작 중
			running = true;
			
			//윈도우를 띄움
			this->ShowWindow(SW_SHOW);
			this->stealFocus2(this->GetSafeHwnd());
			::SetFocus(focushwnd);
		
			KillTimer(1);
		}
	}
	if (nIDEvent == 2){
		//TRACE_WIN32A("FOCUS - 2");
		char name[255];
		GetWindowTextA(focushwnd, name, 255);
		TRACE_WIN32A("FOCUS PROCESS NAME %u ,%s", focushwnd,name);

		::ShowWindow(focushwnd, SW_SHOW);
		this->stealFocus2(focushwnd);
		::SetFocus(focushwnd);

		KillTimer(2);
	}
	__super::OnTimer(nIDEvent);
}
