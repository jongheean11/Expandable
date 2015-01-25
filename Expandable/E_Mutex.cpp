#include "stdafx.h"
#include "E_Mutex.h"

mutex E_Mutex::updateMutex;

//mutex E_Mutex::updateGetAllWindows;

mutex E_Mutex::updateStartStopMutex;	//startUpdate와 stopUpdate에 사용되는 뮤텍스
mutex E_Mutex::windowSwitcherEvent;	//Window Switcher에서 SHOW와 FOCUS를 중재하는 뮤텍스
mutex E_Mutex::hotkeyMutex;	//Window Switcher에서 SHOW와 FOCUS를 중재하는 뮤텍스

E_Mutex::E_Mutex()
{
}


E_Mutex::~E_Mutex()
{
}
