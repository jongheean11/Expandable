#include "stdafx.h"
#include "E_Mutex.h"

mutex E_Mutex::updateMutex;

//mutex E_Mutex::updateGetAllWindows;

mutex E_Mutex::updateStartStopMutex;	//startUpdate�� stopUpdate�� ���Ǵ� ���ؽ�
mutex E_Mutex::windowSwitcherEvent;	//Window Switcher���� SHOW�� FOCUS�� �����ϴ� ���ؽ�
mutex E_Mutex::hotkeyMutex;	//Window Switcher���� SHOW�� FOCUS�� �����ϴ� ���ؽ�

E_Mutex::E_Mutex()
{
}


E_Mutex::~E_Mutex()
{
}
