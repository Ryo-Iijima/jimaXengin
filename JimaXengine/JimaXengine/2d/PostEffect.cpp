#include "PostEffect.h"
#include "../ResourceShader.h"
#include "Object2d.h"
#include "../general/Input.h"

using namespace Microsoft::WRL;
using namespace std;

JimaXengine::DirectXCommon* JimaXengine::PostEffect::dxCommon = nullptr;
ComPtr<ID3D12DescriptorHeap> JimaXengine::PostEffect::descHeapSRV = {};
ComPtr<ID3D12DescriptorHeap> JimaXengine::PostEffect::descHeapRTV = {};
ComPtr<ID3D12DescriptorHeap> JimaXengine::PostEffect::descHeapDSV = {};
const float JimaXengine::PostEffect::clearColor[4] = { 0.25f,0.5f,0.1f,1.0f };
std::map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> JimaXengine::PostEffect::pipelines;
Microsoft::WRL::ComPtr<ID3D12RootSignature> JimaXengine::PostEffect::rootSignature;

void JimaXengine::PostEffect::DrawCommands(const std::string& filename, const std::string& registername, const std::string& blendtype)
{
	dxCommon->GetCommandList()->SetPipelineState(pipelines[registername + blendtype].Get());
	dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	PostEffect::dxCommon->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vBView);

	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeapSRV->GetGPUDescriptorHandleForHeapStart(),0,
			dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);

	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeapSRV->GetGPUDescriptorHandleForHeapStart(),1,
			dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);

	dxCommon->GetCommandList()->DrawInstanced(4, 1, 0, 0);
}

void JimaXengine::PostEffect::CreateRootSignature()
{
	HRESULT result;

	//ディスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV0 = {};
	descRangeSRV0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);	// t0レジスタ

	CD3DX12_DESCRIPTOR_RANGE descRangeSRV1 = {};
	descRangeSRV1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);	// t1レジスタ

	//ルートパラメータ(定数用、テクスチャ用)
	CD3DX12_ROOT_PARAMETER rootParams[3] = {};
	rootParams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParams[1].InitAsDescriptorTable(1, &descRangeSRV0, D3D12_SHADER_VISIBILITY_ALL);
	rootParams[2].InitAsDescriptorTable(1, &descRangeSRV1, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init_1_0(_countof(rootParams), rootParams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	ComPtr<ID3DBlob> errorBlob;

	result = D3DX12SerializeVersionedRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob);

	result = dxCommon->GetDevice()->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
}

void JimaXengine::PostEffect::GenerateTexture()
{
	HRESULT result;

	// テクスチャリソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		WinApp::WINDOW_WIDTH,
		WinApp::WINDOW_HEIGHT,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	for (int i = 0; i < 2; i++)
	{
		// テクスチャバッファの生成
		result = dxCommon->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor),
			IID_PPV_ARGS(&texBuff[i])
		);
		assert(SUCCEEDED(result));

		{// テクスチャを赤クリア
			// 画素数
			const UINT pixelCount = WinApp::WINDOW_WIDTH * WinApp::WINDOW_HEIGHT;
			// 画像１行分のデータサイズ
			const UINT rowPitch = sizeof(UINT) * WinApp::WINDOW_WIDTH;
			// 画像全体のデータサイズ
			const UINT depthPitch = rowPitch * WinApp::WINDOW_HEIGHT;
			// 画像イメージ
			UINT* img = new UINT[pixelCount];
			for (int j = 0; j < pixelCount; j++) { img[j] = 0xff0000ff; }

			// テクスチャバッファにデータ転送
			result = texBuff[i]->WriteToSubresource(0, nullptr, img, rowPitch, depthPitch);
			assert(SUCCEEDED(result));
			delete[] img;
		}
	}

}

void JimaXengine::PostEffect::CreateSRV()
{
	HRESULT result;

	// SRV用デスクリプタ設定
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 2;
	result = dxCommon->GetDevice()->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV));
	// SRV用デスクリプタヒープを生成
	result = dxCommon->GetDevice()->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV));
	assert(SUCCEEDED(result));
	//シェーダーリソースビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < 2; i++)
	{
		dxCommon->GetDevice()->CreateShaderResourceView(
			texBuff[i].Get(),
			&srvDesc,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeapSRV->GetCPUDescriptorHandleForHeapStart(),
				i,
				dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV))	
		);
	}
}

