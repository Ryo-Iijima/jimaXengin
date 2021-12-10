#include "Object3d.h"
#include "../WinApp.h"
#include "FbxLoader.h"
#include "../ResourceShader.h"

#include <d3dcompiler.h>
#pragma comment (lib,"d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace DirectX;

// static変数の実体
JimaXengine::DirectXCommon* JimaXengine::Object3d::dxCommon = nullptr;
ID3D12Device* JimaXengine::Object3d::_dev = nullptr;
ComPtr<ID3D12RootSignature> JimaXengine::Object3d::rootSignature;
ComPtr<ID3D12PipelineState> JimaXengine::Object3d::piplineState;
JimaXengine::Light* JimaXengine::Object3d::light=nullptr;

void  JimaXengine::Object3d::StaticInitialize(DirectXCommon* dxcommon, WinApp* winapp)
{
	Object3d::dxCommon = dxcommon;
}

void  JimaXengine::Object3d::CreateGraphicsPipline()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;    // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	assert(_dev);

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		// xy座標
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		// 法線ベクトル
		{
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		// uv座標
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		// 影響を受けるボーン番号
		{
			"BONEINDICES",0,DXGI_FORMAT_R32G32B32A32_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		// ボーンのスキンウェイト
		{
			"BONEWEIGHTS",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(ResourceShader::GetShaders("FBX/FBXVS.hlsl").Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(ResourceShader::GetShaders("FBX/FBXPS.hlsl").Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;    // RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.BlendState.RenderTarget[1] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 2;    // 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
	gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

	// ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[5];
	// CBV（座標変換行列用）
	rootparams[(int)ViewName::transform].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	// SRV（テクスチャ）
	rootparams[(int)ViewName::texture].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	// CBV（スキニング用）
	rootparams[(int)ViewName::skin].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
	// CBV（ライト）
	rootparams[(int)ViewName::light].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	// CBV（マテリアル）
	rootparams[(int)ViewName::material].InitAsConstantBufferView(4, 0, D3D12_SHADER_VISIBILITY_ALL);

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ルートシグネチャの生成
	result = _dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		assert(0); 
	}

	gpipeline.pRootSignature = rootSignature.Get();

	// グラフィックスパイプラインの生成
	result = _dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(piplineState.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{ 
		assert(0); 
	}
}

void JimaXengine::Object3d::Initialize()
{
	HRESULT result;
	// 定数バッファの生成
	result = _dev->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(TransformData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBufferTranceform.GetAddressOf())
	);
	assert(SUCCEEDED(result));

	result = _dev->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(SkinData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBufferSkin.GetAddressOf())
	);
	assert(SUCCEEDED(result));

	// スキン無し用
	SkinData* constMapSkin = nullptr;
	result = constBufferSkin->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < MAX_BONES; i++)
	{
		constMapSkin->bones[i] = XMMatrixIdentity();
	}
	constBufferSkin->Unmap(0, nullptr);
	assert(SUCCEEDED(result));

	eye = { 0, 0, -20 };
	target = { 0, 0, 0 };
	up = { 0, 1, 0 };

	// 1フレーム分の時間を60FPSに設定
	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);
}

void JimaXengine::Object3d::Update()
{
	////////////////////////////
	/// トランスフォームの転送
	////////////////////////////

	XMMATRIX matScale, matRot, matTrans;

	// 行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ワールド行列の合成
	matWorld = XMMatrixIdentity();
	matWorld *= matScale;
	matWorld *= matRot;
	matWorld *= matTrans;

	const auto matView = camera->GetMatView();
	const auto matProje = camera->GetMatProjection();

	const XMMATRIX matViewProjection = matView * matProje;
	const XMMATRIX modelTransform = model->GetModelTransform();
	const XMFLOAT3 cameraPos = camera->GetEye();

	HRESULT result;
	// 定数バッファに転送
	TransformData* constMap = nullptr;
	result = constBufferTranceform->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		constMap->color = color;
		constMap->viewproj = matViewProjection;
		//constMap->world = matWorld * modelTransform;
		constMap->world = matWorld;
		constMap->cameraPos = cameraPos;
		constBufferTranceform->Unmap(0, nullptr);
	}

	////////////////////////////
	/// スキンの転送
	////////////////////////////

	// アニメーション
	if (isPlay)
	{
		// 1フレーム進める
		currentTime += frameTime;
		// 最後まで再生したら先頭に戻す
		if (currentTime > endTime)
		{
			currentTime = startTime;
		}
	}
	else
	{
		PlayAnimation();
	}

	// ボーン配列
	std::vector<JimaXengine::Model::Bone>& bones = model->GetBonse();

	// 定数バッファへデータ転送
	SkinData* constMapSkin = nullptr;
	result = constBufferSkin->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < bones.size(); i++)
	{
		// 今の姿勢行列
		XMMATRIX matCurrentPos;
		// 今の姿勢行列を取得
		FbxAMatrix fbxCurrentPose = bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
		// XMMATRIXに変換
		FbxLoader::ConvertMatrixFromFbx(&matCurrentPos, fbxCurrentPose);
		// 合成してスキニング行列に
		constMapSkin->bones[i] = bones[i].invInitialPose * matCurrentPos;
	}
	constBufferSkin->Unmap(0, nullptr);
}

void JimaXengine::Object3d::Draw()
{
	// モデルがなければ描画しない
	if (model == nullptr)
	{
		return;
	}
	
	dxCommon->GetCommandList()->SetPipelineState(piplineState.Get());
	dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView((UINT)ViewName::transform, constBufferTranceform->GetGPUVirtualAddress());
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView((UINT)ViewName::skin, constBufferSkin->GetGPUVirtualAddress());
	//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView((UINT)ViewName::light, constBufferSkin->GetGPUVirtualAddress());
	light->Draw((int)ViewName::light);
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView((UINT)ViewName::material, model->GetCBMaterial().Get()->GetGPUVirtualAddress());	// 無理やり引っ張って来た感あっていびつ

	model->Draw(dxCommon->GetCommandList());
}

void JimaXengine::Object3d::SetModelforBuff(const std::string& modelName)
{
	model = FbxLoader::GetInstance().GetFbxModel(modelName);
}

void JimaXengine::Object3d::PlayAnimation()
{
	FbxScene* fbxScene = model->GetFbxScene();
	// 0番のアニメーション取得
	FbxAnimStack* animstack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	if (!animstack)
	{
		// アニメーションがなかったら返す
		return;
	}
	// アニメーションの名前取得
	const char* animstackname = animstack->GetName();
	// アニメーションの時間情報
	FbxTakeInfo* takeinfo = fbxScene->GetTakeInfo(animstackname);

	// 開始時間取得
	startTime = takeinfo->mLocalTimeSpan.GetStart();
	// 終了時間取得
	endTime = takeinfo->mLocalTimeSpan.GetStop();
	// 開始時間に合わせる
	currentTime = startTime;
	// 再生中状態にする
	isPlay = true;
}