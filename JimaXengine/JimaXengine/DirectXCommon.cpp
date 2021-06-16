#include "DirectXCommon.h"
#include <string>
#include <cassert>

DirectXCommon::~DirectXCommon()
{
	Finalize();
}

void DirectXCommon::Initialize(WinApp* winApp)
{
	// nullptr�`�F�b�N
	assert(winApp);

	this->winApp = winApp;

	// DXGI�f�o�C�X������
	if (!InitializeDXGIDevice()) {
		assert(0);
	}

	// �R�}���h�֘A������
	if (!InitializeCommand()) {
		assert(0);
	}

	// �X���b�v�`�F�[���̐���
	if (!CreateSwapChain()) {
		assert(0);
	}

	// �����_�[�^�[�Q�b�g����
	if (!CreateFinalRenderTargets()) {
		assert(0);
	}

	// �t�F���X����
	if (!CreateFence()) {
		assert(0);
	}

}

void DirectXCommon::Finalize()
{
}

void DirectXCommon::PreDraw()
{
	result = _cmdAllocator->Reset();	// �R�}���h�A���P�[�^�[�̃N���A
	bbIdx = _swapchain->GetCurrentBackBufferIndex();	// ���݂̃o�b�N�o�b�t�@�̃C���f�b�N�X

#pragma region ���\�[�X�o���A��ύX

	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	// �J��
	barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		// ���ɂȂ�
	barrierDesc.Transition.pResource = _backBuffers[bbIdx];		// �o�b�N�o�b�t�@���\�[�X
	barrierDesc.Transition.Subresource = 0;
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;		// �\������
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;	// �`���
	_cmdList->ResourceBarrier(1, &barrierDesc);		// �o���A�w����s

#pragma endregion

#pragma region ��ʃN���A�R�}���h
	rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);

	ClearRenderTarget();

#pragma endregion

}

void DirectXCommon::PostDraw()
{
#pragma region ���\�[�X�o���A��߂�
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;		// �`�悩��
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;				// �\����
	_cmdList->ResourceBarrier(1, &barrierDesc);		// �o���A�w����s

#pragma endregion

#pragma region �`��R�}���h
	_cmdList->Close();	// ���߂̃N���[�Y

	ID3D12CommandList* cmdLists[] = { _cmdList };	// ���s����R�}���h��p�z��
	_cmdQueue->ExecuteCommandLists(1, cmdLists);	// �R�}���h���X�g�̎��s

	_cmdQueue->Signal(_fence, ++_fenceVal);
	if (_fence->GetCompletedValue() != _fenceVal)
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);	// �C�x���g�n���h���̎擾
		_fence->SetEventOnCompletion(_fenceVal, event);
		WaitForSingleObject(event, INFINITE);	// �C�x���g����������܂ő҂�������
		CloseHandle(event);		// �C�x���g�n���h�������
	}

	_cmdAllocator->Reset();	// �L���[���N���A
	_cmdList->Reset(_cmdAllocator, nullptr);	// �ĂуR�}���h���X�g�����߂鏀��

	_swapchain->Present(1, 0);	// �o�b�t�@���t���b�v

#pragma endregion

}

void DirectXCommon::ClearRenderTarget()
{
	bbIdx = _swapchain->GetCurrentBackBufferIndex();	// ���݂̃o�b�N�o�b�t�@�̃C���f�b�N�X

	rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);	// ��ʃN���A
}

bool DirectXCommon::InitializeDXGIDevice()
{
#ifdef DEBUG
	// �f�o�b�O���C���[���I����
	ID3D12Debug* debugContoroller;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugContoroller))))
	{
		debugContoroller->EnableDebugLayer();
	}

	//debugContoroller->Release();
