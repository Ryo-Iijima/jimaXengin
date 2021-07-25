#include "imGuiDevice.h"

ImGuiDevice::ImGuiDevice()
{
}

ImGuiDevice::~ImGuiDevice()
{
}

void ImGuiDevice::Initialize(WinApp* winApp, DirectXCommon* dxCommon)
{
	this->winApp = winApp;
	this->dxCommon = dxCommon;

	if (ImGui::CreateContext() == nullptr)
	{
		assert(0);
	}
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

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

void ImGuiDevice::Update()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiDevice::Draw()
{
	ImGui::Render();
	dxCommon->GetCommandList()->SetDescriptorHeaps(1, dxCommon->GetHeapForImgui().GetAddressOf());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());
}
