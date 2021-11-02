#pragma once
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx12.h"

#include "../WinApp.h"
#include "../DirectXCommon.h"

namespace JimaXengine
{
	class ImGuiDevice
	{
	private:
		WinApp* winApp = nullptr;
		DirectXCommon* dxCommon = nullptr;

	public:
		ImGuiDevice();

		~ImGuiDevice();

		void Initialize(WinApp* winApp, DirectXCommon* dxCommon);

		void Update();

		/// <summary>
		/// UpdateとDrawの間でウィンドウの設定をする
		/// </summary>
		void Draw();
	};

}