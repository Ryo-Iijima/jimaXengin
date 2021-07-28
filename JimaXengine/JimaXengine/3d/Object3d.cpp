#include "Object3d.h"
#include <d3dcompiler.h>
#include "../WinApp.h"
#pragma comment (lib,"d3dcompiler.lib")

#include "../general/Input.h"
#include "FbxLoader.h"

#include "ResourceShader.h"
#include "../Texture.h"

using namespace std;

using namespace Microsoft::WRL;
using namespace DirectX;

// static変数の実体
ComPtr<ID3D12RootSignature> Object3d::rootsignature;
static std::map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipelines;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Object3d::basicDescHeap;
UINT Object3d::texNumber = 0;
std::map<std::string, UINT> Object3d::textureMap;

HRESULT Object3d::CreateDescriptorHeap()
{
	HRESULT result;

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = SRV_MAX_COUNT;
	result = Object3d::dxCommon->GetDevice()->CreateDescriptorHeap(
		&descHeapDesc,
		IID_PPV_ARGS(&basicDescHeap));

	return result;
}

HRESULT Object3d::CreateRootSignature()
{
	HRESULT result;
	ComPtr<ID3DBlob> errorBlob;

	//テクスチャサンプラーの設定
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	//デスクリプタレンジの設定
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV = {};
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	//ルートパラメータ(定数用、テクスチャ用)
	CD3DX12_ROOT_PARAMETER rootParams[2] = {};
	rootParams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init_1_0(
		_countof(rootParams),
		rootParams,
		1,
		&samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	result = D3DX12SerializeVersionedRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob);

	result = Object3d::dxCommon->GetDevice()->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootsignature));

	return result;

}

HRESULT Object3d::CreateGraphicsPipeline(const std::string& vsfilename, const std::string& psfilename, const std::string& registername)
{
	HRESULT result;

	//頂点シェーダーに渡すための頂点データ
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{	"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0	},
		{	"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0		},
		{	"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0		},
	};

	//ブレンド設定
	enum BlendType { NOBLEND, ALPHA, ADD, SUB, INVSRC };
	std::string blendname[5] = { "NOBLEND", "ALPHA", "ADD", "SUB", "INVSRC" };
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc = {};

	std::map<int, D3D12_RENDER_TARGET_BLEND_DESC>tmpmap;

	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blenddesc.BlendEnable = false;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_ONE;
	tmpmap.emplace(NOBLEND, blenddesc);

	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	tmpmap.emplace(ALPHA, blenddesc);

	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_ONE;
	tmpmap.emplace(ADD, blenddesc);

	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blenddesc.SrcBlend = D3D12_BLEND_ONE;
	blenddesc.DestBlend = D3D12_BLEND_ONE;
	tmpmap.emplace(SUB, blenddesc);

	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blenddesc.DestBlend = D3D12_BLEND_ZERO;
	tmpmap.emplace(INVSRC, blenddesc);

	//ステータス設定構造体
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipelineDef{};
	//shader
	gpipelineDef.VS = CD3DX12_SHADER_BYTECODE(ResourceShader::GetShaders(vsfilename).Get());
	gpipelineDef.PS = CD3DX12_SHADER_BYTECODE(ResourceShader::GetShaders(psfilename).Get());
	//sample mask
	gpipelineDef.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//rasterizer
	gpipelineDef.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//depth stencil
	gpipelineDef.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	gpipelineDef.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	//input layout
	gpipelineDef.InputLayout.pInputElementDescs = inputLayout;
	gpipelineDef.InputLayout.NumElements = _countof(inputLayout);
	//primitive type
	gpipelineDef.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//blend type
	gpipelineDef.BlendState.RenderTarget[0] = tmpmap[NOBLEND];
	//other
	gpipelineDef.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpipelineDef.NumRenderTargets = 1;
	gpipelineDef.SampleDesc.Count = 1;
	gpipelineDef.pRootSignature = rootsignature.Get();

	//カリングなし
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipelineCullNone = gpipelineDef;
	gpipelineCullNone.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	//ワイヤーフレーム
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipelineSolid = gpipelineDef;
	gpipelineSolid.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

	//深度テストなし
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipelineDepthNone = gpipelineDef;
	gpipelineDepthNone.DepthStencilState.DepthEnable = false;

	//デフォルト
	for (int i = 0; i < 5; i++)
	{
		ComPtr<ID3D12PipelineState> tmppipeline;
		gpipelineDef.BlendState.RenderTarget[0] = tmpmap[i];
		result = Object3d::dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipelineDef, IID_PPV_ARGS(&tmppipeline));
		pipelines.emplace(registername + blendname[i] + "DEFAULT", tmppipeline);
	}

	//カリングなし
	for (int i = 0; i < 5; i++)
	{
		ComPtr<ID3D12PipelineState> tmppipeline;
		gpipelineCullNone.BlendState.RenderTarget[0] = tmpmap[i];
		result = Object3d::dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipelineCullNone, IID_PPV_ARGS(&tmppipeline));
		pipelines.emplace(registername + blendname[i] + "CULLNONE", tmppipeline);
	}

	//ワイヤーフレーム
	for (int i = 0; i < 5; i++)
	{
		ComPtr<ID3D12PipelineState> tmppipeline;
		gpipelineSolid.BlendState.RenderTarget[0] = tmpmap[i];
		result = Object3d::dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipelineSolid, IID_PPV_ARGS(&tmppipeline));
		pipelines.emplace(registername + blendname[i] + "WIREFRAME", tmppipeline);
	}

	//深度テストなし
	for (int i = 0; i < 5; i++)
	{
		ComPtr<ID3D12PipelineState> tmppipeline;
		gpipelineDepthNone.BlendState.RenderTarget[0] = tmpmap[i];
		result = Object3d::dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipelineDepthNone, IID_PPV_ARGS(&tmppipeline));
		pipelines.emplace(registername + blendname[i] + "DEPTHNONE", tmppipeline);
	}

	return result;
}

