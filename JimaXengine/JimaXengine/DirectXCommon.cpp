#include "DirectXCommon.h"
#include <string>
#include <cassert>
#include <DirectXTexWIC.cpp>


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
	if (!InitializeDXGIDevice()) 
	{
		assert(0);
	}

	// コマンド関連初期化
	if (!InitializeCommand())
	{
		assert(0);
	}

	// スワップチェーンの生成
	if (!CreateSwapChain())
	{
		assert(0);
	}

	// レンダーターゲット生成
	if (!CreateFinalRenderTargets()) 
	{
		assert(0);
	}

	// フェンス生成
	if (!CreateFence()) 
	{
		assert(0);
	}

	// 頂点バッファの生成
	if (!GenerateVertexBuffer())
	{
		assert(0);
	}

	// 頂点情報のマップ
	if (!MapVertexBuffer())
	{
		assert(0);
	}

	// 頂点バッファビューの作成
	CreateVertexBufferView();

	// インデックスバッファの生成
	if (!GenerateIndexBuffer())
	{
		assert(0);
	}

	// インデックスバッファのマップ
	if (!MapIndexBuffer())
	{
		assert(0);
	}

	// インデックスバッファビューの作成
	CreateIndexBufferView();

	// テクスチャバッファの生成
	if (!GenerateTextureBuffer())
	{
		assert(0);
	}

	// 定数バッファの生成
	if (!GenerateConstBufferView())
	{
		assert(0);
	}

#pragma region 深度バッファ
	// リソース作成
	D3D12_RESOURCE_DESC depthResDesc = {};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = WinApp::WINDOW_WIDTH;		// レンダーターゲットのサイズ
	depthResDesc.Height = WinApp::WINDOW_HEIGHT;
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;	// 深度値フォーマット
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	// 深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp = {};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	// 深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue = {};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	// リソース生成
	ID3D12Resource* depthBuffer = nullptr;
	result = _dev->CreateCommittedResource
	(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	// 深度値書き込み用
		&depthClearValue,
		IID_PPV_ARGS(&depthBuffer)
	);
	if (FAILED(result)) {
		assert(0);
	}

#pragma endregion

#pragma region 深度バッファビュー
	// 深度ビュー用デスクリプターヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	result = _dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(dsvHeap.GetAddressOf()));
	if (FAILED(result)) {
		assert(0);
	}

	// 深度ビュー生成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;	// 深度値に32ビット使う
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;	// 特になし

	_dev->CreateDepthStencilView
	(
		depthBuffer,
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);



#pragma endregion
	// シェーダーリソースビューの作成
	if (!CreateTextureShaderResourceView())
	{
		assert(0);
	}

	// シェーダーの読み込み
	if (!LoadShader())
	{
		assert(0);
	}

	// グラフィックスパイプラインステートオブジェクトの作成
	if (!CreateGPipelineStateObject())
	{
		assert(0);
	}

	// ビューポートの設定
	SetUpViewport();

	// シザー矩形の設定
	SetUpScissorrect();
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

	dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	_cmdList->OMSetRenderTargets(1, &rtvH, true, &dsvH);

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

	ID3D12CommandList* cmdLists[] = { _cmdList.Get() };	// 実行するコマンドのp配列
	_cmdQueue->ExecuteCommandLists(1, cmdLists);	// コマンドリストの実行

	_cmdQueue->Signal(_fence.Get(), ++_fenceVal);
	if (_fence->GetCompletedValue() != _fenceVal)
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);	// イベントハンドルの取得
		_fence->SetEventOnCompletion(_fenceVal, event);
		WaitForSingleObject(event, INFINITE);	// イベントが発生するまで待ち続ける
		CloseHandle(event);		// イベントハンドルを閉じる
	}

	_cmdAllocator->Reset();	// キューをクリア
	_cmdList->Reset(_cmdAllocator.Get(), nullptr);	// 再びコマンドリストをためる準備

	_swapchain->Present(1, 0);	// バッファをフリップ

#pragma endregion

}

