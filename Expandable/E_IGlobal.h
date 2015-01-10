#pragma once
class E_IGlobal
{
public:
	E_IGlobal();
	~E_IGlobal();
	virtual void moveTopWindowLeft() = 0;
	// 탑 윈도우를 오른쪽 데스크탑으로 전달
	virtual void moveTopWindowRight() = 0;
	// 탑 윈도우를 아래로 데스크탑으로 전달
	virtual void moveTopWindowDown() = 0;
	// 탑 윈도우를 위쪽 데스크탑으로 전달
	virtual void moveTopWindowUp() = 0;
	// 왼쪽 데스크 탑으로 이동
	virtual void moveDesktopLeft() = 0;
	// 오른쪽 데스크탑으로 이동
	virtual void moveDesktopRight() = 0;
	// 위쪽 데스크탑으로 이동
	virtual void moveDesktopUp() = 0;
	// 아래쪽 데스크탑으로 이동
	virtual void moveDesktopDown() = 0;
};

