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
	~E_AeroPeekController();	//���� ������ ����
public:
	//get singleton function
	static E_AeroPeekController* E_AeroPeekController::getSingleton()
	{
		if (E_AeroPeekController::singleton == NULL)
			E_AeroPeekController::singleton = new E_AeroPeekController();
		return E_AeroPeekController::singleton;
	}

	// Aero API�� ����Ͽ� ȭ���� �������ִ� �Լ�
	HRESULT E_AeroPeekController::registerAero(HWND src, HWND dest, RECT rect, __out HTHUMBNAIL &hthumbnail);

	// ����� �̸����⸦ ������
	HRESULT unregisterAero(HTHUMBNAIL hThumbnail);

	// ����� ��� �̸����⸦ ������
	DWORD unregisterAllAreo();
	
private:
	std::list<HTHUMBNAIL> handle_list;
public:
	// AeroPeek�� Ȱ��ȭ �Ǿ��ִ���?
	bool isAeroPeekMode();
	// ����� ��ġ �̵���Ŵ
	HRESULT moveAero(__in HTHUMBNAIL hThumbnail, RECT rect);
	// �ڵ��� �����ϴ��� �˻�
	bool existHandle(HTHUMBNAIL thumbnail);
	
	void enableBlurWindow(HWND hwnd);
	void disableBlurWindow(HWND hwnd);
};

