#pragma once
class E_IGlobal
{
public:
	E_IGlobal();
	~E_IGlobal();
	virtual void moveTopWindowLeft() = 0;
	// ž �����츦 ������ ����ũž���� ����
	virtual void moveTopWindowRight() = 0;
	// ž �����츦 �Ʒ��� ����ũž���� ����
	virtual void moveTopWindowDown() = 0;
	// ž �����츦 ���� ����ũž���� ����
	virtual void moveTopWindowUp() = 0;
	// ���� ����ũ ž���� �̵�
	virtual void moveDesktopLeft() = 0;
	// ������ ����ũž���� �̵�
	virtual void moveDesktopRight() = 0;
	// ���� ����ũž���� �̵�
	virtual void moveDesktopUp() = 0;
	// �Ʒ��� ����ũž���� �̵�
	virtual void moveDesktopDown() = 0;
};

