#include "imGuiDevice.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"

void imGuiDevice::Initialize(WinApp* winApp, DirectXCommon* dxCommon)
{
	this->winApp = winApp;
	this->dxCommon = dxCommon;

	if (ImGui::CreateContext() == nullptr)
	{
		assert(0);
	}

	bool blnResult = ImGui_ImplWin32_Init(this->winApp->GetHwnd());
	if (!blnResult)
	{
		assert(0);
	}
	
	blnResult = ImGui_ImplDX12_Init
	(
		this->dxCommon->GetDevice(),
		3,								// 複数のバッファを用意することでGPUの処理を待たずに次のコマンドリストを発行するためのものらしい（バッファの数？）
		DXGI_FORMAT_R8G8B8A8_UNORM,
		this->dxCommon->GetHeapForImgui().Get(),
		this->dxCommon->GetHeapForImgui()->GetCPUDescriptorHandleForHeapStart(),	// CPUハンドル
		this->dxCommon->GetHeapForImgui()->GetGPUDescriptorHandleForHeapStart()		// GPUハンドル
	);
}

void imGuiDevice::PreDraw()
{
	// 描画前処理
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// ウィンドウの定義
	ImGui::Begin("Test Window");	// ウィンドウの名前
	ImGui::SetWindowSize(ImVec2(400, 500), ImGuiCond_::ImGuiCond_FirstUseEver);	// ウィンドウサイズ
	ImGui::End();
}

void imGuiDevice::Draw()
{
	ImGui::Render();
	dxCommon->GetCommandList()->SetDescriptorHeaps(1, dxCommon->GetHeapForImgui().GetAddressOf());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());
}
