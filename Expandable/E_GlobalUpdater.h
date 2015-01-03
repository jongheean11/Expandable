#pragma once
class E_GlobalUpdater
{
public:
	E_GlobalUpdater();
	~E_GlobalUpdater();
	//해상도가 변경되었을 경우 전달하는 함수 (순수 가상 함수)
	virtual void OnChangeResolution(double width, double height){};// = 0;
	//에어로픽 모드 활성/비활성 이벤트를 전달하는 함수 (순수 가상 함수)
	virtual void OnChangeAeroPeek(bool result){};// = 0;
	// 듀얼 모니터가 연결되었음이 감지되었을 경우 이벤트를 전달하는 함수 (순수 가상 함수)
	virtual void OnDualMonitorMode(bool result){};// = 0;
};

