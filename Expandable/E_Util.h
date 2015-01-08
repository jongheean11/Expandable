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
	static int E_Util::getDefaultIconSize();
	static MYICON_INFO E_Util::getIconInfo(HICON hIcon);
	static HICON E_Util::getIconHandle(HWND hwnd);
	static HBITMAP ConvertIconToBitmap(HICON hIcon, int nWidth, int nHeight, COLORREF clrBackground = RGB(255, 255, 255));
	E_Util();
	~E_Util();
};
