#include "E_Desktop.h"

class E_Global
{
private:
	int selectedDesktopIndex;
	int aeroMode;
	int updateState;
	list<E_Desktop> desktopList;
	list<E_Window> dockedWindowList;
public:
	E_Global();
	~E_Global();
	static double resolutionWidth, resolutionHeight;
	static double virtualWidth, virtualHeight;
	/*static void setresolutionWidth(int paramWidth);
	static double getresolutionWidth();
	static void setresolutionHeight(int paramHeight);
	static double getresolutionHeight();
	static void setVirtualWidth(int paramWidth);
	static double getVirtualWidth();
	static void setVirtualHeight(int paramHeight);
	static double getVirtualHeight();*/
	void setSelectedDesktopIndex(int paramIndex);
	int getSelectedDesktopIndex();
	void startUpdateSelectedDesktop();
	void stopUpdateSelectedDesktop();
};