// WindowApp関連のクラス
#pragma once
#include <Windows.h>

namespace JimaXengine
{
	class WinApp
	{
	public:	// 静的メンバ変数
		static const int WINDOW_WIDTH = 1280;	// 画面縦
		static const int WINDOW_HEIGHT = 720;	// 画面横
		static const wchar_t windowClassName[];

	private:	// 静的メンバ関数
		/// <summary>
		/// ウィンドウプロシージャ
		/// </summary>
		/// <param name="hwnd">ウィンドウハンドル</param>
		/// <param name="msg">メッセージ番号</param>
		/// <param name="wparam">メッセージ情報1</param>
		/// <param name="lparam">メッセージ情報2</param>
		/// <returns>成否</returns>
		static LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	private:	// メンバ変数
		HWND hwnd = nullptr;	// ウィンドウハンドル
		WNDCLASSEX wndClass{};	// ウィンドウクラス

	public:		// メンバ関数
		/// <summary>
		/// ウィンドウの作成
		/// </summary>
		void CreateGameWindow();

		/// <summary>
		/// ウィンドウの破棄
		/// </summary>
		void DestroyGameWindow();

		/// <summary>
		/// メッセージ処理
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