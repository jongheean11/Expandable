#include "stdafx.h"
#include "E_WindowSwitcherTest.h"


E_WindowSwitcherTest::E_WindowSwitcherTest()
{
}


E_WindowSwitcherTest::~E_WindowSwitcherTest()
{
}


void E_WindowSwitcherTest::testGetIconSize()
{
	if (E_WindowSwitcher::getSingleton()->getIconSize(1280) == 48)
		TRACE_WIN32A("%sOK\n","[testGetIconSize]");
	else
		TRACE_WIN32A("%sFAIL\n", "[testGetIconSize]");
}


void E_WindowSwitcherTest::testGetAeroSize()
{
	if (E_WindowSwitcher::getSingleton()->getIconSize(1280) == 48)
		TRACE_WIN32A("%sOK\n", "[testGetAeroSize]");
	else
		TRACE_WIN32A("%sFAIL\n", "[testGetAeroSize]");
}


void E_WindowSwitcherTest::testGetPaddingSize()
{
	if (E_WindowSwitcher::getSingleton()->getPaddingSize(1920) == 10)
		TRACE_WIN32A("%sOK\n", "[testGetPaddingSize]");
	else
		TRACE_WIN32A("%sFAIL\n", "[testGetPaddingSize]");
}

void E_WindowSwitcherTest::testGetShape()
{

	int res_width = 100;
	int res_height = 80;

	int width = 100;
	int height = 50;
	if (E_WindowSwitcher::getSingleton()->getShape(width, height, res_width, res_height) == HORIZONTAL)
		TRACE_WIN32A("%sOK\n", "[testGetShape]");
	else
		TRACE_WIN32A("%sFAIL\n", "[testGetShape]");

	res_width = 100;
	res_height = 80;

	width = 50;
	height = 100;

	
	if (E_WindowSwitcher::getSingleton()->getShape(width, height, res_width, res_height) == VERTICAL)
		TRACE_WIN32A("%sOK\n", "[testGetShape]");
	else
		TRACE_WIN32A("%sFAIL\n", "[testGetShape]");
	
}