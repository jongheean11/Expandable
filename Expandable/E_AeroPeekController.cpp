#include "stdafx.h"
#include "E_AeroPeekController.h"
E_AeroPeekController::E_AeroPeekController()
{
}

E_AeroPeekController::~E_AeroPeekController()
{
}
E_AeroPeekController* E_AeroPeekController::singleton = NULL;

// Aero API를 사용하여 미리화면을 등록해주는 함수
HRESULT E_AeroPeekController::registerAero(HWND src, HWND dest, RECT rect, __out HTHUMBNAIL& hThumbnail)
{
	HRESULT hr = S_OK;

	hr = DwmRegisterThumbnail(dest, src, &hThumbnail);
	if (SUCCEEDED(hr))  {
		// Set the thumbnail properties for use
		DWM_THUMBNAIL_PROPERTIES dskThumbProps;
		dskThumbProps.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY;

		// Use the window frame and client area
		dskThumbProps.fSourceClientAreaOnly = FALSE;
		dskThumbProps.fVisible = TRUE;
		dskThumbProps.opacity = (255 * 70) / 100;
		dskThumbProps.rcDestination = rect;

		// Display the thumbnail
		hr = DwmUpdateThumbnailProperties(hThumbnail, &dskThumbProps);

		//리스트에 등록
		handle_list.push_back(hThumbnail);
	}
	return hr;
}


// 등록한 미리보기를 제거함
HRESULT E_AeroPeekController::unregisterAero(HTHUMBNAIL hThumbnail)
{
	HRESULT result = DwmUnregisterThumbnail(hThumbnail);
	if (SUCCEEDED(result))
		handle_list.remove(hThumbnail);
	return result;
}

// 등록된 모든 미리보기 제거
DWORD E_AeroPeekController::unregisterAllAreo()
{
	DWORD result = TRUE;
	for (std::list<HTHUMBNAIL>::iterator iterator = handle_list.begin(); iterator != handle_list.end();) {
		//모든 핸들 반환
		if (SUCCEEDED(DwmUnregisterThumbnail(*iterator))) {
			//iterator = 
			iterator = handle_list.erase(iterator);
		}
		else {
			result = FALSE;
			iterator++;
		}
	}
	return result;
}