HRESULT Object3d::CreateConstBuffer()
{
	HRESULT result;

	result = Object3d::dxCommon->GetDevice()->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(TransformData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBufferTranceform.GetAddressOf())
	);

	result = Object3d::dxCommon->GetDevice()->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(SkinData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBufferSkin.GetAddressOf())
	);

	return result;
}


void Object3d::UpdateMatrix(Vector3 pos, Vector3 rot, Vector3 scale, XMMATRIX billboardMat, Vector3 localRot)
{
	//スケール
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);

	//回転(ワールド)
	matRotation = XMMatrixIdentity();
	matRotation *= XMMatrixRotationX(XMConvertToRadians(rot.x));
	matRotation *= XMMatrixRotationY(XMConvertToRadians(rot.y));
	matRotation *= XMMatrixRotationZ(XMConvertToRadians(rot.z));

	//移動
	matTrans = XMMatrixTranslation(pos.x, pos.y, pos.z);

	//回転(ローカル)
	XMVECTOR q;
	q = XMQuaternionRotationNormal(XMLoadFloat3(&GetLocalZ()), XMConvertToRadians(localRot.x));
	matRotation *= XMMatrixRotationQuaternion(q);
	q = XMQuaternionRotationNormal(XMLoadFloat3(&GetLocalZ()), XMConvertToRadians(localRot.y));
	matRotation *= XMMatrixRotationQuaternion(q);
	q = XMQuaternionRotationNormal(XMLoadFloat3(&GetLocalZ()), XMConvertToRadians(localRot.z));
	matRotation *= XMMatrixRotationQuaternion(q);

	//行列合成
	matWorld = XMMatrixIdentity();

	//ビルボード
	matWorld *= billboardMat;

	//各種パラメータ
	matWorld *= matScale;
	matWorld *= matRotation;
	matWorld *= matTrans;

	//親子構造かどうか
	if (parent != nullptr)
	{
		XMMATRIX tmpScale = XMMatrixIdentity();
		XMMATRIX tmpRot = XMMatrixIdentity();
		XMMATRIX tmpTrans = XMMatrixIdentity();
		XMMATRIX tmpWorld = XMMatrixIdentity();

		tmpScale = XMMatrixScaling(parent->scale.x, parent->scale.y, parent->scale.z);
		tmpRot *= XMMatrixRotationX(XMConvertToRadians(parent->rotation.x));
		tmpRot *= XMMatrixRotationY(XMConvertToRadians(parent->rotation.y));
		tmpRot *= XMMatrixRotationZ(XMConvertToRadians(parent->rotation.z));
		tmpTrans = XMMatrixTranslation(parent->position.x, parent->position.y, parent->position.z);

		tmpWorld *= tmpScale;
		tmpWorld *= tmpRot;
		tmpWorld *= tmpTrans;
		matWorld *= tmpWorld;
	}

	rotation = rot;
	this->scale = scale;
	position.x = matWorld.r[3].m128_f32[0];
	position.y = matWorld.r[3].m128_f32[1];
	position.z = matWorld.r[3].m128_f32[2];


	localY.x = matWorld.r[1].m128_f32[0];
	localY.y = matWorld.r[1].m128_f32[1];
	localY.z = matWorld.r[1].m128_f32[2];
	localY.Normalize();
}

