using namespace std;
#pragma once
class E_Mutex
{
public:
	//mutex
	static mutex updateMutex;	//������ ������Ʈ�� ������ ����Ʈ�� �������� ���� ���ڷ� ó���ϴ� ���ؽ�
	static mutex updateStartStopMutex;	//startUpdate�� stopUpdate�� ���Ǵ� ���ؽ�
	static mutex windowSwitcherEvent;	//Window Switcher���� SHOW�� FOCUS�� �����ϴ� ���ؽ�
	static mutex hotkeyMutex;	//Window Switcher���� SHOW�� FOCUS�� �����ϴ� ���ؽ�
	
	E_Mutex();
	~E_Mutex();
};
