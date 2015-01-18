#include "stdafx.h"
#include "E_MyCSocket.h"


E_MyCSocket::E_MyCSocket()
{
}


E_MyCSocket::~E_MyCSocket()
{
}


void E_MyCSocket::OnClose(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	closeFlag = true;
	CSocket::OnClose(nErrorCode);
}
