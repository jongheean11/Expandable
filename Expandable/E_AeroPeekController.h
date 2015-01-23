#pragma once
//#include "E_Global.h"
#include "E_EnvironmentManager.h"
using namespace std;

class E_AeroPeekController
{
private:
	//singleton variable
	static E_AeroPeekController* singleton;
	DWM_THUMBNAIL_PROPERTIES dskThumbProps; // Set the thumbnail properties for use
	//private constructor
	E_AeroPeekController();		
	~E_AeroPeekController();	//강제 해제를 막음
public:
	//get singleton function
	static E_AeroPeekController* E_AeroPeekController::getSingleton()
	{
		if (E_AeroPeekController::singleton == NULL)
			E_AeroPeekController::singleton = new E_AeroPeekController();
		return E_AeroPeekController::singleton;
	}

	// Aero API를 사용하여 화면을 복사해주는 함수
	HRESULT E_AeroPeekController::registerAero(HWND src, HWND dest, RECT rect, __out HTHUMBNAIL &hthumbnail);

	// 등록한 미리보기를 제거함
	HRESULT unregisterAero(HTHUMBNAIL hThumbnail);

	// 등록한 모든 미리보기를 제거함
	DWORD unregisterAllAreo();
	
private:
	std::list<HTHUMBNAIL> handle_list;
public:
	// AeroPeek이 활성화 되어있는지?
	bool isAeroPeekMode();
	// 에어로 위치 이동시킴
	HRESULT moveAero(__in HTHUMBNAIL hThumbnail, RECT rect);
	// 핸들이 존재하는지 검사
	bool existHandle(HTHUMBNAIL thumbnail);
	
	void enableBlurWindow(HWND hwnd);
	void disableBlurWindow(HWND hwnd);
};

