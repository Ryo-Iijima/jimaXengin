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

#include <DirectXTex.h>
#pragma comment (lib,"DirectXTex.lib")

#include <d3dx12.h>

/// <summary>
/// DirectX�ėp�����̃N���X
/// �i�Ƃ肠���������Ƃ�����������������Ɉڂ��Ă����j
/// </summary>

class DirectXCommon
{
private:	// �G�C���A�X
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

private:	// �\����
	// ���_�f�[�^�\����
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
	};

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
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH;
	ID3D12Fence* _fence;
	UINT16 _fenceVal = 0;

	float clearColor[4] = { 0.3f,0.3f,0.7f,1.0f };	// ��ʃN���A�F

	// ���_���W
	Vertex vertices[24] =
	{
		// �O
		{{-1.0f,-1.0f,-1.0f},{0.0f,1.0f}},
		{{-1.0f, 1.0f,-1.0f},{0.0f,0.0f}},
		{{ 1.0f,-1.0f,-1.0f},{1.0f,1.0f}},
		{{ 1.0f, 1.0f,-1.0f},{1.0f,0.0f}},
		// ��
		{{-1.0f,-1.0f, 1.0f},{0.0f,1.0f}},
		{{-1.0f, 1.0f, 1.0f},{0.0f,0.0f}},
		{{ 1.0f,-1.0f, 1.0f},{1.0f,1.0f}},
		{{ 1.0f, 1.0f, 1.0f},{1.0f,0.0f}},
		// ��
		{{-1.0f,-1.0f,-1.0f},{0.0f,1.0f}},
		{{-1.0f,-1.0f, 1.0f},{0.0f,0.0f}},
		{{-1.0f, 1.0f,-1.0f},{1.0f,1.0f}},
		{{-1.0f, 1.0f, 1.0f},{1.0f,0.0f}},
		// ��
		{{ 1.0f,-1.0f,-1.0f},{0.0f,1.0f}},
		{{ 1.0f,-1.0f, 1.0f},{0.0f,0.0f}},
		{{ 1.0f, 1.0f,-1.0f},{1.0f,1.0f}},
		{{ 1.0f, 1.0f, 1.0f},{1.0f,0.0f}},
		// ��
		{{-1.0f, 1.0f,-1.0f},{0.0f,1.0f}},
		{{-1.0f, 1.0f, 1.0f},{0.0f,0.0f}},
		{{ 1.0f, 1.0f,-1.0f},{1.0f,1.0f}},
		{{ 1.0f, 1.0f, 1.0f},{1.0f,0.0f}},
		// ��
		{{-1.0f,-1.0f,-1.0f},{0.0f,1.0f}},
		{{-1.0f,-1.0f, 1.0f},{0.0f,0.0f}},
		{{ 1.0f,-1.0f,-1.0f},{1.0f,1.0f}},
		{{ 1.0f,-1.0f, 1.0f},{1.0f,0.0f}},
	};

	// �C���f�b�N�X���
	unsigned short indices[36] =
	{
		// �O
		0,1,2,
		1,2,3,
		// �O
		4,5,6,
		5,6,7,
		// �O
		8,9,10,
		9,10,11,
		// �O
		12,13,14,
		14,15,16,
		// �O
		17,18,19,
		18,19,20,
		// �O
		21,22,23,
		22,23,24,
	};

	ID3D12RootSignature* rootsignature;
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	ID3D12DescriptorHeap* basicDescHeap = nullptr;
	ID3D12PipelineState* _piplineState = nullptr;
	ID3D12Resource* vertBuff = nullptr;
	D3D12_RESOURCE_DESC resdesc = {};
	D3D12_HEAP_PROPERTIES heapprop = {};
	ID3D12Resource* idxBuff = nullptr;
	DirectX::TexMetadata metadata = {};
	ID3D12Resource* texbuff = nullptr;
	ID3DBlob* vsBlob = nullptr;		// �V�F�[�_�[�ێ��p
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	D3D12_DESCRIPTOR_RANGE descTblRange[2] = {};
	D3D12_ROOT_PARAMETER rootparam = {};
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	D3D12_VIEWPORT viewport = {};
	D3D12_RECT scissorrect = {};
	ID3D12Resource* constBuff = nullptr;
	ID3D12DescriptorHeap* dsvHeap = nullptr;

public:		// �����o�֐�

	~DirectXCommon();

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="win"></param>
	void Initialize(WinApp* win);

	/// <summary>
	/// �㏈��
	/// </summary>
	void Finalize();

	/// <summary>
	/// �`��O����
	/// </summary>
	void PreDraw();

	/// <summary>
	/// �`��㏈��
	/// </summary>
	void PostDraw();

	/// <summary>
	/// �`��
	/// </summary>
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


	/// <summary>
	/// ���_�o�b�t�@�̐���
	/// </summary>
	/// <returns>����</returns>
	bool GenerateVertexBuffer();

	/// <summary>
	/// ���_���̃}�b�v
	/// </summary>
	/// <returns>����</returns>
	bool MapVertexBuffer();

	/// <summary>
	/// ���_�o�b�t�@�r���[�̍쐬
	/// </summary>
	void CreateVertexBufferView();

	/// <summary>
	/// �C���f�b�N�X�o�b�t�@�̐���
	/// </summary>
	/// <returns>����</returns>
	bool GenerateIndexBuffer();

	/// <summary>
	/// �C���f�b�N�X���̃}�b�v
	/// </summary>
	/// <returns>����</returns>
	bool MapIndexBuffer();

	/// <summary>
	/// �C���f�b�N�X�o�b�t�@�r���[�̍쐬
	/// </summary>
	void CreateIndexBufferView();

	/// <summary>
	/// �e�N�X�`���o�b�t�@�̐���
	/// </summary>
	/// <returns>����</returns>
	bool GenerateTextureBuffer();

	/// <summary>
	/// �萔�o�b�t�@�̐���
	/// </summary>
	/// <returns>����</returns>
	bool GenerateConstBufferView();

	/// <summary>
	/// �V�F�[�_�[���\�[�X�r���[�̍쐬
	/// </summary>
	/// <returns>����</returns>
	bool CreateTextureShaderResourceView();

	/// <summary>
	/// �V�F�[�_�[�̓ǂݍ��݂Ɛ���
	/// </summary>
	/// <returns>����</returns>
	bool LoadShader();

	/// <summary>
	/// �O���t�B�b�N�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̐���
	/// </summary>
	/// <returns>����</returns>
	bool CreateGPipelineStateObject();

	/// <summary>
	/// ���[�g�p�����[�^�[�̐ݒ�
	/// </summary>
	void SetUpRootParameter();

	/// <summary>
	/// �T���v���[�̐ݒ�
	/// </summary>
	/// <returns>����</returns>
	void SetUpSampler();

	/// <summary>
	/// ���[�g�V�O�l�`���̍쐬
	/// </summary>
	/// <returns>����</returns>
	bool CreatRootSignature();

	/// <summary>
	/// �r���[�|�[�g�̐ݒ�
	/// </summary>
	void SetUpViewport();

	/// <summary>
	/// �V�U�[��`�̐ݒ�
	/// </summary>
	void SetUpScissorrect();
};