#include "stdafx.h"
#include "E_Hotkey.h"

E_Hotkey* E_Hotkey::singleton = NULL;
const wchar_t* E_Hotkey::caption = L"E_Hotkey";


const int E_Hotkey::IDWINDOWLEFT = 100;
const int E_Hotkey::IDWINDOWRIGHT = 101;
const int E_Hotkey::IDWINDOWUP = 102;
const int E_Hotkey::IDWINDOWDOWN = 103;

const  int E_Hotkey::IDDESKTOPLEFT = 200;
const  int E_Hotkey::IDDESKTOPRIGHT = 201;
const  int E_Hotkey::IDDESKTOPUP = 202;
const  int E_Hotkey::IDDESKTOPDOWN = 203;

int E_Hotkey::MODIFIERWINDOWLEFT = MOD_ALT | MOD_WIN;
int E_Hotkey::MODIFIERWINDOWRIGHT = MOD_ALT | MOD_WIN;
int E_Hotkey::MODIFIERWINDOWUP = MOD_ALT | MOD_WIN;
int E_Hotkey::MODIFIERWINDOWDOWN = MOD_ALT | MOD_WIN;

int E_Hotkey::MODIFIERDESKTOPLEFT = MOD_CONTROL | MOD_WIN;
int E_Hotkey::MODIFIERDESKTOPRIGHT = MOD_CONTROL | MOD_WIN;
int E_Hotkey::MODIFIERDESKTOPUP = MOD_CONTROL | MOD_WIN;
int E_Hotkey::MODIFIERDESKTOPDOWN = MOD_CONTROL | MOD_WIN;

char E_Hotkey::CHARWINDOWLEFT = VK_LEFT;
char E_Hotkey::CHARWINDOWRIGHT = VK_RIGHT;
char E_Hotkey::CHARWINDOWUP = VK_UP;
char E_Hotkey::CHARWINDOWDOWN = VK_DOWN;

char E_Hotkey::CHARDESKTOPLEFT = VK_LEFT;
char E_Hotkey::CHARDESKTOPRIGHT = VK_RIGHT;
char E_Hotkey::CHARDESKTOPUP = VK_UP;
char E_Hotkey::CHARDESKTOPDOWN = VK_DOWN;

E_Hotkey::E_Hotkey()
{
	CBrush brush_window;
	UINT nClassStyle_window = 0;// CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	CString szClassName_window = AfxRegisterWndClass(nClassStyle_window, 0, (HBRUSH)CreateSolidBrush(RGB(0xff, 0xff, 0xff)), 0);
	this->CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_window, E_Hotkey::caption, WS_VISIBLE | WS_POPUP, CRect(0, 0, 0, 0), CWnd::GetDesktopWindow(), 0);
	this->ShowWindow(SW_HIDE);
	this->UpdateWindow();
}


E_Hotkey::~E_Hotkey()
{
}