void DirectXCommon::ClearRenderTarget()
{
	bbIdx = _swapchain->GetCurrentBackBufferIndex();	// 現在のバックバッファのインデックス

	rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);	// 画面クリア

	_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);	// 深度クリア

	_cmdList->SetPipelineState(_piplineState.Get());	// パイプラインステートのセット

	_cmdList->RSSetViewports(1, &viewport);

	_cmdList->RSSetScissorRects(1, &scissorrect);

	_cmdList->SetGraphicsRootSignature(rootsignature.Get());		// ルートシグネチャの設定

	_cmdList->SetDescriptorHeaps(1, basicDescHeap.GetAddressOf());	// ディスクリプターヒープの指定

	auto heapHandle = basicDescHeap->GetGPUDescriptorHandleForHeapStart();

	_cmdList->SetGraphicsRootDescriptorTable(0, heapHandle);	// ルートパラメーターとディスクリプターヒープの関連付け

	_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// プリミティブ形状の設定コマンド

	_cmdList->IASetVertexBuffers(0, 1, &vbView);	// 頂点バッファの設定コマンド

	_cmdList->IASetIndexBuffer(&ibView);	// インデックスバッファの設定コマンド

	_cmdList->DrawIndexedInstanced(vertNum, 1, 0, 0, 0);	// 描画コマンド

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

	result = CreateDXGIFactory1(IID_PPV_ARGS(_dxgiFactory.GetAddressOf()));	// DXGIFactoryを_dxgiFactoryに入れる
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
		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(_dev.GetAddressOf())) == S_OK)
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
	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_cmdAllocator.GetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	_cmdList = nullptr;
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator.Get(), nullptr, IID_PPV_ARGS(_cmdList.GetAddressOf()));
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
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(_cmdQueue.GetAddressOf()));		// キュー生成
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
		_cmdQueue.Get(),
		winApp->GetHwnd(),
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)_swapchain.GetAddressOf()
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
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;						// 裏表で2枚
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// 特に指定なし

	rtvHeaps = nullptr;
	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(rtvHeaps.GetAddressOf()));
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

	// ガンマ補正用の設定(SRGB)
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// ガンマ補正あり
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

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
		_dev->CreateRenderTargetView(_backBuffers[i], &rtvDesc, handle);			// RTVの生成
	}

	return true;
}

bool DirectXCommon::CreateFence()
{
	// フェンスの生成
	_fence = nullptr;
	result = _dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_fence.GetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	return true;
}

bool DirectXCommon::GenerateVertexBuffer()
{
	// ヒープ設定
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	// リソース設定

	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = sizeof(vertices);	// 頂点情報のサイズ
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.Format = DXGI_FORMAT_UNKNOWN;
	resdesc.SampleDesc.Count = 1;
	resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


	result = _dev->CreateCommittedResource
	(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertBuff.GetAddressOf())
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	return true;
}

bool DirectXCommon::MapVertexBuffer()
{
	// 法線の計算
	for (int i = 0; i < indicesNum / 3; i++)
	{
		// 三角形のインデックスを一時的な変数に入れる
		unsigned short index0 = indices[i * 3 + 0];
		unsigned short index1 = indices[i * 3 + 1];
		unsigned short index2 = indices[i * 3 + 2];
		// 三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);
		// ベクトルを計算
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		// 2つのベクトルの外積
		XMVECTOR normal = XMVector3Cross(v1, v2);
		// 正規化
		normal = XMVector3Normalize(normal);
		// 法線を代入
		XMStoreFloat3(&vertices[index0].normal, normal);
		XMStoreFloat3(&vertices[index1].normal, normal);
		XMStoreFloat3(&vertices[index2].normal, normal);
	}

	// 頂点情報のマップ
	Vertex* vertMap = nullptr;

	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	std::copy(std::begin(vertices), std::end(vertices), vertMap);

	vertBuff->Unmap(0, nullptr);

	return true;
}

void DirectXCommon::CreateVertexBufferView()
{
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();	// バッファの仮想アドレス
	vbView.SizeInBytes = sizeof(vertices);		// 全バイト数
	vbView.StrideInBytes = sizeof(vertices[0]);	// 1頂点のバイト数
}

