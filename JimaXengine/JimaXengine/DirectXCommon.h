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
/// DirectX汎用処理のクラス
/// （とりあえず資料とかやった部分をここに移していく）
/// </summary>

class DirectXCommon
{
private:	// エイリアス
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

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


public:		// メンバ関数

	~DirectXCommon();

	void Initialize(WinApp* win);

	void Finalize();

	void PreDraw();

	void PostDraw();

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

};