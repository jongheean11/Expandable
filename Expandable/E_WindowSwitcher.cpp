#include "stdafx.h"
#include "E_WindowSwitcher.h"

const COLORREF E_WindowSwitcher::backgroundColor = RGB(0x37, 0xb6, 0xeb);
const COLORREF E_WindowSwitcher::aeroColor = RGB(0x40, 0xc0, 0xef);
const COLORREF E_WindowSwitcher::aeroColorSelected = RGB(0x30, 0xb0, 0xe5);
const COLORREF E_WindowSwitcher::aeroColorSelectedMouse = RGB(0x30, 0xb0, 0xee);
const COLORREF E_WindowSwitcher::borderColor = RGB(0xdc, 0xdb, 0xdb);
const COLORREF E_WindowSwitcher::borderColorSelected = RGB(0xcc, 0xcc, 0xcc);
const COLORREF E_WindowSwitcher::borderColorSelectedMouse = RGB(0xcf, 0xdc, 0xdc);

E_WindowSwitcher* E_WindowSwitcher::singleton = NULL;
const wchar_t* E_WindowSwitcher::caption = L"WindowSwitcher";

void E_WindowSwitcher::updateSelectedDesktop()
{
	if (running){
		this->SetWindowPos(NULL	//���� ������ �ܻ� ����
			, 0
			, 0
			, 1, 1
			, SWP_NOZORDER);
		restartSwitcher();
		Invalidate(0);
	}
	//������Ʈ�� �߻��� ��� �ڵ����� ȣ���
	//TRACE_WIN32A("[E_WindowSwitcher::updateSelectedDesktop()]");
}

