#pragma once
class E_MyCSocket : public CSocket
{
public:
	E_MyCSocket();
	~E_MyCSocket();
	bool closeFlag = false;
	virtual void OnClose(int nErrorCode);
};

