#pragma once
class AeroPeekController
{
public:
	AeroPeekController();
	~AeroPeekController();
	// Aero API를 사용하여 화면을 복사해주는 함수
	HRESULT AeroPeekController::registerAero(HWND src, HWND dest, RECT rect, __out HTHUMBNAIL &hthumbnail);
	// 등록한 미리보기를 제거함
	HRESULT unregisterAero(HTHUMBNAIL hThumbnail);
private:
	std::list<HTHUMBNAIL> handle_list;
public:
	// 등록한 모든 미리보기를 제거함
	DWORD unregisterAllAreo();
};

