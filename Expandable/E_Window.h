using namespace Gdiplus;
using namespace std;

#pragma once
class E_Window
{
private:
	double start_x, start_y;
	double width, height;
	int zIndex;
	int windowState;
	int dockState;
	int alertState;
	HWND hwnd;
	Bitmap *icon;
	Bitmap *screenshot;

public:
	E_Window();
	E_Window(const E_Window &tmp);
	~E_Window();
	void operator=(const E_Window &tmp); 
	bool operator==(const E_Window &tmp);
	void setHWND(string mainWindowStr, string exWindowStr);
	static void setIconInvisible(HWND hwnd);
	static void setIconVisible(HWND hwnd);
	void setTransparent();
	void setOpaque();
	HWND getHWND();
	double getStartX();
	void setStartX(double param_x);
	double getStartY();
	void setStartY(double param_y);
	double getWidth();
	void setWidth(double param_w);
	double getHeight();
	void setHeight(double param_h);
};