void JimaXengine::PostEffect::CreateRTV()
{
	HRESULT result;

	// RTV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHeapDesc.NumDescriptors = 2;
	// RTV用デスクリプタヒープを生成
	result = dxCommon->GetDevice()->CreateDescriptorHeap(&rtvDescHeapDesc, IID_PPV_ARGS(&descHeapRTV));
	assert(SUCCEEDED(result));

	for (int i = 0; i < 2; i++)
	{
		// デスクリプタヒープにRTV作成
		dxCommon->GetDevice()->CreateRenderTargetView(
			texBuff[i].Get(),
			nullptr,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeapRTV->GetCPUDescriptorHandleForHeapStart(),
				i,
				dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV))
		);
	}
}

void JimaXengine::PostEffect::GenerateDepthBuffer()
{
	HRESULT result;

	// 深度バッファリソース設定
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		WinApp::WINDOW_WIDTH,
		WinApp::WINDOW_HEIGHT,
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);
	// 深度バッファの生成
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer)
	);
	assert(SUCCEEDED(result));
}

void JimaXengine::PostEffect::CreateDSV()
{
	HRESULT result;

	// DSV用デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC dsvDescHeapDesc{};
	dsvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescHeapDesc.NumDescriptors = 1;
	// DSV用デスクリプタヒープを作成
	result = dxCommon->GetDevice()->CreateDescriptorHeap(&dsvDescHeapDesc, IID_PPV_ARGS(&descHeapDSV));
	assert(SUCCEEDED(result));
	// デスクリプタヒープにDSV作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dxCommon->GetDevice()->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		descHeapDSV->GetCPUDescriptorHandleForHeapStart()
	);
}

void JimaXengine::PostEffect::CreateGraphicsPiplineState(const std::string& vsfilename, const std::string& psfilename, const std::string& registername)
{
	HRESULT result;

	//頂点レイアウト配列の宣言
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};

	//パイプラインステート
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(ResourceShader::GetShaders(vsfilename).Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(ResourceShader::GetShaders(psfilename).Get());
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpipeline.SampleDesc.Count = 1;
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DepthStencilState.DepthEnable = false; //2Dのスプライト画像は深度テストをしないだけ(コードの順番で描画されていく)
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	gpipeline.pRootSignature = rootSignature.Get();

	//ブレンドモード
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc = {};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//-----ノーブレンド-----
	ComPtr<ID3D12PipelineState> noblend;
	blenddesc.BlendEnable = false;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; //加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; //ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; //デストの値を0%使う
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.BlendState.RenderTarget[1] = blenddesc;
	result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&noblend));
	assert(SUCCEEDED(result));

	pipelines.emplace(registername + "NOBLEND", noblend);

	//-----ワイヤーフレーム-----
	ComPtr<ID3D12PipelineState> wireframe;
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.BlendState.RenderTarget[1] = blenddesc;
	result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&wireframe));
	assert(SUCCEEDED(result));
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	blenddesc.BlendEnable = true;

	pipelines.emplace(registername + "WIREFRAME", wireframe);

	//-----加算-----
	ComPtr<ID3D12PipelineState> add;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA; //ソースの値を100%使う
	blenddesc.DestBlend = D3D12_BLEND_ONE; //デストの値を100%使う
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.BlendState.RenderTarget[1] = blenddesc;
	result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&add));
	assert(SUCCEEDED(result));

	pipelines.emplace(registername + "ADD", add);

	//-----減算-----
	ComPtr<ID3D12PipelineState> sub;
	blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT; //デストからソースを減算
	blenddesc.SrcBlend = D3D12_BLEND_ONE; //ソースの値を100%使う
	blenddesc.DestBlend = D3D12_BLEND_ONE; //デストの値を100%使う
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.BlendState.RenderTarget[1] = blenddesc;
	result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&sub));
	assert(SUCCEEDED(result));

	pipelines.emplace(registername + "SUB", sub);

	//-----反転-----
	ComPtr<ID3D12PipelineState> invsrc;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //加算
	blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR; //1.0f-デストカラー値
	blenddesc.DestBlend = D3D12_BLEND_ZERO; //使わない
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.BlendState.RenderTarget[1] = blenddesc;
	result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&invsrc));
	assert(SUCCEEDED(result));

	pipelines.emplace(registername + "INVSRC", invsrc);

	//-----半透明-----
	ComPtr<ID3D12PipelineState> alpha;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA; //ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA; //1.0f-ソースのアルファ値
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.BlendState.RenderTarget[1] = blenddesc;
	result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&alpha));
	assert(SUCCEEDED(result));

	pipelines.emplace(registername + "ALPHA", alpha);

	ComPtr<ID3D12PipelineState> depthalpha;
	gpipeline.DepthStencilState.DepthEnable = true;
	dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&depthalpha));
	pipelines.emplace(registername + "ALPHADEPTH", depthalpha);

}

