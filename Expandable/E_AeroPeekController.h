
#include "E_Global.h"
using namespace std;

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
	static HRESULT AeroPeekController::UpdateDesktop_Background(HWND hwnd, double left, double top, double right, double bottom)
	{
		HRESULT hr = S_OK;
		// Register the thumbnail
		HTHUMBNAIL thumbnail = NULL;

		HWND hShellWnd = GetShellWindow();
		hr = DwmRegisterThumbnail(hwnd, hShellWnd, &thumbnail);
		CRect temp;
		GetWindowRect(hShellWnd, temp);

		if (SUCCEEDED(hr))
		{
			// The destination rectangle size
			RECT dest = { left, top, right, bottom };

			// Set the thumbnail properties for use
			DWM_THUMBNAIL_PROPERTIES dskThumbProps;
			dskThumbProps.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY;

			// Use the window frame and client area
			dskThumbProps.fSourceClientAreaOnly = TRUE;
			dskThumbProps.fVisible = TRUE;
			//dskThumbProps.opacity = (255 * 70) / 100;
			dskThumbProps.rcDestination = dest;

			// Display the thumbnail
			hr = DwmUpdateThumbnailProperties(thumbnail, &dskThumbProps);
			if (SUCCEEDED(hr))
			{
				// ...
			}
		}
		return hr;
	}

	static HRESULT AeroPeekController::UpdateDesktop_Taskbar(HWND hwnd, double left, double top, double right, double bottom)
	{
		string mainWindowStr = "Shell_TrayWnd",
			exWindowStr = "ReBarWindow32";
		std::wstring l_temp = std::wstring(mainWindowStr.begin(), mainWindowStr.end());
		LPCWSTR l_str = l_temp.c_str();
		std::wstring t_temp = std::wstring(exWindowStr.begin(), exWindowStr.end());
		LPCWSTR t_str = t_temp.c_str();
		HRESULT hr = S_OK;
		// Register the thumbnail
		HTHUMBNAIL thumbnail = NULL;

		HWND taskbar = FindWindowW(l_str, NULL);
		if (NULL != taskbar) {
			CRect sizeRect;
			GetWindowRect(taskbar, &sizeRect);

			HWND child = FindWindowExW(taskbar, NULL, t_str, NULL);
			if (NULL != child) {
				hr = DwmRegisterThumbnail(hwnd, taskbar, &thumbnail);
				if (SUCCEEDED(hr))
				{
					OutputDebugString(L"DRAW SUCCESS\n");

					// 비 계산
					double ratio_y = ((right - left) * E_Global::resolutionHeight) / E_Global::resolutionWidth;

					// The destination rectangle size
					RECT dest = { left, bottom - (ratio_y*(sizeRect.bottom - sizeRect.top) / E_Global::resolutionHeight), right, bottom };
					//RECT dest = { left, bottom - (sizeRect.bottom - sizeRect.top), right, bottom };

					// Set the thumbnail properties for use
					DWM_THUMBNAIL_PROPERTIES dskThumbProps;
					dskThumbProps.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY;

					// Use the window frame and client area
					dskThumbProps.fSourceClientAreaOnly = FALSE;
					dskThumbProps.fVisible = TRUE;
					dskThumbProps.opacity = (255 * 70) / 100;
					dskThumbProps.rcDestination = dest;

					// Display the thumbnail
					hr = DwmUpdateThumbnailProperties(thumbnail, &dskThumbProps);
					if (SUCCEEDED(hr))
					{
						// ...
					}

					return hr;
				}
				else {
					OutputDebugString(L"DRAW FAIL\n");

				}
			}
			else
				OutputDebugString(L"NO CHILD FOUND\n");
		}
		else {
			OutputDebugString(L"NO Chrome FOUND\n");
		}
	}
private:
	std::list<HTHUMBNAIL> handle_list;
public:
	// 등록한 모든 미리보기를 제거함
	DWORD unregisterAllAreo();
};

