#pragma once
class AeroPeekController
{
public:
	AeroPeekController();
	~AeroPeekController();
	// Aero API�� ����Ͽ� ȭ���� �������ִ� �Լ�
	HRESULT AeroPeekController::registerAero(HWND src, HWND dest, RECT rect, __out HTHUMBNAIL &hthumbnail);
	// ����� �̸����⸦ ������
	HRESULT unregisterAero(HTHUMBNAIL hThumbnail);
private:
	std::list<HTHUMBNAIL> handle_list;
public:
	// ����� ��� �̸����⸦ ������
	DWORD unregisterAllAreo();
};

