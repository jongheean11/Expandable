#include "stdafx.h"
#include "E_Window.h"
#include "E_Map.h"

void E_Map::drawMap()
{

}

void E_Map::onMouseLButtonDown()
{
 
}

void E_Map::onMouseMove()
{

}

void E_Map::OnMouseLButtonUp()
{

}

void E_Map::startTimer()
{
	sTime = clock();
	SetTimer(NULL,0, 500, NULL);
	
}

void E_Map::stopTimer()
{
	nTime = clock();
	if (1)//마우스가 미니맵내부
	{
		sTime = clock();
	}
	else //외부
	{
		nTime = clock();
		if (nTime - sTime > visibleTime)
			KillTimer(NULL, 0);
	}

}