E_WindowSwitcher::E_WindowSwitcher() : running(false), updateFlag(false)
{
	envManager = E_EnvironmentManager::getSingleton();

	UINT nClassStyle_window = 0;// CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	
	CBrush brush_map;
	//brush_map.CreateSolidBrush(E_WindowSwitcher::backgroundColor);
	brush_map.CreateStockObject(NULL_BRUSH);
	CString szClassName_window = AfxRegisterWndClass(nClassStyle_window, 0, (HBRUSH)brush_map.GetSafeHandle(), 0);
	this->CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_window, E_WindowSwitcher::caption, WS_VISIBLE | WS_POPUP, CRect(0, 0, 1, 1), CWnd::GetDesktopWindow(), 0);
	this->ShowWindow(SW_HIDE);
	this->UpdateWindow();
	brush_map.DeleteObject();
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
	tabIndex = 1;
	E_AeroPeekController* aeroManager = E_AeroPeekController::getSingleton();
	E_Global* global = E_Global::getSingleton();
	//global->startUpdate();

	RECT r = { 0, 0, 0, 0 };
	HWND hwnd = NULL;
	HTHUMBNAIL hthumbnail = NULL;
	//���� ������Ʈ
	global->onUpdate();

	//AERO���
	startTPMode();

	//���� ���� ���
	E_Desktop* desktop = global->getSelectedDesktop();
	for (list<E_Desktop*>::iterator iterDesktop = global->desktopList.begin(); iterDesktop != global->desktopList.end(); iterDesktop++){
		std::list<E_Window*> winlist = (*iterDesktop)->getWindowList();
		for (std::list<E_Window*>::iterator iter = winlist.begin(); iter != winlist.end(); iter++) {

			icon_map.insert(unordered_map<HWND, CWnd*>::value_type(hwnd, createChild()));

			hwnd = (*iter)->getWindow();
			mode_map.insert(unordered_map<HWND, DRAWMODE>::value_type(hwnd, DRAW_NORMAL));
			if (SUCCEEDED(aeroManager->registerAero(hwnd, this->GetSafeHwnd(), r, hthumbnail))){ // && (*iter)->isAeroPossible()) {
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
	
	//����ũž
	E_Window* desktopWindow = global->backgroundWindow;
	mode_map.insert(unordered_map<HWND, DRAWMODE>::value_type(desktopWindow->getWindow(), DRAW_NORMAL));
	if (SUCCEEDED(aeroManager->registerAero(desktopWindow->getWindow(), this->GetSafeHwnd(), r, hthumbnail)) && desktopWindow->isAeroPossible()) {
		thumb_map.insert(unordered_map<HWND, HTHUMBNAIL>::value_type(desktopWindow->getWindow(), hthumbnail));
		group_map.insert(unordered_map<HWND, GROUP2>::value_type(desktopWindow->getWindow(), SELECTEDDESKTOP));
	}

	this->ShowWindow(SW_SHOW);
	running = true;
}


// UI�� ������ â�� ������ �Լ�
void E_WindowSwitcher::terminateSwitcher()
{
	if (running) {
		stopTPMode();
		running = false;
		//ũ��Ƽ�� ����?
		E_AeroPeekController* aeroManager = E_AeroPeekController::getSingleton();
		//E_Global::getSingleton()->stopUpdate();

		HRESULT result;
		this->ShowWindow(SW_HIDE);

		//�ڿ� ����
		for (unordered_map<HWND, HTHUMBNAIL>::iterator iter = thumb_map.begin(); iter != thumb_map.end(); iter++) {
			result = aeroManager->unregisterAero(iter->second);
			if (SUCCEEDED(result)) {
				//TRACE_WIN32A("[E_WindowSwitcher::terminateSwitcher] RELEASE OK");
			}
			else {
				//TRACE_WIN32A("[E_WindowSwitcher::terminateSwitcher] RELEASE FAIL");
			}
		}
		//������ ����
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

		this->SetWindowPos(NULL	//���� ������ �ܻ� ����
			, 0
			, 0
			, 1, 1
			, SWP_NOZORDER);
	}
}

// ����ó�� ������ϴ� �Լ�
void E_WindowSwitcher::restartSwitcher()
{
	//���� ���� ���
	stopTPMode();
	
	HRESULT result;
	E_AeroPeekController* aeroManager = E_AeroPeekController::getSingleton();
	E_Global* global = E_Global::getSingleton();
	//�ڿ� ����
	for (unordered_map<HWND, HTHUMBNAIL>::iterator iter = thumb_map.begin(); iter != thumb_map.end(); iter++) {
		result = aeroManager->unregisterAero(iter->second);
		if (SUCCEEDED(result)) {
			//TRACE_WIN32A("[E_WindowSwitcher::terminateSwitcher] RELEASE OK");
		}
		else {
			//TRACE_WIN32A("[E_WindowSwitcher::terminateSwitcher] RELEASE FAIL");
		}
	}

	//������ ����
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

	//�����ϴ� �κ�

	//���� ������Ʈ
	global->onUpdate();

	RECT r = { 0, 0, 0, 0 };
	HWND hwnd = NULL;
	HTHUMBNAIL hthumbnail = NULL;

	//���� ���� ���
	startTPMode();

	E_Desktop* desktop = global->getSelectedDesktop();
	for (list<E_Desktop*>::iterator iterDesktop = global->desktopList.begin(); iterDesktop != global->desktopList.end(); iterDesktop++){
		std::list<E_Window*> winlist = (*iterDesktop)->getWindowList();
		for (std::list<E_Window*>::iterator iter = winlist.begin(); iter != winlist.end(); iter++) {
			hwnd = (*iter)->getWindow();
			mode_map.insert(unordered_map<HWND, DRAWMODE>::value_type(hwnd, DRAW_NORMAL));
			if (SUCCEEDED(aeroManager->registerAero(hwnd, this->GetSafeHwnd(), r, hthumbnail))){ // && (*iter)->isAeroPossible()) {
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

	//����ũž
	E_Window* desktopWindow = global->backgroundWindow;
	mode_map.insert(unordered_map<HWND, DRAWMODE>::value_type(desktopWindow->getWindow(), DRAW_NORMAL));
	if (SUCCEEDED(aeroManager->registerAero(desktopWindow->getWindow(), this->GetSafeHwnd(), r, hthumbnail)) && desktopWindow->isAeroPossible()) {
		thumb_map.insert(unordered_map<HWND, HTHUMBNAIL>::value_type(desktopWindow->getWindow(), hthumbnail));
		group_map.insert(unordered_map<HWND, GROUP2>::value_type(desktopWindow->getWindow(), SELECTEDDESKTOP));
	}

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
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_KILLFOCUS()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
END_MESSAGE_MAP()

/*â�� ���� �׸��� �Լ�*/
void E_WindowSwitcher::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	static long resWidth = envManager->getWidth();
	static long resHeight = envManager->getHeight();

	E_Global* global = E_Global::getSingleton();
	//TRACE_WIN32A("[E_WindowSwitcher::OnPaint]resWidth: %d, resHeight: %d", resWidth, resHeight);
	if (E_AeroPeekController::getSingleton()->isAeroPeekMode()) {

		//aero peek size...
		//��ü ����ũž ���� ����...
		{
			long aeroWidth = getAeroWidthSize(resWidth);	//�е� ���� aeroũ��
			long aeroHeight = getAeroHeightSize(resHeight); // �е� ���� aeroũ��
			long paddingSize = getPaddingSize(resWidth);	// �е��� ũ��
			long titleSize = paddingSize * 3;				// Ÿ��Ʋ�� ũ��
			long titleFontSize = paddingSize * 2;				// �۾� ���� ũ��
			long previewWidth = aeroWidth - paddingSize * 2;	//���� aero ũ��
			long previewHeight = aeroHeight - paddingSize * 2;	//���� aero ũ��
			long bottomPadding = paddingSize * 6;

			//TRACE_WIN32A("[E_WindowSwitcher::OnPaint]��ü ����ũž ���뺯�� aeroWidth: %d paddingSize: %d, previewWidth: %d", aeroWidth, paddingSize, previewWidth);
			{
				//Ÿ��Ʋ ��
				CFont* pOldFont;
				CFont font;
				LOGFONT lf;
				::ZeroMemory(&lf, sizeof(lf));
				wsprintf(lf.lfFaceName, TEXT("%s"), TEXT("Arial"));
				lf.lfHeight = titleFontSize;
				//lf.lfWeight = 550;
				//��Ʈ ����
				font.CreateFontIndirectW(&lf);

				//ù��° ����ũž ������ ����ó ��� //
				long maxWidthCount = 1;
				long maxHeightCount = 1;

				list<E_Window*> winlist = E_Global::getSingleton()->getSelectedDesktop()->getWindowList();
				winlist.push_front(global->backgroundWindow);	//����ȭ�� ����

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
				long switcherHeight = titleSize + aeroHeight * maxHeightCount + paddingSize * 2; //����ġ �̸� ���� ���߿� �߰� �ʿ�
				long switcherLeft = 0;				//������ ���� ������				
				long switcherTop = 0;				//CDC�������� �׻� 0

				if (windowSize == 0){ switcherWidth = 0; switcherHeight = 0; }

				//ũ�� ����� ���� ī��Ʈ ����
				int count = 0;
				int widthCount = 0;//0~6 ���� �ݺ�
				int heightCount = 0;//0~1 ���� �ݺ�
				WINDOWPLACEMENT windowState;


				//�ι�° ����ũž ������ ����ó ��� //
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
				//����
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
				long secondSwitcherHeight = titleSize + aeroHeight * secondMaxHeightCount + paddingSize * 2; //����ġ �̸� ���� ���߿� �߰� �ʿ�
				long secondSwitcherLeft = 0;				//������ ���� ������	
				long secondSwitcherTop = 0;					//CDC�������� �׻� 0
				
				//ũ�� ����� ���� ī��Ʈ ����
				int secondCount = 0;
				int secondWidthCount = 0;//0~6 ���� �ݺ�
				int secondHeightCount = 0;//0~1 ���� �ݺ�

				if (secondWindowSize == 0){ secondSwitcherWidth = 0; secondSwitcherHeight = 0; }

				//��ü ����ũž ����ó ���
				long allSwitcherWidth = secondSwitcherWidth > switcherWidth ? secondSwitcherWidth : switcherWidth;	//�� ū ����ó�� ������ ���󰣴�.
				long allSwitcherHeight = switcherHeight + bottomPadding + secondSwitcherHeight;
				long allswitcherLeft = resWidth / 2 - allSwitcherWidth / 2;
				long allswitcherTop = resHeight / 2 - switcherHeight / 2;


				//��� ����ó ���� ������ �ʱ�ȭ
				switcherLeft = allSwitcherWidth / 2 - switcherWidth / 2;
				secondSwitcherLeft = allSwitcherWidth / 2 - secondSwitcherWidth / 2;
				
				//����ó ��ġ �̵�
				this->SetWindowPos(NULL
					, allswitcherLeft
					, allswitcherTop
					, allSwitcherWidth, allSwitcherHeight
					, SWP_NOZORDER | SWP_SHOWWINDOW);

				// ���� ���۸��� ���� �ڵ�
				//�޸� DC�� �����Ѵ�. (���� �޸� �Ҵ�)
				CDC memDC;
				//�׸� Bitmap�� �����Ѵ�. (�ѹ��� �׸� ��ȭ�� ���� ����)
				CBitmap bmp;
				//�޸� DC�� ���� CPaintDC�� dc�� ȣȯ�ǰ� ����� �ش�.
				memDC.CreateCompatibleDC(&dc);
				//�־��� dc�� ȣȯ�ϴ� ��Ʈ���� �����Ѵ�.
				bmp.CreateCompatibleBitmap(&dc, switcherWidth, switcherHeight);
				//���� memDC�� ������ ��Ʈ���� �����Ѵ�.
				memDC.SelectObject(bmp);

				
				// ���� ���۸��� ���� �ڵ�
				//�޸� DC�� �����Ѵ�. (���� �޸� �Ҵ�)
				CDC secondMemDC;
				//�׸� Bitmap�� �����Ѵ�. (�ѹ��� �׸� ��ȭ�� ���� ����)
				CBitmap secondBmp;
				//�޸� DC�� ���� CPaintDC�� dc�� ȣȯ�ǰ� ����� �ش�.
				secondMemDC.CreateCompatibleDC(&dc);
				//�־��� dc�� ȣȯ�ϴ� ��Ʈ���� �����Ѵ�.
				secondBmp.CreateCompatibleBitmap(&dc, secondSwitcherWidth, secondSwitcherHeight);
				//���� memDC�� ������ ��Ʈ���� �����Ѵ�.
				secondMemDC.SelectObject(secondBmp);

				//���� ����ũž ��� ��
				{
					CBrush brush;
					brush.CreateSolidBrush(E_WindowSwitcher::aeroColor);
					RECT rect;
					rect.left = 0;
					rect.top = 0;
					rect.right = switcherWidth;
					rect.bottom = switcherHeight;
					memDC.FillRect(&rect, &brush);
					brush.DeleteObject();
				}

				//�ٸ� ����ũž ��� 
				{
					CBrush brush;
					brush.CreateSolidBrush(E_WindowSwitcher::aeroColor);
					RECT rect;
					rect.left = 0;
					rect.top = 0;
					rect.right = secondSwitcherWidth;
					rect.bottom = secondSwitcherHeight;
					secondMemDC.FillRect(&rect, &brush);
					brush.DeleteObject();
				}

				//ù��° ����ũž�� ������Ʈ�� �����ϴ���?
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


				//ù��° ����ũž�� ������Ʈ�� �����ϴ���?
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

				//ù��° ����ũž
				for (list<E_Window*>::reverse_iterator iter = winlist.rbegin(); iter != winlist.rend(); iter++){
					//mode_map.key
					if (mode_map.find((*iter)->getWindow()) == mode_map.end() || (IsWindow((*iter)->getWindow()) == FALSE)){
						continue;
					}
					unordered_map<HWND, DRAWMODE>::iterator modeIter = mode_map.find((*iter)->getWindow());
					DRAWMODE mode = modeIter->second;
					CWnd* cwnd = CWnd::FromHandle((*iter)->getWindow());
					cwnd->GetWindowPlacement(&windowState);

					CString windowName;
					cwnd->GetWindowTextW(windowName);
					if (wcscmp(windowName.GetBuffer(), L"Program Manager") == 0)
						windowName = L"����ȭ��"; 

					//TRACE_WIN32(L"[E_WindowSwitcher::OnPaint] %s\t\t[state]: %d", windowName.GetBuffer(), windowState.showCmd);

					long aeroLeftoffset = paddingSize + (aeroWidth * widthCount); //������ ���� ��ġ�� �޶���!!!
					long aeroTopoffset = titleSize + paddingSize + (aeroHeight * heightCount); //����ġ �̸� ���� ���߿� �߰� �ʿ� //������ ���� ��ġ�� �޶���!!!


					//Aero ���� �簢�� �귯��
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


					//��輱 �귯��
					CPen pen;
					if (mode == DRAW_NORMAL)
						pen.CreatePen(PS_SOLID, 1, E_WindowSwitcher::borderColor);
					else if (mode == UPDATE_TAB)
						pen.CreatePen(PS_SOLID, 1, E_WindowSwitcher::borderColorSelected);
					else if (mode == UPDATE_TOUCH)
						pen.CreatePen(PS_SOLID, 1, E_WindowSwitcher::borderColorSelectedMouse);

					//��� �׸���
					//dc.Rectangle(temprect);
					memDC.FillRect(&temprect, &brush1);

					//�� ��輱 �׸���
					if (tabIndex == count) {
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

					//��ġ ��輱 �׸���
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

					//Ÿ��Ʋ �׸���
					if (tabIndex == count && existUpdateMode == 0){
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

					//aero ���� ������
					static long previewLeftoffset = paddingSize;	//���� aero ũ�� (aero box�� preview box ���� ũ��)
					static long previewTopoffset = paddingSize;	//���� aero ũ�� (aero box�� preview box ���� ũ��)

					//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] aeroLeftoffset: %d aeroTopoffset: %d previewLeftoffset: %d previewTopoffset: %d", aeroLeftoffset, aeroTopoffset, previewLeftoffset, previewTopoffset);

					CRect crect = { 0, 0, 0, 0 };

					//�ϳ��� ������ �ڽ� ���� ��ҵ� ������ ũ��
					long windowWidth = 0;
					long windowHeight = 0;
					long windowTopOffset = 0;
					long windowLeftOffset = 0;

					CBitmap* screenshot;
					BITMAP bmpinfo;             //��Ʈ���� ���̿� ũ�Ⱑ �޶�
					bool isAero = true;	//(*iter)->isAeroPossible();	//��� AERO ���
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

					//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] â ratio: %lf windowWidth: %d windowHeight: %d windowTopOffset: %d windowLeftOffset: %d", ratio, windowWidth, windowHeight, windowTopOffset, windowLeftOffset);


					//��ũ���� ��ǥ (cdc ��ǥ��)
					RECT rect;
					rect.top = aeroTopoffset + previewTopoffset + windowTopOffset;
					rect.left = aeroLeftoffset + previewLeftoffset + windowLeftOffset;
					rect.right = rect.left + windowWidth;
					rect.bottom = rect.top + windowHeight;

					//����� ��ǥ (������ ��ǥ��)
					RECT rectForAero;
					rectForAero.top = aeroTopoffset + previewTopoffset + windowTopOffset;
					rectForAero.left = aeroLeftoffset + previewLeftoffset + windowLeftOffset + switcherLeft;
					rectForAero.right = rectForAero.left + windowWidth;
					rectForAero.bottom = rectForAero.top + windowHeight;
					
					//��ü ����ó ���� �̺�Ʈ ��ǥ (������ ��ǥ��)
					CRect rectForEvent;
					rectForEvent.top = aeroTopoffset;
					rectForEvent.left = aeroLeftoffset + switcherLeft;	//���� ������
					rectForEvent.bottom = temprect.top + aeroHeight;
					rectForEvent.right = temprect.left + aeroWidth + switcherLeft; //���� ������

					rect_map.insert(unordered_map<HWND, RECT>::value_type(cwnd->GetSafeHwnd(), rectForEvent));

					//��������̳� ��ũ����
					if (isAero && (validKey != thumb_map.end())) {
						//����ΰ� �����Ҷ�
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

					//������
					/*CBitmap* icon = (*iter)->getIcon();
					BITMAP icon_info;
					if (icon->m_hObject != NULL){
						icon->GetBitmap(&icon_info);
						CDC cdc;
						cdc.CreateCompatibleDC(&dc);
						cdc.SelectObject(icon);
						memDC.SetStretchBltMode(COLORONCOLOR);
						memDC.TransparentBlt(rect.right - icon_info.bmWidth, rect.bottom - icon_info.bmHeight, icon_info.bmWidth, icon_info.bmHeight, &cdc, 0, 0, icon_info.bmWidth, icon_info.bmHeight, 0xffffffff);
						cdc.DeleteDC();
					}*/

					//������Ʈ �÷���
					mode_map.find((*iter)->getWindow())->second = DRAW_NORMAL;

					//��ġ ����
					widthCount = (count + 1) % 7;
					count++;
					if (count >= 7)
						heightCount = 1; //0 ~ 1 ����
					//�׽�Ʈ �ڵ�
					if (count >= 14)
						break;


					brush1.DeleteObject();
					pen.DeleteObject();
				}
				
				//�ι�° ����ũž
				for (list<E_Window*>::reverse_iterator iter = secondWinlist.rbegin(); iter != secondWinlist.rend(); iter++){
					//mode_map.key
					if (mode_map.find((*iter)->getWindow()) == mode_map.end() || (IsWindow((*iter)->getWindow()) == FALSE)){
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

					//TRACE_WIN32(L"[E_WindowSwitcher::OnPaint] %s\t\t[state]: %d", windowName.GetBuffer(), windowState.showCmd);

					long aeroLeftoffset = paddingSize + (aeroWidth * secondWidthCount); //������ ���� ��ġ�� �޶���!!!
					long aeroTopoffset = titleSize + paddingSize + (aeroHeight * secondHeightCount); //����ġ �̸� ���� ���߿� �߰� �ʿ� //������ ���� ��ġ�� �޶���!!!


					//Aero ���� �簢�� �귯��
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


					//��輱 �귯��
					CPen pen;
					if (mode == DRAW_NORMAL)
						pen.CreatePen(PS_SOLID, 1, E_WindowSwitcher::borderColor);
					else if (mode == UPDATE_TAB)
						pen.CreatePen(PS_SOLID, 1, E_WindowSwitcher::borderColorSelected);
					else if (mode == UPDATE_TOUCH)
						pen.CreatePen(PS_SOLID, 1, E_WindowSwitcher::borderColorSelectedMouse);

					//��� �׸���
					//dc.Rectangle(temprect);
					secondMemDC.FillRect(&temprect, &brush1);

					//��輱 �׸���
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


					//Ÿ��Ʋ �׸���
					if (tabIndex == count + windowSize && secondExistUpdateMode == 0){
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

					//aero ���� ������
					static long previewLeftoffset = paddingSize;	//���� aero ũ�� (aero box�� preview box ���� �е�)
					static long previewTopoffset = paddingSize;	//���� aero ũ�� (aero box�� preview box ���� �е�)

					//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] aeroLeftoffset: %d aeroTopoffset: %d previewLeftoffset: %d previewTopoffset: %d", aeroLeftoffset, aeroTopoffset, previewLeftoffset, previewTopoffset);

					CRect crect = { 0, 0, 0, 0 };

					//�ϳ��� ������ �ڽ� ���� ��ҵ� ������ ũ��
					long windowWidth = 0;
					long windowHeight = 0;
					long windowTopOffset = 0;
					long windowLeftOffset = 0;

					CBitmap* screenshot;
					BITMAP bmpinfo;             //��Ʈ���� ���̿� ũ�Ⱑ �޶�
					bool isAero = true; //(*iter)->isAeroPossible(); //����� ���
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

					//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] â ratio: %lf windowWidth: %d windowHeight: %d windowTopOffset: %d windowLeftOffset: %d", ratio, windowWidth, windowHeight, windowTopOffset, windowLeftOffset);
					
					//��ũ���� ��ǥ (cdc ��ǥ��)
					RECT rect;
					rect.top = aeroTopoffset + previewTopoffset + windowTopOffset;
					rect.left = aeroLeftoffset + previewLeftoffset + windowLeftOffset;
					rect.right = rect.left + windowWidth;
					rect.bottom = rect.top + windowHeight;

					//����� ��ǥ (������ ��ǥ��)
					RECT rectForAero;
					rectForAero.top = aeroTopoffset + previewTopoffset + windowTopOffset + switcherHeight + bottomPadding;	//�� ����ó�� ���� ����������
					rectForAero.left = aeroLeftoffset + previewLeftoffset + windowLeftOffset + secondSwitcherLeft;
					rectForAero.right = rectForAero.left + windowWidth;
					rectForAero.bottom = rectForAero.top + windowHeight;


					//��ü ����ó ���� �̺�Ʈ ��ǥ (������ ��ǥ��)
					CRect rectForEvent;
					rectForEvent.top = aeroTopoffset + switcherHeight + bottomPadding;
					rectForEvent.left = aeroLeftoffset + secondSwitcherLeft;	//���� ������
					rectForEvent.bottom = rectForEvent.top + aeroHeight;
					rectForEvent.right = rectForEvent.left + aeroWidth; //���� ������

					rect_map.insert(unordered_map<HWND, RECT>::value_type(cwnd->GetSafeHwnd(), rectForEvent));
					
					//��������̳� ��ũ����
					if (isAero && (validKey != thumb_map.end())) {
						//����ΰ� �����Ҷ�
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

					//������
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

					//������Ʈ �÷���
					mode_map.find((*iter)->getWindow())->second = DRAW_NORMAL;

					//��ġ ����
					secondWidthCount = (secondCount + 1) % 7;
					secondCount++;
					if (secondCount >= 7)
						secondHeightCount = 1; //0 ~ 1 ����
					//�׽�Ʈ �ڵ�
					if (secondCount >= 14)
						break;


					brush1.DeleteObject();
					pen.DeleteObject();
				}


				//���ۿ� �ִ� ���� �ѹ��� �׸���

				dc.StretchBlt(switcherLeft, 0, switcherWidth, switcherHeight, &memDC, 0, 0, switcherWidth, switcherHeight, SRCCOPY);
				dc.StretchBlt(secondSwitcherLeft, switcherHeight + bottomPadding, secondSwitcherWidth, secondSwitcherHeight
					, &secondMemDC
					, 0, 0, secondSwitcherWidth, secondSwitcherHeight
					, SRCCOPY); 
				
				//dc ����
				font.DeleteObject();
				memDC.DeleteDC();
				bmp.DeleteObject();

				secondMemDC.DeleteDC();
				secondBmp.DeleteObject();
			}
		}
	}
	else {
		//icon size...
	}
	//update �� ��� ó��
	/*if (updateFlag == true){
		restartSwitcher();
		Invalidate(0);
		updateFlag = false;
		}*/
	// �׸��� �޽����� ���ؼ��� CWnd::OnPaint()��(��) ȣ������ ���ʽÿ�.
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

	return res_width / 192;
}


// â�� ����� �� �� �ִ�.
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
	//lock_guard<std::mutex> lock(E_Mutex::windowSwitcherEvent);
	//E_Global::getSingleton()->stopUpdate();
	HWND hwnd = NULL;	//���� ���� ����� ���� iterating ���� ���� ��ȸ
	TRACE_WIN32A("x: %d, y: %d", point.x, point.y);
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
					TRACE_WIN32A("[OnLButtonDown] title: %s showCmd: %d", title, windowState.showCmd);

					hwnd = itr->first;	//���� ���� ���

					//LOSS FOCUS
					if (strcmp(title, "Program Manager") == 0){
						E_Global::getSingleton()->getSelectedDesktop()->setAllMinimize();
					}
					else{
						if (::IsIconic(hwnd) == true)
							::ShowWindow(hwnd, SW_RESTORE);
						
					/*	if (::GetForegroundWindow() != hwnd){
							HWND h_active_wnd = ::GetForegroundWindow();
							if (h_active_wnd != NULL){
								DWORD thread_id = GetWindowThreadProcessId(h_active_wnd, NULL);
							c	DWORD current_thread_id = GetCurrentThreadId();
								if (current_thread_id != thread_id){
									if (AttachThreadInput(current_thread_id, thread_id, TRUE)){
										BringWindowToTop();
										AttachThreadInput(current_thread_id, thread_id, FALSE);
									}
								}
							}
						}*/

						::BringWindowToTop(hwnd);
					}
				}
				else if (group_map.find(itr->first)->second == OTHERDESKTOP) {
					//�Ҽ� ����ũž
					E_Desktop* desktop = desktop_map.find(hwnd)->second;
					
					//�� ����ũž���� �̵�
					int index = 0;
					E_Global* global = E_Global::getSingleton();

					hwnd = itr->first;	//���� ���� ���

					//LOSS FOCUS
					global->moveDesktop(desktop->getIndex());

					WINDOWPLACEMENT windowState;

					char title[255] = { 0, };
					::GetWindowTextA(hwnd, title, 255);
					::GetWindowPlacement(hwnd, &windowState);
					TRACE_WIN32A("[OnLButtonDown] title: %s showCmd: %d", title, windowState.showCmd);

					if (::IsIconic(hwnd) == true)
						::ShowWindow(hwnd, SW_RESTORE);
					/*if (windowState.showCmd != SW_MAXIMIZE)
						::ShowWindow(hwnd, SW_RESTORE);*/


				/*	if (::GetForegroundWindow() != hwnd){
						HWND h_active_wnd = ::GetForegroundWindow();
						if (h_active_wnd != NULL){
							DWORD thread_id = GetWindowThreadProcessId(h_active_wnd, NULL);
							DWORD current_thread_id = GetCurrentThreadId();
							if (current_thread_id != thread_id){
								if (AttachThreadInput(current_thread_id, thread_id, TRUE)){
									BringWindowToTop();
									AttachThreadInput(current_thread_id, thread_id, FALSE);
								}
							}
						}
					}*/

					::BringWindowToTop(hwnd);
				}
				terminateSwitcher();
				break;
			}
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}


void E_WindowSwitcher::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	return __super::OnEraseBkgnd(pDC);
}


void E_WindowSwitcher::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (nChar == VK_TAB){
		TRACE_WIN32A("VK_TAB");
		
	}

	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}


void E_WindowSwitcher::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}


void E_WindowSwitcher::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus(pNewWnd);

	//lock_guard<std::mutex> lock(E_Mutex::windowSwitcherEvent);
	/*if (running==true)
		terminateSwitcher();*/
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


void E_WindowSwitcher::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	__super::OnSysKeyDown(nChar, nRepCnt, nFlags);
}


void E_WindowSwitcher::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	
	__super::OnSysKeyUp(nChar, nRepCnt, nFlags);
}


// ���� ������� �̵�
void E_WindowSwitcher::selectNextWindow()
{
	E_Global* global = E_Global::getSingleton();

	//�ι�° ����ũž ������ ����ó ��� //
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

	int selectedSize = global->getSelectedDesktop()->getWindowList().size() + 1;//����ũž ����
	int otherSize = secondWinlist.size();

	tabIndex++;
	if (tabIndex >= selectedSize){
		tabIndex = 0;
	}
	Invalidate(0);
}


// ���� ������� �̵�
void E_WindowSwitcher::selectPrevWindow()
{
	E_Global* global = E_Global::getSingleton();

	//�ι�° ����ũž ������ ����ó ��� //
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

	tabIndex--;

	if (tabIndex < 0){
		tabIndex = selectedSize - 1;
	}
	Invalidate(0);
}


// �ٸ� ����ũž���� ���
void E_WindowSwitcher::selectOtherDesktop()
{
	
}


void E_WindowSwitcher::startTPMode() 
{
	E_Window::SetMinimizeMaximizeAnimation(false);
	E_Global* global = E_Global::getSingleton();  
	E_Desktop* selectedDesktop = global->getSelectedDesktop();

	for (list<E_Desktop*>::iterator iterDesktop = global->desktopList.begin(); iterDesktop != global->desktopList.end(); iterDesktop++){
		if (selectedDesktop == *iterDesktop){
			(*iterDesktop)->setAllTransParentExclude();	//tp ���� ����
			continue;
		}
		(*iterDesktop)->setAllIconInvisible();

		//â ��� ������
		(*iterDesktop)->setAllShow();	//â ��ġ ����
		(*iterDesktop)->setAllSaveShowState();	//â ��ġ ����
		(*iterDesktop)->setAllHide();			//â ��ġ �����

		(*iterDesktop)->setAllTransParentExclude();
	}
	E_Window::SetMinimizeMaximizeAnimation(true);
}


void E_WindowSwitcher::stopTPMode()
{
	E_Window::SetMinimizeMaximizeAnimation(false);
	E_Global* global = E_Global::getSingleton();

	//�ּ�ȭ
	E_Desktop* selectedDesktop = global->getSelectedDesktop();
	std::list<E_Window*> winlist = selectedDesktop->getWindowList();
	selectedDesktop->setAllMinimizeTransparent();	//tp��� ����
	
	for (list<E_Desktop*>::iterator iterDesktop = global->desktopList.begin(); iterDesktop != global->desktopList.end(); iterDesktop++){
		if (*iterDesktop == selectedDesktop)
			continue;
		//â �����
		(*iterDesktop)->setAllOpaque();

		//â ��� ������
		(*iterDesktop)->setAllRestoreSavedShowState(); //���� â ��ġ ���� �� 

		(*iterDesktop)->setAllHide();					//����
		(*iterDesktop)->setAllIconVisible();			//������ ���� ��
	}
	E_Window::SetMinimizeMaximizeAnimation(true);
}

//void E_WindowSwitcher::saveShowState()
//{
//}


CWnd* E_WindowSwitcher::createChild()
{
	int iconsize = getIconSize(E_EnvironmentManager::getSingleton()->getWidth());
	RECT winRect;
	winRect.left = 0;
	winRect.right = 0;
	winRect.bottom = iconsize;
	winRect.right = iconsize;

	CBrush brush_map;
	brush_map.CreateStockObject(NULL_BRUSH);

	CWnd* cwnd = new CWnd;
	UINT nClassStyle_window = 0;
	CString szClassName_window = AfxRegisterWndClass(nClassStyle_window, 0, brush_map, 0);
	cwnd->CreateEx(WS_EX_TOPMOST, szClassName_window, L"icon", WS_VISIBLE | WS_POPUP, winRect, this, 0, NULL);
	//CREATE
	brush_map.DeleteObject();

	return cwnd;
}