// 글로벌 윈도우를 매개변수로 받은 후 핫키 등록
void E_Hotkey::init(E_IGlobal* iglobal)
{
	this->iglobal = iglobal;
	if (!RegisterHotKey(this->m_hWnd, IDWINDOWLEFT, MODIFIERWINDOWLEFT, CHARWINDOWLEFT)){
		TRACE_WIN32A("[E_Hotkey::init]HOTKEY %d INIT FAIL", IDWINDOWLEFT);
	}
	if (!RegisterHotKey(this->m_hWnd, IDWINDOWRIGHT, MODIFIERWINDOWRIGHT, CHARWINDOWRIGHT)){
		TRACE_WIN32A("[E_Hotkey::init]HOTKEY %d INIT FAIL", IDWINDOWRIGHT);
	}
	if (!RegisterHotKey(this->m_hWnd, IDWINDOWUP, MODIFIERWINDOWUP, CHARWINDOWUP)){
		TRACE_WIN32A("[E_Hotkey::init]HOTKEY %d INIT FAIL", IDWINDOWUP);
	}
	if (!RegisterHotKey(this->m_hWnd, IDWINDOWDOWN, MODIFIERWINDOWDOWN, CHARWINDOWDOWN)){
		TRACE_WIN32A("[E_Hotkey::init]HOTKEY %d INIT FAIL", IDWINDOWDOWN);
	}
	if (!RegisterHotKey(this->m_hWnd, IDDESKTOPLEFT, MODIFIERDESKTOPLEFT, CHARDESKTOPLEFT)){
		TRACE_WIN32A("[E_Hotkey::init]HOTKEY %d INIT FAIL", IDDESKTOPLEFT);
	}
	if (!RegisterHotKey(this->m_hWnd, IDDESKTOPRIGHT, MODIFIERDESKTOPRIGHT, CHARDESKTOPRIGHT)){
		TRACE_WIN32A("[E_Hotkey::init]HOTKEY %d INIT FAIL", IDDESKTOPRIGHT);
	}
	if (!RegisterHotKey(this->m_hWnd, IDDESKTOPUP, MODIFIERDESKTOPUP, CHARDESKTOPUP)){
		TRACE_WIN32A("[E_Hotkey::init]HOTKEY %d INIT FAIL", IDDESKTOPUP);
	}
	if (!RegisterHotKey(this->m_hWnd, IDDESKTOPDOWN, MODIFIERDESKTOPDOWN, CHARDESKTOPDOWN)){
		TRACE_WIN32A("[E_Hotkey::init]HOTKEY %d INIT FAIL", IDDESKTOPDOWN);
	}
}


E_Hotkey* E_Hotkey::getSingleton()
{
	if (singleton == NULL)
		singleton = new E_Hotkey();

	return singleton;
}
BEGIN_MESSAGE_MAP(E_Hotkey, CWnd)
	ON_WM_HOTKEY()
END_MESSAGE_MAP()

void E_Hotkey::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nHotKeyId){
	case E_Hotkey::IDWINDOWLEFT:
	{
								   iglobal->moveTopWindowLeft();
								   TRACE_WIN32A("[E_Hotkey::OnHotKey]MOVE WINDOW TO LEFT");
								   break;
	}
	case E_Hotkey::IDWINDOWRIGHT:
	{
									iglobal->moveTopWindowRight();
									//TRACE_WIN32A("[E_Hotkey::OnHotKey]MOVE WINDOW TO RIGHT");
									break;
	}
	case E_Hotkey::IDWINDOWUP:
	{
								 iglobal->moveTopWindowUp();

								 //TRACE_WIN32A("[E_Hotkey::OnHotKey]MOVE WINDOW TO UP");
								 break;
	}
	case E_Hotkey::IDWINDOWDOWN:
	{
								   iglobal->moveTopWindowDown();
								   //TRACE_WIN32A("[E_Hotkey::OnHotKey]MOVE WINDOW TO DOWN");
								   break;
	}
	case E_Hotkey::IDDESKTOPLEFT:{
									 iglobal->moveDesktopLeft();
									 //TRACE_WIN32A("[E_Hotkey::OnHotKey]MOVE WINDOW TO DOWN");
									 break;
	}
	case E_Hotkey::IDDESKTOPRIGHT:{
									  iglobal->moveDesktopRight();
									  //TRACE_WIN32A("[E_Hotkey::OnHotKey]MOVE WINDOW TO DOWN");
									  break;
	}
	case E_Hotkey::IDDESKTOPUP:{
								   iglobal->moveDesktopUp();
								   //TRACE_WIN32A("[E_Hotkey::OnHotKey]MOVE WINDOW TO DOWN");
								   break;
	}
	case E_Hotkey::IDDESKTOPDOWN:{
									 iglobal->moveDesktopDown();
									 //TRACE_WIN32A("[E_Hotkey::OnHotKey]MOVE WINDOW TO DOWN");
									 break;
	}
	}
	CWnd::OnHotKey(nHotKeyId, nKey1, nKey2);
}
