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
		3,								// �����̃o�b�t�@��p�ӂ��邱�Ƃ�GPU�̏�����҂����Ɏ��̃R�}���h���X�g�𔭍s���邽�߂̂��̂炵���i�o�b�t�@�̐��H�j
		DXGI_FORMAT_R8G8B8A8_UNORM,
		this->dxCommon->GetHeapForImgui().Get(),
		this->dxCommon->GetHeapForImgui()->GetCPUDescriptorHandleForHeapStart(),	// CPU�n���h��
		this->dxCommon->GetHeapForImgui()->GetGPUDescriptorHandleForHeapStart()		// GPU�n���h��
	);
}

void imGuiDevice::PreDraw()
{
	// �`��O����
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// �E�B���h�E�̒�`
	ImGui::Begin("Test Window");	// �E�B���h�E�̖��O
	ImGui::SetWindowSize(ImVec2(400, 500), ImGuiCond_::ImGuiCond_FirstUseEver);	// �E�B���h�E�T�C�Y
	ImGui::End();
}

void imGuiDevice::Draw()
{
	ImGui::Render();
	dxCommon->GetCommandList()->SetDescriptorHeaps(1, dxCommon->GetHeapForImgui().GetAddressOf());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());
}
