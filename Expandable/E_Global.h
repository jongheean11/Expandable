#include "E_Desktop.h"

class E_Global
{
private:
	double resolutionWidth, resolutionHeight;
	double virtualWidth, virtualHeight;
	int selectedDesktopIndex;
	int aeroMode;
	int updateState;
	list<E_Desktop> desktopList;
	list<E_Window> dockedWindowList;
public:
	E_Global();
	~E_Global();
	void setresolutionWidth(int paramWidth);
	double getresolutionWidth();
	void setresolutionHeight(int paramHeight);
	double getresolutionHeight();
	void setVirtualWidth(int paramWidth);
	double getVirtualWidth();
	void setVirtualHeight(int paramHeight);
	double getVirtualHeight();
	void setSelectedDesktopIndex(int paramIndex);
	int getSelectedDesktopIndex();
	void startUpdateSelectedDesktop();
	void stopUpdateSelectedDesktop();
};