HRESULT Object3d::TransferConstBuffer(Camera* camera, Vector4 color, Vector3 light)
{
	matWorldViewProj = matWorld * camera->GetMatView() * camera->GetMatProjection();

	TransformData* constMap = nullptr;
	constBufferTranceform->Map(0, nullptr, (void**)&constMap);
	constMap->viewproj = matWorldViewProj;
	//constMap->world = matWorld * modelTransform;
	constMap->world = matWorld;
	constMap->cameraPos = camera->GetEye();
	constBufferTranceform->Unmap(0, nullptr);
	
	// ボーン配列
	std::vector<Model::Bone>& bones = model->GetBonse();

	// 定数バッファへデータ転送
	SkinData* constMapSkin = nullptr;
	constBufferSkin->Map(0, nullptr, (void**)&constMapSkin);
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

	return S_OK;
}

void Object3d::DrawCommands(const std::string& modelname, const std::string& filename, const std::string& shadername)
{
	dxCommon->GetCommandList()->SetGraphicsRootSignature(rootsignature.Get());
	dxCommon->GetCommandList()->SetPipelineState(pipelines[shadername].Get());
	dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D12DescriptorHeap* ppHeaps[] = { basicDescHeap.Get() };
	dxCommon->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(
		1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			basicDescHeap->GetGPUDescriptorHandleForHeapStart(),
			textureMap[filename],
			dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	Model* mesh = ModelManager::GetObjMesh(modelname);
	dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &mesh->vbView);
	dxCommon->GetCommandList()->IASetIndexBuffer(&mesh->ibView);
	dxCommon->GetCommandList()->DrawIndexedInstanced((UINT)mesh->indices.size(), 1, 0, 0, 0);
}

Object3d::Object3d()
{
}

Object3d::~Object3d()
{
}

void Object3d::Initialize(WinApp* winapp, DirectXCommon* dxcommon)
{
	Object3d::winApp = winapp;
	Object3d::dxCommon = dxcommon;

	CreateDescriptorHeap();
	CreateRootSignature();
	CreateGraphicsPipeline("BasicVertexShader.hlsl", "BasicPixelShader.hlsl", "Basic3D");
	CreateGraphicsPipeline("BasicVertexShader.hlsl", "NonLightPixelShader.hlsl", "NonLight3D");

#ifdef _DEBUG
	printf("object3d initialized\n");
#endif // _DEBUG

	// 1フレーム分の時間を60FPSに設定
	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);
}

void Object3d::LoadResourceTexture(const std::string& filename)
{
	//シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	switch (Texture::GetTextureType(filename))
	{
	case Texture::TextureType::RESOURCE:
		srvDesc.Format = Texture::GetMetadata(filename).format;
		break;
	case Texture::TextureType::CREATE:
		srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	}

	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	Object3d::dxCommon->GetDevice()->CreateShaderResourceView(
		Texture::GetTexture(filename).Get(),
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			basicDescHeap->GetCPUDescriptorHandleForHeapStart(),
			texNumber,
			dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	textureMap.emplace(filename, texNumber);
	texNumber++;
}

void Object3d::CreateMesh()
{
	CreateConstBuffer();
}

void Object3d::SetParent(Object3d* parent)
{
	this->parent = parent;
}


void Object3d::Update(Vector3 pos, Vector3 rot, Vector3 scale, XMMATRIX billboardMat, Vector3 localRot)
{
	UpdateMatrix(pos, rot, scale, billboardMat, localRot);

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

}

void Object3d::Draw(Camera* camera, const std::string& modelname, const std::string& filename, const std::string& registername, const std::string& blendtype, const std::string& pipelinecustom, Vector4 color, Vector3 light)
{
	this->camera = camera;
	TransferConstBuffer(this->camera, color, light);
	DrawCommands(modelname, filename, (registername + blendtype + pipelinecustom));
}

Vector3 Object3d::GetLocalX()
{
	XMMATRIX tmpmat = XMMatrixIdentity();

	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);

	matRotation = XMMatrixIdentity();
	matRotation *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRotation *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matRotation *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));

	tmpmat *= matScale;
	tmpmat *= matRotation;

	Vector3 vector;
	vector.x = tmpmat.r[0].m128_f32[0];
	vector.y = tmpmat.r[0].m128_f32[1];
	vector.z = tmpmat.r[0].m128_f32[2];

	return vector.Normalize();
}

