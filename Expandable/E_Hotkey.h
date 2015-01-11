#pragma once
#include "afxwin.h"
#include "E_IGlobal.h"
class E_Hotkey :
	public CWnd
{
private:
	static E_Hotkey* singleton;
	E_IGlobal* iglobal;
public:
	E_Hotkey();
	~E_Hotkey();
	// 글로벌 윈도우를 매개변수로 받은 후 핫키 등록
	void init(E_IGlobal*);
	const static wchar_t* caption;

	const static int IDWINDOWLEFT;
	const static int IDWINDOWRIGHT;
	const static int IDWINDOWUP;
	const static int IDWINDOWDOWN;

	const static int IDDESKTOPLEFT;
	const static int IDDESKTOPRIGHT;
	const static int IDDESKTOPUP;
	const static int IDDESKTOPDOWN;

	static int MODIFIERWINDOWLEFT;
	static int MODIFIERWINDOWRIGHT;
	static int MODIFIERWINDOWUP;
	static int MODIFIERWINDOWDOWN;

	static int MODIFIERDESKTOPLEFT;
	static int MODIFIERDESKTOPRIGHT;
	static int MODIFIERDESKTOPUP;
	static int MODIFIERDESKTOPDOWN;

	static char CHARWINDOWLEFT;
	static char CHARWINDOWRIGHT;
	static char CHARWINDOWUP;
	static char CHARWINDOWDOWN;

	static char CHARDESKTOPLEFT;
	static char CHARDESKTOPRIGHT;
	static char CHARDESKTOPUP;
	static char CHARDESKTOPDOWN;
	static E_Hotkey* getSingleton();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
};

