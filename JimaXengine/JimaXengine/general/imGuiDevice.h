/// imGui�g�����߂̃N���X

#pragma once
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx12.h"

#include "../WinApp.h"
#include "../DirectXCommon.h"

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
	/// Update��Draw�̊ԂŃE�B���h�E�̐ݒ������
	/// </summary>
	void Draw();
};