Vector3 Object3d::GetLocalY()
{
	XMMATRIX tmpmat = XMMatrixIdentity();

	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);

	matRotation = XMMatrixIdentity();
	matRotation *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRotation *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matRotation *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));

	tmpmat *= matScale;
	tmpmat *= matRotation;

	Vector3 vector;
	vector.x = tmpmat.r[1].m128_f32[0];
	vector.y = tmpmat.r[1].m128_f32[1];
	vector.z = tmpmat.r[1].m128_f32[2];
	vector.Normalize();

	return vector;
}

Vector3 Object3d::GetLocalZ()
{
	XMMATRIX tmpmat = XMMatrixIdentity();

	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);

	matRotation = XMMatrixIdentity();
	matRotation *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRotation *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matRotation *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));

	tmpmat *= matScale;
	tmpmat *= matRotation;

	Vector3 vector;
	vector.x = tmpmat.r[2].m128_f32[0];
	vector.y = tmpmat.r[2].m128_f32[1];
	vector.z = tmpmat.r[2].m128_f32[2];

	return vector.Normalize();
}


void Object3d::PlayAnimation()
{
	FbxScene* fbxScene = model->GetFbxScene();
	// 0番のアニメーション取得
	FbxAnimStack* animstack = fbxScene->GetSrcObject<FbxAnimStack>(0);
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

void Object3d::CameraMove()
{
	//Vector2 currentMousePos = input->GetCurrentMousePos();
	//printf("mousePos:%f \n", currentMousePos.x);
	float moveAcc = 1;
	// 左クリックホールドで
	if (input->MouseButtonPress(0))
	{
		// ALT押しながら動かすとtarget固定でeyeを動かす
		if (input->KeyPress(DIK_LMENU))
		{
			// 右
			if (input->GetPrevMousePos().x < input->GetCurrentMousePos().x)
			{
				eye.x += moveAcc;
			}
			// 左
			if (input->GetPrevMousePos().x > input->GetCurrentMousePos().x)
			{
				eye.x -= moveAcc;
			}

			// 上
			if (input->GetPrevMousePos().y < input->GetCurrentMousePos().y)
			{
				eye.y += moveAcc;
			}
			// 下
			if (input->GetPrevMousePos().y > input->GetCurrentMousePos().y)
			{
				eye.y -= moveAcc;
			}
		}
		// eye固定でtargetを動かす
		else
		{
			// 右
			if (input->GetPrevMousePos().x < input->GetCurrentMousePos().x)
			{
				target.x += moveAcc;
			}
			// 左
			if (input->GetPrevMousePos().x > input->GetCurrentMousePos().x)
			{
				target.x -= moveAcc;
			}

			// 上
			if (input->GetPrevMousePos().y < input->GetCurrentMousePos().y)
			{
				target.y += moveAcc;
			}
			// 下
			if (input->GetPrevMousePos().y > input->GetCurrentMousePos().y)
			{
				target.y -= moveAcc;
			}
		}
	}

	if (input->MouseWheelMove()<0)
	{
		target.z -= moveAcc;
		eye.z -= moveAcc;
	}
	else if (input->MouseWheelMove() > 0)
	{
		target.z += moveAcc;
		eye.z += moveAcc;
	}

	camera->SetViewMatrix(eye, target, up);
}
