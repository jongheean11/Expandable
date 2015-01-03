#include "stdafx.h"
#include "E_Global.h"

E_Global* E_Global::singleton = NULL;

E_Global::E_Global()
{

}
E_Global::~E_Global()
{

}
/*
void E_Global::setresolutionWidth(int paramWidth)
{
	resolutionWidth = paramWidth;
}

double E_Global::getresolutionWidth()
{
	return resolutionWidth;
}

void E_Global::setresolutionHeight(int paramHeight)
{
	resolutionHeight = paramHeight;
}

double E_Global::getresolutionHeight()
{
	return resolutionHeight;
}

void E_Global:: setVirtualWidth(int paramWidth)
{
	virtualWidth = paramWidth;
}

double E_Global::getVirtualWidth()
{
	return virtualWidth;
}

void E_Global::setVirtualHeight(int paramHeight)
{
	virtualHeight = paramHeight;
}

double E_Global::getVirtualHeight()
{
	return virtualHeight;
}
*/

//듀얼 모니터 관련 콜백
void E_Global::OnDualMonitorMode(bool dualMonitorMode){
	
}

void E_Global::setSelectedDesktopIndex(int paramIndex)
{
	selectedDesktopIndex = paramIndex;
}

int E_Global::getSelectedDesktopIndex()
{
	return selectedDesktopIndex;
}

void E_Global::startUpdateSelectedDesktop()
{

}

void E_Global::stopUpdateSelectedDesktop()
{

}

E_Global* E_Global::getSingleton()
{
	if (singleton != NULL)
		singleton = new E_Global();
	return singleton;
}