bool DirectXCommon::GenerateIndexBuffer()
{
	// バッファのサイズ以外の設定を使いまわす
	resdesc.Width = sizeof(indices);

	result = _dev->CreateCommittedResource
	(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(idxBuff.GetAddressOf())
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	return true;
}

bool DirectXCommon::MapIndexBuffer()
{
	unsigned short* mappedIdx = nullptr;
	result = idxBuff->Map(0, nullptr, (void**)&mappedIdx);
	if (FAILED(result)) {
		assert(0);
		return result;
	}
	std::copy(std::begin(indices), std::end(indices), mappedIdx);
	idxBuff->Unmap(0, nullptr);

	return true;
}

void DirectXCommon::CreateIndexBufferView()
{
	ibView.BufferLocation = idxBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indices);
}

bool DirectXCommon::GenerateTextureBuffer()
{
	//WICテクスチャのロード
	DirectX::ScratchImage scrachImg = {};

	result = LoadFromWICFile
	(
		L"Resources/texture/colorGrid.png",
		WIC_FLAGS_NONE,
		&metadata,
		scrachImg
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	auto img = scrachImg.GetImage(0, 0, 0);	// 生データ抽出

	// ヒープ設定
	D3D12_HEAP_PROPERTIES texHeapProp = {};

	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;	// ライトバック
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;	// 転送はCPUから直接行う
	texHeapProp.CreationNodeMask = 0;	// 単一アダプターのため0
	texHeapProp.VisibleNodeMask = 0;

	// リソース設定
	D3D12_RESOURCE_DESC texResDesc = {};

	texResDesc.Format = metadata.format;
	texResDesc.Width = metadata.width;		// 幅
	texResDesc.Height = metadata.height;	// 高さ
	texResDesc.DepthOrArraySize = metadata.arraySize;	// 2D配列じゃないので1
	texResDesc.SampleDesc.Count = 1;	// アンチエイリアシングしない
	texResDesc.SampleDesc.Quality = 0;	// 最低クオリティ
	texResDesc.MipLevels = metadata.mipLevels;	// ミップマップしないので1
	texResDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);	// 2Dテクスチャ用
	texResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	// レイアウトは決定しない
	texResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;	// 特にフラグなし

	// リソースの生成

	result = _dev->CreateCommittedResource
	(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texResDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,		// テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(texbuff.GetAddressOf())
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// データ転送
	result = texbuff->WriteToSubresource
	(
		0,
		nullptr,		// 全領域にコピー
		img->pixels,	// 元データアドレス
		img->rowPitch,	// 1ラインサイズ
		img->slicePitch	// 全サイズ
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	return true;
}

bool DirectXCommon::GenerateConstBufferView()
{
	CalculateMat();

	// ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp = {};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	// 転送用

	// リソース設定
	D3D12_RESOURCE_DESC cbResDesc = {};
	cbResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResDesc.Width = (sizeof(ConstBfferData) + 0xff) & ~0xff;
	cbResDesc.Height = 1;
	cbResDesc.DepthOrArraySize = 1;
	cbResDesc.MipLevels = 1;
	cbResDesc.SampleDesc.Count = 1;
	cbResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 定数バッファ作成
	result = _dev->CreateCommittedResource
	(
		&cbHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&cbResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuff.GetAddressOf())
	);
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// マップ
	ConstBfferData* constMap;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->matrix = matWorld * matView * matProjection;
	constMap->color = XMFLOAT4(1, 1, 1, 0);
	if (FAILED(result)) {
		assert(0);
		return false;
	}
	
	return true;
}

void DirectXCommon::CalculateMat()
{
	matWorld = XMMatrixIdentity();
	matView = XMMatrixIdentity();
	matProjection = XMMatrixIdentity();

	// ワールド行列の計算
	XMFLOAT3 scale = { 1.0f,1.0f,1.0f };
	XMFLOAT3 rotation = { -30.0f,45.0f,0.0f };
	XMFLOAT3 position = { 0.0f,0.0f,0.0f };

	XMMATRIX matScale = XMMatrixIdentity();
	XMMATRIX matRot = XMMatrixIdentity();
	XMMATRIX matTrans = XMMatrixIdentity();

	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);

	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));

	matWorld *= matScale;
	matWorld *= matRot;
	matWorld *= matTrans;

	XMFLOAT3 eye(0, 0, -5);
	XMFLOAT3 target(0, 0, 0);
	XMFLOAT3 up(0, 1, 0);

	// 平行移動行列の計算
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	// プロジェクション行列の計算
	matProjection = XMMatrixPerspectiveFovLH
	(
		XM_PIDIV4,	// 画角
		static_cast<float>(WinApp::WINDOW_WIDTH) / static_cast<float>(WinApp::WINDOW_HEIGHT),	// アスペクト比
		1.0f,		// 近いほう
		10.0f		// 遠いほう
	);
}

