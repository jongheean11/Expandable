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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	closeFlag = true;
	CSocket::OnClose(nErrorCode);
}
