#pragma once
class E_ISwitcherUpdator
{
public:
	E_ISwitcherUpdator();
	~E_ISwitcherUpdator();
	virtual void updateSelectedDesktop() = 0;
};

