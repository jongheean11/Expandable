using namespace std;
#pragma once
class E_Mutex
{
public:
	//mutex
	static mutex updateMutex;	//윈도우 업데이트와 윈도우 리스트를 가져오는 것을 원자로 처리하는 뮤텍스
	static mutex updateStartStopMutex;	//startUpdate와 stopUpdate에 사용되는 뮤텍스
	E_Mutex();
	~E_Mutex();
};
