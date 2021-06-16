#include "WinApp.h"

const wchar_t WinApp::windowClassName[] = L"DirectXGame";

LRESULT WinApp::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// �E�B���h�E���j�����ꂽ��Ă΂��
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);	// OS�ɑ΂��āu�������̃A�v���͏I���v�Ɠ`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);	// ����̏������s��
}

void WinApp::CreateGameWindow()
{
	// �E�B���h�E�N���X�̐ݒ�
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = (WNDPROC)WindowProcedure;	// �R�[���o�b�N�֐��̎w��
	wndClass.lpszClassName = TEXT("DX12Sample");			// �A�v���P�[�V�����N���X��
	wndClass.hInstance = GetModuleHandle(nullptr);		// �n���h���̎擾

	RegisterClassEx(&wndClass);	// �A�v���P�[�V�����N���X

	RECT wrc = { 0,0,WINDOW_WIDTH,WINDOW_HEIGHT };	//�E�B���h�E�T�C�Y�����߂�

	// �֐����g���ăE�B���h�E�̃T�C�Y��␳����
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// �E�B���h�E�I�u�W�F�N�g�̐���
	hwnd = CreateWindow
	(
		wndClass.lpszClassName,
		TEXT("DX12�e�X�g"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		wndClass.hInstance,
		nullptr
	);

	// �E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);
}

void WinApp::DestroyGameWindow()
{
	// �����N���X�͎g��Ȃ��̂œo�^��������
	UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);
}

bool WinApp::ProcessMessage()
{
	MSG msg = {};

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// �A�v���P�[�V�������I���Ƃ���message��WM_QUIT�ɂȂ�
	if (msg.message == WM_QUIT)
	{
		return true;
	}

	return false;
}