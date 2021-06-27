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
/// DirectX汎用処理のクラス
/// （とりあえず資料とかやった部分をここに移していく）
/// </summary>

class DirectXCommon
{
private:	// エイリアス
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

private:	// 構造体
	// 頂点データ構造体
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
	};

private:	// メンバ変数
	// ウィンドウズアプリケーション管理
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

	float clearColor[4] = { 0.3f,0.3f,0.7f,1.0f };	// 画面クリア色

	// 三角形の頂点座標
	Vertex vertices[4] =
	{
		{{-0.4f,-0.7f,0.0f},{0.0f,1.0f}},
		{{-0.4f, 0.7f,0.0f},{0.0f,0.0f}},
		{{ 0.4f,-0.7f,0.0f},{1.0f,1.0f}},
		{{ 0.4f, 0.7f,0.0f},{1.0f,0.0f}},
	};

	unsigned short indices[6] =
	{
		0,1,2,
		2,1,3,
	};

	ID3D12RootSignature* rootsignature;
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	ID3D12DescriptorHeap* texDescHeap = nullptr;
	ID3D12PipelineState* _piplineState = nullptr;
	D3D12_VIEWPORT viewport = {};
	D3D12_RECT scissorrect = {};

	ID3D12Resource* vertBuff = nullptr;
	D3D12_RESOURCE_DESC resdesc = {};
	D3D12_HEAP_PROPERTIES heapprop = {};
	ID3D12Resource* idxBuff = nullptr;
	DirectX::TexMetadata metadata = {};
	ID3D12Resource* texbuff = nullptr;
	ID3DBlob* vsBlob = nullptr;		// シェーダー保持用
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	D3D12_DESCRIPTOR_RANGE descTblRange = {};
	D3D12_ROOT_PARAMETER rootparam = {};
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};


public:		// メンバ関数

	~DirectXCommon();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="win"></param>
	void Initialize(WinApp* win);

	/// <summary>
	/// 後処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 描画前準備
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// 描画
	/// </summary>
	void ClearRenderTarget();

	// getter
	ID3D12Device* GetDevice() { return _dev; }
	ID3D12GraphicsCommandList* GetCommandList() { return _cmdList; }

private:	// メンバ関数
	/// <summary>
	/// DXGIデバイス初期化
	/// </summary>
	/// <returns>成否</returns>
	bool InitializeDXGIDevice();

	/// <summary>
	/// コマンド関連初期化
	/// </summary>
	/// <returns>成否</returns>
	bool InitializeCommand();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	/// <returns>成否</returns>
	bool CreateSwapChain();

	/// <summary>
	/// レンダーターゲット生成
	/// </summary>
	/// <returns>成否</returns>
	bool CreateFinalRenderTargets();

	/// <summary>
	/// フェンス生成
	/// </summary>
	/// <returns>成否</returns>
	bool CreateFence();


	/// <summary>
	/// 頂点バッファの生成
	/// </summary>
	/// <returns>成否</returns>
	bool GenerateVertexBuffer();

	/// <summary>
	/// 頂点情報のマップ
	/// </summary>
	/// <returns>成否</returns>
	bool MapVertexBuffer();

	/// <summary>
	/// 頂点バッファビューの作成
	/// </summary>
	void CreateVertexBufferView();

	/// <summary>
	/// インデックスバッファの生成
	/// </summary>
	/// <returns>成否</returns>
	bool GenerateIndexBuffer();

	/// <summary>
	/// インデックス情報のマップ
	/// </summary>
	/// <returns>成否</returns>
	bool MapIndexBuffer();

	/// <summary>
	/// インデックスバッファビューの作成
	/// </summary>
	void CreateIndexBufferView();

	/// <summary>
	/// テクスチャバッファの生成
	/// </summary>
	/// <returns>成否</returns>
	bool GenerateTextureBuffer();

	/// <summary>
	/// テクスチャ用シェーダーリソースビューの作成
	/// </summary>
	/// <returns>成否</returns>
	bool CreateTextureShaderResourceView();

	/// <summary>
	/// シェーダーの読み込みと生成
	/// </summary>
	/// <returns>成否</returns>
	bool LoadShader();

	/// <summary>
	/// グラフィックパイプラインステートオブジェクトの生成
	/// </summary>
	/// <returns>成否</returns>
	bool CreateGPipelineStateObject();

	/// <summary>
	/// ルートパラメーターの設定
	/// </summary>
	void SetUpRootParameter();

	/// <summary>
	/// サンプラーの設定
	/// </summary>
	/// <returns>成否</returns>
	void SetUpSampler();

	/// <summary>
	/// ルートシグネチャの作成
	/// </summary>
	/// <returns>成否</returns>
	bool CreatRootSignature();

	/// <summary>
	/// ビューポートの設定
	/// </summary>
	void SetUpViewport();

	/// <summary>
	/// シザー矩形の設定
	/// </summary>
	void SetUpScissorrect();
};