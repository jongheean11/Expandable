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
