#include "DirectXCommon.h"
#include <string>
#include <cassert>

DirectXCommon::~DirectXCommon()
{
	Finalize();
}

void DirectXCommon::Initialize(WinApp* winApp)
{
	// nullptrチェック
	assert(winApp);

	this->winApp = winApp;

	// DXGIデバイス初期化
	if (!InitializeDXGIDevice()) {
		assert(0);
	}

	// コマンド関連初期化
	if (!InitializeCommand()) {
		assert(0);
	}

	// スワップチェーンの生成
	if (!CreateSwapChain()) {
		assert(0);
	}

	// レンダーターゲット生成
	if (!CreateFinalRenderTargets()) {
		assert(0);
	}

	// フェンス生成
	if (!CreateFence()) {
		assert(0);
	}

}

void DirectXCommon::Finalize()
{
}

void DirectXCommon::PreDraw()
{
	result = _cmdAllocator->Reset();	// コマンドアロケーターのクリア
	bbIdx = _swapchain->GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックス

#pragma region リソースバリアを変更

	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	// 遷移
	barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		// 特になし
	barrierDesc.Transition.pResource = _backBuffers[bbIdx];		// バックバッファリソース
	barrierDesc.Transition.Subresource = 0;
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;		// 表示から
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;	// 描画へ
	_cmdList->ResourceBarrier(1, &barrierDesc);		// バリア指定実行

#pragma endregion

#pragma region 画面クリアコマンド
	rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);

	ClearRenderTarget();

#pragma endregion

}

void DirectXCommon::PostDraw()
{
#pragma region リソースバリアを戻す
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;		// 描画から
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;				// 表示へ
	_cmdList->ResourceBarrier(1, &barrierDesc);		// バリア指定実行

#pragma endregion

#pragma region 描画コマンド
	_cmdList->Close();	// 命令のクローズ

	ID3D12CommandList* cmdLists[] = { _cmdList };	// 実行するコマンドのp配列
	_cmdQueue->ExecuteCommandLists(1, cmdLists);	// コマンドリストの実行

	_cmdQueue->Signal(_fence, ++_fenceVal);
	if (_fence->GetCompletedValue() != _fenceVal)
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);	// イベントハンドルの取得
		_fence->SetEventOnCompletion(_fenceVal, event);
		WaitForSingleObject(event, INFINITE);	// イベントが発生するまで待ち続ける
		CloseHandle(event);		// イベントハンドルを閉じる
	}

	_cmdAllocator->Reset();	// キューをクリア
	_cmdList->Reset(_cmdAllocator, nullptr);	// 再びコマンドリストをためる準備

	_swapchain->Present(1, 0);	// バッファをフリップ

#pragma endregion

}

void DirectXCommon::ClearRenderTarget()
{
	bbIdx = _swapchain->GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックス

	rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);	// 画面クリア
}

bool DirectXCommon::InitializeDXGIDevice()
{
#ifdef DEBUG
	// デバッグレイヤーをオンに
	ID3D12Debug* debugContoroller;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugContoroller))))
	{
		debugContoroller->EnableDebugLayer();
	}

	//debugContoroller->Release();
#endif // DEBUG

	// フィーチャーレベルを連続でチェックする用配列
	D3D_FEATURE_LEVEL levels[4] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	_dxgiFactory = nullptr;

	result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));	// DXGIFactoryを_dxgiFactoryに入れる
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	std::vector<IDXGIAdapter*> adapters;	// アダプターの列挙用
	IDXGIAdapter* tmpAdapter = nullptr;		// ここに特定の名前を持つアダプターオブジェクトが入る
	for (int i = 0; _dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);	// アダプターの説明オブジェクト取得
		std::wstring strDesc = adesc.Description;

		// 探したいアダプターの名前を確認
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
		// デバイス生成が成功して
		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&_dev)) == S_OK)
		{
			featureLevel = lv;
			break;	// 生成可能なバージョンが見つかったら抜ける
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

	// コマンドリストとコマンドアロケーターの生成
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

	// コマンドキューーの設定と生成
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};

	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;				// タイムアウトなし
	cmdQueueDesc.NodeMask = 0;										// アダプターを１つしか使わないときは０でいい
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	// プライオリティは特に指定なし
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;				// コマンドリストと合わせる

	_cmdQueue = nullptr;
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));		// キュー生成
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	return true;
}

bool DirectXCommon::CreateSwapChain()
{
	result = S_FALSE;

	// スワップチェーンの設定と生成
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};

	swapchainDesc.Width = WinApp::WINDOW_WIDTH;
	swapchainDesc.Height = WinApp::WINDOW_HEIGHT;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;									// 表と裏で二つ
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;					// バックバッファは伸び縮み可能
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// フリップしたら破棄
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;			// 特に指定なし
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// ウィンドウとフルスクリーンの切り替え可能

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

	// デスクリプタヒープを作成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;		// レンダーターゲットビュー
	//heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;						// 裏表で2枚
	//heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// 特に指定なし

	rtvHeaps = nullptr;
	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// スワップチェーンのメモリと紐づける
	DXGI_SWAP_CHAIN_DESC swcDesc = {};

	result = _swapchain->GetDesc(&swcDesc);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	_backBuffers.resize(swcDesc.BufferCount);
	for (int i = 0; i < swcDesc.BufferCount; i++)
	{
		result = _swapchain->GetBuffer(i, IID_PPV_ARGS(&_backBuffers[i]));	// スワップチェインからバッファを取得
		if (FAILED(result)) {
			assert(0);
			return result;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();	// デスクリプタヒープのハンドルを取得
		handle.ptr += i * _dev->GetDescriptorHandleIncrementSize(heapDesc.Type);	// 裏表のアドレスのずれ
		_dev->CreateRenderTargetView(_backBuffers[i], nullptr, handle);			// RTVの生成
	}

	return true;
}

bool DirectXCommon::CreateFence()
{
	// フェンスの生成
	_fence = nullptr;
	result = _dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	return true;
}
