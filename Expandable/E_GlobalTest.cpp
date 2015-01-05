#include "stdafx.h"
#include "E_GlobalTest.h"


E_GlobalTest::E_GlobalTest()
{
}


E_GlobalTest::~E_GlobalTest()
{
}

void E_GlobalTest::testGetKakaoWindow()
{
	if (NULL != E_Global::getSingleton()->getKakaoWindow()) {
		TRACE_WIN32A("[%s] OK","testGetKakaoWindow");
	} else {
		TRACE_WIN32A("[%s] FAIL", "testGetKakaoWindow");

	}
}