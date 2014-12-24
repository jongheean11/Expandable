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
	void setIconInvisible();
	void setIconVisible();
	void setTransparent();
	void setOpaque();
	HWND getHWND();
};