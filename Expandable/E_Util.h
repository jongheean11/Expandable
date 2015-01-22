#pragma once
struct MYICON_INFO
{
	int     nWidth;
	int     nHeight;
	int     nBitsPerPixel;
};

class E_Util
{
public:
	static int getSystemSmallIconSize();
	static int getSystemLargeIconSize();
	static MYICON_INFO E_Util::getIconInfo(HICON hIcon);
	static HICON E_Util::getIconHandle(HWND hwnd);
	static HBITMAP ConvertIconToBitmap(HICON hIcon, int nWidth, int nHeight, COLORREF clrBackground = RGB(255, 255, 255));

	static DWORD GetProcessID(LPCTSTR pszProcessName);	//프로세스 아이디 가져오는 코드
	static bool isContainDLL(wchar_t* processName, wchar_t* dllname);	//DLL 포함 여부
	E_Util();
	~E_Util();
};