bool DirectXCommon::CreateTextureShaderResourceView()
{
	// デスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};

	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// シェーダーから見えるように
	descHeapDesc.NodeMask = 0;	// マスク0
	descHeapDesc.NumDescriptors = 2;	// SRVとCBV
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;	// シェーダーリソースビュー用

	result = _dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(basicDescHeap.GetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// シェーダーリソースビューを作る
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.Format = metadata.format;	// RGBA
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;	// ミップマップは使わないので1

	// ディスクリプタの先頭ハンドルを先に取得しておく
	auto basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	// シェーダーリソースビューの生成
	_dev->CreateShaderResourceView
	(
		texbuff.Get(),	// ビューと関連付けるバッファ
		&srvDesc,		// テクスチャ設定情報
		basicHeapHandle	// ヒープのどこに割り当てるか
	);

	///////////////////////////////////////////////////////////
	// 後で分けるけどとりあえずここで定数バッファビュー生成
	///////////////////////////////////////////////////////////

	basicHeapHandle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);	// ビューの大きさ分だけ後ろにずらす

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};

	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = constBuff->GetDesc().Width;

	// 定数バッファビューの生成
	_dev->CreateConstantBufferView
	(
		&cbvDesc,
		basicHeapHandle
	);

	return true;
}

bool DirectXCommon::LoadShader()
{

	result = D3DCompileFromFile
	(
		L"BasicVS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VSmain",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		vsBlob.GetAddressOf(),
		errorBlob.GetAddressOf()
	);
	if (FAILED(result))
	{
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());

		errstr += "\n";

		// エラー内容をウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		return result;
		exit(1);
	}

	result = D3DCompileFromFile
	(
		L"BasicPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PSmain",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		psBlob.GetAddressOf(),
		errorBlob.GetAddressOf()
	);
	if (FAILED(result))
	{
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());

		errstr += "\n";

		// エラー内容をウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		return result;
		exit(1);
	}


	return true;
}

