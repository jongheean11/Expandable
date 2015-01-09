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

// UI�� �����ְ� �Է��� �޴� â�� Ȱ��ȭ ��Ŵ
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


// UI�� ������ â�� ������ �Լ�
void E_WindowSwitcher::terminateSwitcher()
{
	//ũ��Ƽ�� ����?
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

/*â�� ���� �׸��� �Լ�*/
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
				long tempAeroHeightCount = 1;	//�ӽ� ���̴� 1��

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
				static long switcherHeight = aeroHeight * maxHeightCount + paddingSize * 2; //����ġ �̸� ���� ���߿� �߰� �ʿ�
				static long switcherLeft = resWidth / 2 - switcherWidth / 2;
				static long switcherTop = resHeight / 2 - switcherHeight / 2;

				TRACE_WIN32A("[E_WindowSwitcher::OnPaint]����ũž ��� switcherWidth: %d switcherHeight: %d switcherLeft: %d, switcherTop: %d", switcherWidth, switcherHeight, switcherLeft, switcherTop);

				
				int count = 0;
				int widthCount = 0;//0~6 ���� �ݺ�
				int heightCount = 0;
				WINDOWPLACEMENT windowState;

				for (list<E_Window*>::reverse_iterator iter = winlist.rbegin(); iter != winlist.rend(); iter++){
					CWnd* cwnd = CWnd::FromHandle((*iter)->getWindow());
					cwnd->GetWindowPlacement(&windowState);
					
					//
					CString windowName;
					cwnd->GetWindowTextW(windowName);
					
					TRACE_WIN32(L"[E_WindowSwitcher::OnPaint] %s\t\t[state]: %d", windowName.GetBuffer(), windowState.showCmd);
					
					long aeroLeftoffset = paddingSize + (aeroWidth * widthCount); //������ ���� ��ġ�� �޶���!!!
					long aeroTopoffset = paddingSize + (aeroHeight * heightCount); //����ġ �̸� ���� ���߿� �߰� �ʿ� //������ ���� ��ġ�� �޶���!!!

					CBrush brush1;   // Must initialize!
					brush1.CreateSolidBrush(E_WindowSwitcher::aeroColor);   // Blue brush.
					//Aero ���� �簢��
					CRect temprect;
					temprect.top = aeroTopoffset;
					temprect.left = aeroLeftoffset;
					temprect.bottom = temprect.top + aeroHeight;
					temprect.right = temprect.left + aeroWidth;
					//dc.Rectangle(temprect);
					dc.FillRect(&temprect, &brush1);

					//��輱
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

					//aero ���� ������
					static long previewLeftoffset = paddingSize;	//���� aero ũ��
					static long previewTopoffset = paddingSize;	//���� aero ũ��

					TRACE_WIN32A("[E_WindowSwitcher::OnPaint] aeroLeftoffset: %d aeroTopoffset: %d previewLeftoffset: %d previewTopoffset: %d", aeroLeftoffset, aeroTopoffset, previewLeftoffset, previewTopoffset);

					CRect crect = { 0, 0, 0,0 };

					//�ϳ��� ������ �ڽ� ���� ��ҵ� ������ ũ��
					long windowWidth = 0;
					long windowHeight = 0;
					long windowTopOffset = 0;
					long windowLeftOffset = 0;

					CBitmap* screenshot;
					BITMAP bmpinfo;             //��Ʈ���� ���̿� ũ�Ⱑ �޶�
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
					
					//����� ��

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
					
					//������
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

					//��ġ ����
					widthCount = (count + 1) % 7; 

					count++;

					if (count >= 7)
						heightCount = 1; //0 ~ 1 ����

					//�׽�Ʈ �ڵ�
					if (count >= 14)
						break;
				}

				////ù��° â
				//{
				//	CWnd* cwnd = CWnd::FromHandle((*winlist.rbegin())->getWindow());
				//	static long aeroLeftoffset = paddingSize; //������ ���� ��ġ�� �޶���!!!
				//	static long aeroTopoffset = paddingSize; //����ġ �̸� ���� ���߿� �߰� �ʿ� //������ ���� ��ġ�� �޶���!!!

				//	//��輱
				//	CRect temprect;
				//	temprect.top = aeroTopoffset;
				//	temprect.left = aeroLeftoffset;
				//	temprect.bottom = temprect.top + aeroHeight;
				//	temprect.right = temprect.left + aeroWidth;
				//	dc.Rectangle(temprect);
				//	
				//	//aero ���� ������
				//	static long previewLeftoffset = paddingSize;	//���� aero ũ��
				//	static long previewTopoffset = paddingSize;	//���� aero ũ��

				//	TRACE_WIN32A("[E_WindowSwitcher::OnPaint] â aeroLeftoffset: %d aeroTopoffset: %d previewLeftoffset: %d previewTopoffset: %d", aeroLeftoffset, aeroTopoffset, previewLeftoffset, previewTopoffset);

				//	CRect crect;

				//	//�ϳ��� ������ �ڽ� ���� ��ҵ� ������ ũ��
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
				//		ratio = (double)(crect.right - crect.left) / (double)(crect.bottom - crect.top); //����
				//		//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] ����");

				//		//�������� �涧
				//		windowWidth = previewWidth;
				//		windowHeight = (int)(windowWidth / ratio);
				//		windowTopOffset = (previewHeight - windowHeight) / 2;
				//		windowLeftOffset = 0;
				//		break;
				//		
				//	case VERTICAL:
				//		ratio = (double)(crect.bottom - crect.top) / (double)(crect.right - crect.left); //����
				//		//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] ����");

				//		//�������� �涧
				//		windowHeight = previewHeight;
				//		windowWidth = (int)(windowHeight / ratio);
				//		windowTopOffset = 0;
				//		windowLeftOffset = (previewWidth - windowWidth) / 2;
				//		break;

				//	}

				//	TRACE_WIN32A("[E_WindowSwitcher::OnPaint] â ratio: %lf windowWidth: %d windowHeight: %d windowTopOffset: %d windowLeftOffset: %d",ratio, windowWidth, windowHeight, windowTopOffset, windowLeftOffset);
				//	//��ġ �̵�
				//	this->SetWindowPos(NULL
				//		, switcherLeft
				//		, switcherTop
				//		, switcherWidth, switcherHeight
				//		, SWP_NOZORDER | SWP_SHOWWINDOW);

				//	//�׽�Ʈ 
				//	RECT rect;
				//	rect.top = aeroTopoffset + previewTopoffset + windowTopOffset;
				//	rect.left = aeroLeftoffset + previewLeftoffset + windowLeftOffset;
				//	rect.right = rect.left + windowWidth;
				//	rect.bottom = rect.top + windowHeight;

				//	E_AeroPeekController::getSingleton()->moveAero((thumb_map.find(cwnd->GetSafeHwnd()))->second, rect);
				//}
				////�ι�° â
				//{
				//	static long aeroLeftoffset = paddingSize; //������ ���� ��ġ�� �޶���!!!
				//	static long aeroTopoffset = paddingSize + aeroHeight ; //����ġ �̸� ���� ���߿� �߰� �ʿ� //������ ���� ��ġ�� �޶���!!!
				//	
				//	//��輱
				//	CRect temprect;
				//	temprect.top = aeroTopoffset;
				//	temprect.left = aeroLeftoffset;
				//	temprect.bottom = temprect.top + aeroHeight;
				//	temprect.right = temprect.left + aeroWidth;
				//	dc.Rectangle(temprect);

				//	//aero ���� ������
				//	static long previewLeftoffset = paddingSize;	//���� aero ũ��
				//	static long previewTopoffset = paddingSize;	//���� aero ũ��

				//	TRACE_WIN32A("[E_WindowSwitcher::OnPaint] â aeroLeftoffset: %d aeroTopoffset: %d previewLeftoffset: %d previewTopoffset: %d", aeroLeftoffset, aeroTopoffset, previewLeftoffset, previewTopoffset);

				//	CRect crect;

				//	//�ϳ��� ������ �ڽ� ���� ��ҵ� ������ ũ��
				//	long windowWidth = 0;
				//	long windowHeight = 0;
				//	long windowTopOffset = 0;
				//	long windowLeftOffset = 0;

				//	E_Global::getSingleton()->getKakaoWindow()->GetWindowRect(crect);

				//	TRACE_WIN32A("[E_WindowSwitcher::OnPaint] CRECT top %d left %d bottom %d right %d", crect.top, crect.left, crect.bottom, crect.right);

				//	double ratio = 0;
				//	switch (getShape(crect.right - crect.left, crect.bottom - crect.top, resWidth, resHeight)) {
				//	case HORIZONTAL:
				//		ratio = (double)(crect.right - crect.left) / (double)(crect.bottom - crect.top); //����
				//		//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] ����");

				//		//�������� �涧
				//		windowWidth = previewWidth;
				//		windowHeight = (int)(windowWidth / ratio);
				//		windowTopOffset = (previewHeight - windowHeight) / 2;
				//		windowLeftOffset = 0;
				//		break;

				//	case VERTICAL:
				//		ratio = (double)(crect.bottom - crect.top) / (double)(crect.right - crect.left); //����
				//		//TRACE_WIN32A("[E_WindowSwitcher::OnPaint] ����");

				//		//�������� �涧
				//		windowHeight = previewHeight;
				//		windowWidth = (int)(windowHeight / ratio);
				//		windowTopOffset = 0;
				//		windowLeftOffset = (previewWidth - windowWidth) / 2;
				//		break;

				//	}

				//	TRACE_WIN32A("[E_WindowSwitcher::OnPaint] â ratio: %lf windowWidth: %d windowHeight: %d windowTopOffset: %d windowLeftOffset: %d", ratio, windowWidth, windowHeight, windowTopOffset, windowLeftOffset);
				//	//��ġ �̵�
				//	this->SetWindowPos(NULL
				//		, switcherLeft
				//		, switcherTop
				//		, switcherWidth, switcherHeight
				//		, SWP_NOZORDER | SWP_SHOWWINDOW);

				//	//�׽�Ʈ 
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
	//�׸��� ������ ���� ũ�⸦ �����Ͽ� ��� ������ ��
	OutputDebugStringA("OnPaint()");
	// �׸��� �޽����� ���ؼ��� CWnd::OnPaint()��(��) ȣ������ ���ʽÿ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	for (unordered_map<HWND, RECT>::iterator itr = rect_map.begin(); itr != rect_map.end(); itr++){
		RECT rect = itr->second;
		if (rect.left < point.x && rect.right > point.x && rect.top < point.y && rect.bottom > point.y) {
			
		}
	}
	CWnd::OnMouseMove(nFlags, point);
}
