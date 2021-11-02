// WindowApp�֘A�̃N���X
#pragma once
#include <Windows.h>

namespace JimaXengine
{
	class WinApp
	{
	public:	// �ÓI�����o�ϐ�
		static const int WINDOW_WIDTH = 1280;	// ��ʏc
		static const int WINDOW_HEIGHT = 720;	// ��ʉ�
		static const wchar_t windowClassName[];

	private:	// �ÓI�����o�֐�
		/// <summary>
		/// �E�B���h�E�v���V�[�W��
		/// </summary>
		/// <param name="hwnd">�E�B���h�E�n���h��</param>
		/// <param name="msg">���b�Z�[�W�ԍ�</param>
		/// <param name="wparam">���b�Z�[�W���1</param>
		/// <param name="lparam">���b�Z�[�W���2</param>
		/// <returns>����</returns>
		static LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	private:	// �����o�ϐ�
		HWND hwnd = nullptr;	// �E�B���h�E�n���h��
		WNDCLASSEX wndClass{};	// �E�B���h�E�N���X

	public:		// �����o�֐�
		/// <summary>
		/// �E�B���h�E�̍쐬
		/// </summary>
		void CreateGameWindow();

		/// <summary>
		/// �E�B���h�E�̔j��
		/// </summary>
		void DestroyGameWindow();

		/// <summary>
		/// ���b�Z�[�W����
		/// </summary>
		/// <returns></returns>
		bool ProcessMessage();

		// getter
		HWND GetHwnd() { return hwnd; }
		WNDCLASSEX GetWndClass() { return wndClass; }
		HINSTANCE GetInstance() { return wndClass.hInstance; }
		int GetWindowWidth() { return WINDOW_WIDTH; }
		int GetWindowHeight() { return WINDOW_HEIGHT; }

	};
}