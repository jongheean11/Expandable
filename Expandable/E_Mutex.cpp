#include "stdafx.h"
#include "E_Mutex.h"

mutex E_Mutex::updateMutex;

mutex E_Mutex::updateStartStopMutex;	//startUpdate와 stopUpdate에 사용되는 뮤텍스

E_Mutex::E_Mutex()
{
}


E_Mutex::~E_Mutex()
{
}
