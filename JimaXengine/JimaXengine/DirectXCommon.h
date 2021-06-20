#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include <vector>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#include "WinApp.h"

#include <DirectXMath.h>
#include <d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

/// <summary>
/// DirectX�ėp�����̃N���X
/// �i�Ƃ肠���������Ƃ�����������������Ɉڂ��Ă����j
/// </summary>

class DirectXCommon
{
private:	// �G�C���A�X
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

private:	// �����o�ϐ�
	// �E�B���h�E�Y�A�v���P�[�V�����Ǘ�
	WinApp* winApp;

	HRESULT result;

	ID3D12Device* _dev;
	IDXGIFactory6* _dxgiFactory;
	IDXGISwapChain4* _swapchain;
	ID3D12CommandAllocator* _cmdAllocator;
	ID3D12GraphicsCommandList* _cmdList;
	ID3D12CommandQueue* _cmdQueue;
	std::vector<ID3D12Resource*> _backBuffers;
	ID3D12DescriptorHeap* rtvHeaps;
	UINT bbIdx;
	D3D12_RESOURCE_BARRIER barrierDesc = {};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH;
	ID3D12Fence* _fence;
	UINT16 _fenceVal = 0;

	float clearColor[4] = { 0.3f,0.3f,0.7f,1.0f };	// ��ʃN���A�F

	// �O�p�`�̒��_���W
	DirectX::XMFLOAT3 vertices[4] =
	{
		{-0.4f,-0.7f,0.0f},
		{-0.4f, 0.7f,0.0f},
		{ 0.4f,-0.7f,0.0f},
		{ 0.4f, 0.7f,0.0f},
	};

	unsigned short indices[6] =
	{
		0,1,2,
		2,1,3,
	};

	ID3D12RootSignature* rootsignature;
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	ID3D12PipelineState* _piplineState = nullptr;
	D3D12_VIEWPORT viewport = {};
	D3D12_RECT scissorrect = {};


public:		// �����o�֐�

	~DirectXCommon();

	void Initialize(WinApp* win);

	void Finalize();

	void PreDraw();

	void PostDraw();

	void ClearRenderTarget();

	// getter
	ID3D12Device* GetDevice() { return _dev; }
	ID3D12GraphicsCommandList* GetCommandList() { return _cmdList; }

private:	// �����o�֐�
	/// <summary>
	/// DXGI�f�o�C�X������
	/// </summary>
	/// <returns>����</returns>
	bool InitializeDXGIDevice();

	/// <summary>
	/// �R�}���h�֘A������
	/// </summary>
	/// <returns>����</returns>
	bool InitializeCommand();

	/// <summary>
	/// �X���b�v�`�F�[���̐���
	/// </summary>
	/// <returns>����</returns>
	bool CreateSwapChain();

	/// <summary>
	/// �����_�[�^�[�Q�b�g����
	/// </summary>
	/// <returns>����</returns>
	bool CreateFinalRenderTargets();

	/// <summary>
	/// �t�F���X����
	/// </summary>
	/// <returns>����</returns>
	bool CreateFence();

};