bool DirectXCommon::CreateGPipelineStateObject()
{
	// 頂点レイアウトの設定
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		// 座標
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		// 法線
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		// uv
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};

	// シェーダーのセット
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipline = {};

	gPipline.pRootSignature = nullptr;

	gPipline.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	gPipline.VS.BytecodeLength = vsBlob->GetBufferSize();
	gPipline.PS.pShaderBytecode = psBlob->GetBufferPointer();
	gPipline.PS.BytecodeLength = psBlob->GetBufferSize();

	// サンプルマスクとラスタライザ―ステートの設定
	gPipline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// まだアンチエイリアスを使わないのでfalse
	gPipline.RasterizerState.MultisampleEnable = false;
	gPipline.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;	// 背面カリング
	//gPipline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	// カリングしない
	gPipline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	// 中身を塗りつぶす
	gPipline.RasterizerState.DepthClipEnable = true;			// 深度方向のクリッピング

	// 深度
	gPipline.DepthStencilState.DepthEnable = true;	// 深度バッファ有効
	gPipline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	// 書き込む
	gPipline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;	// 小さいほうをs採用

	// ブレンドステートの設定
	gPipline.BlendState.AlphaToCoverageEnable = false;
	gPipline.BlendState.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	renderTargetBlendDesc.BlendEnable = false;
	renderTargetBlendDesc.LogicOpEnable = false;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	gPipline.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	// 入力レイアウトの設定
	gPipline.InputLayout.pInputElementDescs = inputLayout;		// レイアウト先頭アドレス
	gPipline.InputLayout.NumElements = _countof(inputLayout);	// レイアウト配列の要素数
	gPipline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;		// カット無し
	gPipline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	// 三角形で構成

	// レンダーターゲットの設定
	gPipline.NumRenderTargets = 1;		// 今は1つのみ
	gPipline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;	// 0~1に正規化されたRGBA

	// アンチエイリアシングのためのサンプル数設定
	gPipline.SampleDesc.Count = 1;	// サンプリングは1ピクセル1回
	gPipline.SampleDesc.Quality = 0;	// 最低クオリティ


	SetUpRootParameter();

	SetUpSampler();

	CreatRootSignature();

	gPipline.pRootSignature = rootsignature.Get();

	// グラフィックパイプラインステートオブジェクトの生成
	result = _dev->CreateGraphicsPipelineState(&gPipline, IID_PPV_ARGS(_piplineState.GetAddressOf()));

	if (FAILED(result)) {
		assert(0);
		return result;
	}

	return true;
}

void DirectXCommon::SetUpRootParameter()
{
	// デスクリプターレンジの設定

	// テクスチャ用
	descTblRange[0].NumDescriptors = 1;								// テクスチャ1つ
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// 種別はテクスチャ
	descTblRange[0].BaseShaderRegister = 0;							// 0番スロットから
	descTblRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// 定数用
	descTblRange[1].NumDescriptors = 1;								// 定数1つ
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	// 種別はテクスチャ
	descTblRange[1].BaseShaderRegister = 0;							// 0番スロットから
	descTblRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// テクスチャ・定数共用
	rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;			// ピクセルシェーダーから見える
	rootparam.DescriptorTable.pDescriptorRanges = descTblRange;			// デスクリプタレンジのアドレス
	rootparam.DescriptorTable.NumDescriptorRanges = 2;					// デスクリプタレンジ数
}

void DirectXCommon::SetUpSampler()
{
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// 横方向の繰り返し
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// 縦方向の繰り返し
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// 奥行方向の繰り返し
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	// ボーダーは黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	// 線形補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;	// ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;				// ミップマップ最小値
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// ピクセルシェーダーから見える
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;	// サンプリングしない
}

bool DirectXCommon::CreatRootSignature()
{
	// D3D12_ROOT_SIGNAAATURE_DESCの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};

	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = &rootparam;				// ルートパラメーターの先頭アドレス
	rootSignatureDesc.NumParameters = 1;					// ルートパラメーターの数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	// バイナリコードの作成
	ID3D10Blob* rootSigBlob = nullptr;

	result = D3D12SerializeRootSignature
	(
		&rootSignatureDesc,				// ルートシグネチャ設定
		D3D_ROOT_SIGNATURE_VERSION_1_0,	// ルートシグネチャバージョン
		&rootSigBlob,
		&errorBlob
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// ルートシグネチャオブジェクトの作成
	result = _dev->CreateRootSignature
	(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(rootsignature.GetAddressOf())
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	rootSigBlob->Release();

	return true;
}

void DirectXCommon::SetUpViewport()
{
	// ビューポートの設定
	viewport.Width = WinApp::WINDOW_WIDTH;
	viewport.Height = WinApp::WINDOW_HEIGHT;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;
}

void DirectXCommon::SetUpScissorrect()
{
	// シザー矩形の設定
	// 切り抜き座標
	scissorrect.top = 0;
	scissorrect.left = 0;
	scissorrect.right = scissorrect.left + WinApp::WINDOW_WIDTH;
	scissorrect.bottom = scissorrect.top + WinApp::WINDOW_HEIGHT;
}
