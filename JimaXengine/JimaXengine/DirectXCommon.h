// DirectX�̏������Ƃ肠�����܂Ƃ߂Ă�N���X

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
using namespace DirectX;

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "Camera.h"


class DirectXCommon
{
private:	// �G�C���A�X
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

private:	// �\����
	// ���_�f�[�^�\����
	struct Vertex
	{
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
	};

	struct ConstBfferData
	{
		XMMATRIX matrix;
		XMFLOAT4 color;
	};

private:	// �����o�ϐ�
	// �E�B���h�E�Y�A�v���P�[�V�����Ǘ�
	WinApp* winApp = nullptr;

	HRESULT result;

	ComPtr<ID3D12Device> _dev;
	ComPtr<IDXGIFactory6> _dxgiFactory;
	ComPtr<IDXGISwapChain4> _swapchain;
	ComPtr<ID3D12CommandAllocator> _cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList> _cmdList;
	ComPtr<ID3D12CommandQueue> _cmdQueue;
	std::vector<ID3D12Resource*> _backBuffers;
	ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	UINT bbIdx;
	D3D12_RESOURCE_BARRIER barrierDesc = {};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH;
	ComPtr<ID3D12Fence> _fence;
	UINT16 _fenceVal = 0;

	float clearColor[4] = { 0.3f,0.3f,0.7f,1.0f };	// ��ʃN���A�F

	ComPtr<ID3D12RootSignature> rootsignature;
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	ComPtr<ID3D12DescriptorHeap> basicDescHeap;
	ComPtr<ID3D12PipelineState> _piplineState = nullptr;
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	D3D12_RESOURCE_DESC resdesc = {};
	D3D12_HEAP_PROPERTIES heapprop = {};
	ComPtr<ID3D12Resource> idxBuff = nullptr;
	TexMetadata metadata = {};
	ComPtr<ID3D12Resource> texbuff = nullptr;
	ComPtr<ID3DBlob> vsBlob = nullptr;		// �V�F�[�_�[�ێ��p
	ComPtr<ID3DBlob> psBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	D3D12_DESCRIPTOR_RANGE descTblRange[2] = {};
	D3D12_ROOT_PARAMETER rootparam = {};
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	D3D12_VIEWPORT viewport = {};
	D3D12_RECT scissorrect = {};
	ComPtr<ID3D12Resource> depthBuffer = nullptr;
	ComPtr<ID3D12Resource> constBuff = nullptr;
	ComPtr<ID3D12DescriptorHeap> dsvHeap = nullptr;

	XMMATRIX matWorld;			// ���[���h
	XMMATRIX matView;			// �r���[
	XMMATRIX matProjection;		// �v���W�F�N�V����

	// imgui�p�q�[�v
	ComPtr<ID3D12DescriptorHeap> _heapForImgui;

	// ���_���W
	const static unsigned int vertNum = 24;
	Vertex vertices[vertNum] =
	{
		// �O
		{{-1.0f,-1.0f,-1.0f}, {}, {0.0f,1.0f}},
		{{-1.0f, 1.0f,-1.0f}, {}, {0.0f,0.0f}},
		{{ 1.0f,-1.0f,-1.0f}, {}, {1.0f,1.0f}},
		{{ 1.0f, 1.0f,-1.0f}, {}, {1.0f,0.0f}},
		// ��					 
		{{ 1.0f,-1.0f, 1.0f}, {}, {0.0f,1.0f}},
		{{ 1.0f, 1.0f, 1.0f}, {}, {0.0f,0.0f}},
		{{-1.0f,-1.0f, 1.0f}, {}, {1.0f,1.0f}},
		{{-1.0f, 1.0f, 1.0f}, {}, {1.0f,0.0f}},
		// ��					 
		{{-1.0f,-1.0f,-1.0f}, {}, {0.0f,1.0f}},
		{{-1.0f,-1.0f, 1.0f}, {}, {0.0f,0.0f}},
		{{-1.0f, 1.0f,-1.0f}, {}, {1.0f,1.0f}},
		{{-1.0f, 1.0f, 1.0f}, {}, {1.0f,0.0f}},
		// �E					 
		{{ 1.0f,-1.0f,-1.0f}, {}, {0.0f,1.0f}},
		{{ 1.0f, 1.0f,-1.0f}, {}, {0.0f,0.0f}},
		{{ 1.0f,-1.0f, 1.0f}, {}, {1.0f,1.0f}},
		{{ 1.0f, 1.0f, 1.0f}, {}, {1.0f,0.0f}},
		// ��					 
		{{-1.0f, 1.0f,-1.0f}, {}, {0.0f,1.0f}},
		{{-1.0f, 1.0f, 1.0f}, {}, {0.0f,0.0f}},
		{{ 1.0f, 1.0f,-1.0f}, {}, {1.0f,1.0f}},
		{{ 1.0f, 1.0f, 1.0f}, {}, {1.0f,0.0f}},
		// ��
		{{ 1.0f,-1.0f,-1.0f}, {}, {0.0f,1.0f}},
		{{ 1.0f,-1.0f, 1.0f}, {}, {0.0f,0.0f}},
		{{-1.0f,-1.0f,-1.0f}, {}, {1.0f,1.0f}},
		{{-1.0f,-1.0f, 1.0f}, {}, {1.0f,0.0f}},
	};

	// �C���f�b�N�X���
	const static unsigned short indicesNum = 36;
	unsigned short indices[indicesNum] =
	{
		// �O
		0,1,2,
		2,1,3,
		// ��
		4,5,6,
		6,5,7,
		// ��
		8,9,10,
		10,9,11,
		// �E
		12,13,14,
		14,13,15,
		// ��
		16,17,18,
		18,17,19,
		// ��
		20,21,22,
		22,21,23,
	};

	
	Camera* camera = nullptr;

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
	ID3D12Device* GetDevice() { return _dev.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return _cmdList.Get(); }

	// imgui�p�̃q�[�v����
	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeapForImgui();
	ComPtr<ID3D12DescriptorHeap> GetHeapForImgui();

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
	/// �萔�o�b�t�@�r���[�̐���
	/// </summary>
	/// <returns>����</returns>
	bool GenerateConstBufferView();

	/// <summary>
	/// �[�x�o�b�t�@�̐���
	/// </summary>
	/// <returns>����</returns>
	bool GenerateDepthBuffer();

	/// <summary>
	/// �[�x�o�b�t�@�r���[�̐���
	/// </summary>
	/// <returns>����</returns>
	bool GenerateDepthBufferView();

	/// <summary>
	/// �s��v�Z
	/// </summary>
	void CalculateMat();

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