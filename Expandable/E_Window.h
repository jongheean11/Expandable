using namespace Gdiplus;
using namespace std;

#pragma once
class E_Window
{
private:
	HWND window;
//	HWND hwnd;
	CBitmap icon;
	CBitmap screenshot;
	//E_Desktop* desktop;

	double start_x, start_y;
	double width, height;
	int zIndex;
	int windowState;
	int dockState;
	int alertState;

public:

	bool taskScreenshot();
	E_Window(HWND);
	E_Window();
	E_Window(const E_Window &tmp);
	~E_Window();

	static bool setIconInvisible(HWND hwnd);
	static bool setIconVisible(HWND hwnd);
	void setTransparent();
	void setOpaque();
	void setHide();
	// 윈도우 세팅
	void setWindow(HWND window);
	HWND getWindow();
	void setIcon();
	CBitmap getIcon();

	void operator=(const E_Window &tmp);
	bool operator==(const E_Window &tmp);
//	void setHWND(string mainWindowStr, string exWindowStr);

//	HWND getHWND();
	double getStartX();
	void setStartX(double param_x);
	double getStartY();
	void setStartY(double param_y);
	double getWidth();
	void setWidth(double param_w);
	double getHeight();
	void setHeight(double param_h);
	void setShow();
};