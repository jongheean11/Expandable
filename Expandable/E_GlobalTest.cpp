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
 list<HWND> E_GlobalTest::testGetAllWindows()
{
	 E_Global::getSingleton()->getKakaoWindow()->ShowWindow(SW_HIDE);

	E_Global* object = E_Global::getSingleton();
	
	//EnumWindows(E_Global::EnumCallBack, 0);

	E_Global::getSingleton()->getKakaoWindow()->ShowWindow(SW_SHOW);
	list<HWND> result = object->getAllWindows();

	return result;
}