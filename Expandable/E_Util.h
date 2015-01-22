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

	static DWORD GetProcessID(LPCTSTR pszProcessName);	//���μ��� ���̵� �������� �ڵ�
	static bool isContainDLL(wchar_t* processName, wchar_t* dllname);	//DLL ���� ����
	E_Util();
	~E_Util();
};