JimaXengine::PostEffect::PostEffect()
{
	filename = "white1x1.png";
	blendtype = "NOBLEND";
	color = Vector4(1, 1, 1, 1);
}

void JimaXengine::PostEffect::Initialize(DirectXCommon* dxcommon)
{
	dxCommon = dxcommon;
	
	HRESULT result;

	// 頂点データ
	JimaXengine::Object2d::SpriteVertex vertices[4] = {
		{{-1.0f,-1.0f,0.0f},{ 0.0f, 1.0f }},
		{{-1.0f,+1.0f,0.0f},{ 0.0f, 0.0f }},
		{{+1.0f,-1.0f,0.0f},{ 1.0f, 1.0f }},
		{{+1.0f,+1.0f,0.0f},{ 1.0f, 0.0f }},
	};

	//頂点バッファ生成
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(JimaXengine::Object2d::SpriteVertex) * 4),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));


	// 頂点バッファへのデータ転送
	JimaXengine::Object2d::SpriteVertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}
	// 頂点バッファビューの作成
	vBView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vBView.SizeInBytes = sizeof(JimaXengine::Object2d::SpriteVertex) * 4;
	vBView.StrideInBytes = sizeof(JimaXengine::Object2d::SpriteVertex);

	// 定数バッファの生成
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(JimaXengine::Object2d::SpriteConstBufferData) + 0xff) & 0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);
	assert(SUCCEEDED(result));

	GenerateTexture();

	CreateSRV();

	CreateRTV();

	GenerateDepthBuffer();

	CreateDSV();

	CreateRootSignature();

	CreateGraphicsPiplineState("PostEffectTest/PostEffectTestVS.hlsl", "PostEffectTest/PostEffectTestPS.hlsl", "Basic2D");
}

void JimaXengine::PostEffect::Draw()
{	
	//if (Input::KeyTrigger(DIK_0))
	//{
	//	// デスクリプタヒープにSRVを作成
	//	static int tex = 0;
	//	// テクスチャ番号を0と1で切り替え
	//	tex = (tex + 1) % 2;

	//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	//	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//	srvDesc.Texture2D.MipLevels = 1;

	//	dxCommon->GetDevice()->CreateShaderResourceView(
	//		texBuff[tex].Get(),
	//		&srvDesc,
	//		descHeapSRV->GetCPUDescriptorHandleForHeapStart()
	//	);
	//}

	//定数バッファにデータ転送
	JimaXengine::Object2d::SpriteConstBufferData* constMap = nullptr;
	constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = color;
	constMap->matWP = XMMatrixIdentity();
	constBuff->Unmap(0, nullptr);

	DrawCommands(filename, "Basic2D", blendtype);
}

void JimaXengine::PostEffect::PreDrawScene()
{
	for (int i = 0; i < 2; i++)
	{
		// リソースバリアを変更（シェーダーリソース→描画可能）
		dxCommon->GetCommandList()->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(texBuff[i].Get(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				D3D12_RESOURCE_STATE_RENDER_TARGET)
		);
	}

	// レンダーターゲットビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHs[2]; 
	for (int i = 0; i < 2; i++)
	{
		rtvHs[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeapRTV->GetCPUDescriptorHandleForHeapStart(),
			i,
			dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		);
	}

	// 深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = descHeapDSV->GetCPUDescriptorHandleForHeapStart();
	// レンダーターゲットをセット
	dxCommon->GetCommandList()->OMSetRenderTargets(2, rtvHs, false, &dsvH);

	CD3DX12_VIEWPORT viewports[2];
	CD3DX12_RECT scissorRects[2];
	for (int i = 0; i < 2; i++)
	{
		viewports[i] = CD3DX12_VIEWPORT(0.0f, 0.0f, WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT);
		scissorRects[i] = CD3DX12_RECT(0.0f, 0.0f, WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT);
	}
	// ビューポートの設定
	dxCommon->GetCommandList()->RSSetViewports(2, viewports);
	// シザリング矩形の設定
	dxCommon->GetCommandList()->RSSetScissorRects(2, scissorRects);

	for (int i = 0; i < 2; i++)
	{
		// 全画面クリア
		dxCommon->GetCommandList()->ClearRenderTargetView(rtvHs[i], clearColor, 0, nullptr);
	}
	// 深度バッファのクリア
	dxCommon->GetCommandList()->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void JimaXengine::PostEffect::PostDrawScene()
{
	for (int i = 0; i < 2; i++)
	{
		// リソースバリアを変更（描画可能→シェーダーリソース）
		dxCommon->GetCommandList()->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(texBuff[i].Get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
		);
	}
}