#endif // DEBUG

	// �t�B�[�`���[���x����A���Ń`�F�b�N����p�z��
	D3D_FEATURE_LEVEL levels[4] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	_dxgiFactory = nullptr;

	result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));	// DXGIFactory��_dxgiFactory�ɓ����
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	std::vector<IDXGIAdapter*> adapters;	// �A�_�v�^�[�̗񋓗p
	IDXGIAdapter* tmpAdapter = nullptr;		// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	for (int i = 0; _dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);	// �A�_�v�^�[�̐����I�u�W�F�N�g�擾
		std::wstring strDesc = adesc.Description;

		// �T�������A�_�v�^�[�̖��O���m�F
		if (strDesc.find(L"NVIDIA") != std::string::npos)
		{
			tmpAdapter = adpt;
			break;
		}
	}

	D3D_FEATURE_LEVEL featureLevel;

	_dev = nullptr;
	result = S_FALSE;
	for (auto lv : levels)
	{
		// �f�o�C�X��������������
		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&_dev)) == S_OK)
		{
			featureLevel = lv;
			break;	// �����\�ȃo�[�W���������������甲����
		}
	}
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	return true;
}

bool DirectXCommon::InitializeCommand()
{
	result = S_FALSE;

	// �R�}���h���X�g�ƃR�}���h�A���P�[�^�[�̐���
	_cmdAllocator = nullptr;
	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	_cmdList = nullptr;
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// �R�}���h�L���[�[�̐ݒ�Ɛ���
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};

	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;				// �^�C���A�E�g�Ȃ�
	cmdQueueDesc.NodeMask = 0;										// �A�_�v�^�[���P�����g��Ȃ��Ƃ��͂O�ł���
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	// �v���C�I���e�B�͓��Ɏw��Ȃ�
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;				// �R�}���h���X�g�ƍ��킹��

	_cmdQueue = nullptr;
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));		// �L���[����
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	return true;
}

bool DirectXCommon::CreateSwapChain()
{
	result = S_FALSE;

	// �X���b�v�`�F�[���̐ݒ�Ɛ���
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};

	swapchainDesc.Width = WinApp::WINDOW_WIDTH;
	swapchainDesc.Height = WinApp::WINDOW_HEIGHT;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;									// �\�Ɨ��œ��
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;					// �o�b�N�o�b�t�@�͐L�яk�݉\
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// �t���b�v������j��
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;			// ���Ɏw��Ȃ�
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// �E�B���h�E�ƃt���X�N���[���̐؂�ւ��\

	_swapchain = nullptr;
	result = _dxgiFactory->CreateSwapChainForHwnd
	(
		_cmdQueue,
		winApp->GetHwnd(),
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)&_swapchain
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	return true;
}

bool DirectXCommon::CreateFinalRenderTargets()
{
	result = S_FALSE;

	// �f�X�N���v�^�q�[�v���쐬
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;		// �����_�[�^�[�Q�b�g�r���[
	//heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;						// ���\��2��
	//heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// ���Ɏw��Ȃ�

	rtvHeaps = nullptr;
	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// �X���b�v�`�F�[���̃������ƕR�Â���
	DXGI_SWAP_CHAIN_DESC swcDesc = {};

	result = _swapchain->GetDesc(&swcDesc);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	_backBuffers.resize(swcDesc.BufferCount);
	for (int i = 0; i < swcDesc.BufferCount; i++)
	{
		result = _swapchain->GetBuffer(i, IID_PPV_ARGS(&_backBuffers[i]));	// �X���b�v�`�F�C������o�b�t�@���擾
		if (FAILED(result)) {
			assert(0);
			return result;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();	// �f�X�N���v�^�q�[�v�̃n���h�����擾
		handle.ptr += i * _dev->GetDescriptorHandleIncrementSize(heapDesc.Type);	// ���\�̃A�h���X�̂���
		_dev->CreateRenderTargetView(_backBuffers[i], nullptr, handle);			// RTV�̐���
	}

	return true;
}

bool DirectXCommon::CreateFence()
{
	// �t�F���X�̐���
	_fence = nullptr;
	result = _dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	return true;
}
