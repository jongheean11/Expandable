#pragma once
class E_GlobalUpdater
{
public:
	E_GlobalUpdater();
	~E_GlobalUpdater();
	//�ػ󵵰� ����Ǿ��� ��� �����ϴ� �Լ� (���� ���� �Լ�)
	virtual void OnChangeResolution(double width, double height){};// = 0;
	//������� ��� Ȱ��/��Ȱ�� �̺�Ʈ�� �����ϴ� �Լ� (���� ���� �Լ�)
	virtual void OnChangeAeroPeek(bool result){};// = 0;
	// ��� ����Ͱ� ����Ǿ����� �����Ǿ��� ��� �̺�Ʈ�� �����ϴ� �Լ� (���� ���� �Լ�)
	virtual void OnDualMonitorMode(bool result){};// = 0;
};

