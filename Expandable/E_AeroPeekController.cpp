#include "stdafx.h"
#include "E_AeroPeekController.h"
E_AeroPeekController::E_AeroPeekController()
{
}

E_AeroPeekController::~E_AeroPeekController()
{
}

// Aero API�� ����Ͽ� �̸�ȭ���� ������ִ� �Լ�
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

		//����Ʈ�� ���
		handle_list.push_back(hThumbnail);
	}
	return hr;
}


// ����� �̸����⸦ ������
HRESULT E_AeroPeekController::unregisterAero(HTHUMBNAIL hThumbnail)
{
	HRESULT result = DwmUnregisterThumbnail(hThumbnail);
	if (SUCCEEDED(result))
		handle_list.remove(hThumbnail);
	return result;
}

// ��ϵ� ��� �̸����� ����
DWORD E_AeroPeekController::unregisterAllAreo()
{
	DWORD result = TRUE;
	for (std::list<HTHUMBNAIL>::iterator iterator = handle_list.begin(); iterator != handle_list.end();) {
		//��� �ڵ� ��ȯ
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