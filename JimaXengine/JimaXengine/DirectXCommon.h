#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include <vector>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#include "WinApp.h"

/// <summary>
/// DirectX汎用処理のクラス
/// （とりあえず資料とかやった部分をここに移していく）
/// </summary>

class DirectXCommon
{
private:	// エイリアス
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	float clearColor[4] = { 0.3f,0.3f,0.7f,1.0f };	// 画面クリア色

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