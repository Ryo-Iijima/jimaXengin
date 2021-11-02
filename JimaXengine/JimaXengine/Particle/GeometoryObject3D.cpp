#include "GeometoryObject3D.h"
#include <DirectXTex.h>
#include "../ResourceShader.h"
#include "../2d/Texture.h"
#include "../general/General.h"

JimaXengine::DirectXCommon* JimaXengine::GeometoryObject3D::dxcommon = nullptr;
ComPtr<ID3D12DescriptorHeap> JimaXengine::GeometoryObject3D::descHeap;
ComPtr<ID3D12RootSignature> JimaXengine::GeometoryObject3D::rootsignature;
std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> JimaXengine::GeometoryObject3D::pipelines;
std::map<std::string, int> JimaXengine::GeometoryObject3D::textureMap;
int JimaXengine::GeometoryObject3D::texNumber = 0;

bool JimaXengine::GeometoryObject3D::PreInitialize(DirectXCommon* dxcommon)
{
	GeometoryObject3D::dxcommon = dxcommon;

	InitializeDescriptorHeap();
	InitializeRootSignature();

	pipelines.emplace("quad_alpha", InitializeGraphicsPipeline(BlendType::ALPHA, "Geometory/ParticleVS.hlsl", "Geometory/QuadPS.hlsl", "Geometory/QuadGS.hlsl", false));
	pipelines.emplace("quad_add", InitializeGraphicsPipeline(BlendType::ADD, "Geometory/ParticleVS.hlsl", "Geometory/QuadPS.hlsl", "Geometory/QuadGS.hlsl", false));
	pipelines.emplace("cube_alpha", InitializeGraphicsPipeline(BlendType::ALPHA, "Geometory/ParticleVS.hlsl", "Geometory/CubePS.hlsl", "Geometory/CubeGS.hlsl", true));
	pipelines.emplace("cube_add", InitializeGraphicsPipeline(BlendType::ADD, "Geometory/ParticleVS.hlsl", "Geometory/CubePS.hlsl", "Geometory/CubeGS.hlsl", true));

	return true;
}

void JimaXengine::GeometoryObject3D::LoadTexture(const std::string& filename)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_RESOURCE_DESC resDesc = Texture::GetTexture(filename)->GetDesc();
	switch (Texture::GetTextureType(filename))
	{
	case Texture::TextureType::RESOURCE:
		srvDesc.Format = Texture::GetMetadata(filename).format;
		break;
	case Texture::TextureType::CREATE:
		srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	}
	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	dxcommon->GetDevice()->CreateShaderResourceView(
		Texture::GetTexture(filename).Get(),
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeap->GetCPUDescriptorHandleForHeapStart(),
			texNumber,
			dxcommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
	
	textureMap.emplace(filename, texNumber);
	texNumber++;
}

void JimaXengine::GeometoryObject3D::Initialize()
{
	{
		dxcommon->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertBuff));

		VertexPos* vertMap = nullptr;
		HRESULT result = vertBuff->Map(0, nullptr, (void**)&vertMap);
		if (SUCCEEDED(result)) {
			memcpy(vertMap, vertices, sizeof(vertices));
			vertBuff->Unmap(0, nullptr);
		}

		vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
		vbView.SizeInBytes = sizeof(vertices);
		vbView.StrideInBytes = sizeof(vertices[0]);

		dxcommon->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuff));
	}
}

void JimaXengine::GeometoryObject3D::Update(std::forward_list<ParticleBase*>& particles)
{
	HRESULT result;

	VertexPos* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result))
	{
		auto end = particles.end();
		for (auto itr = particles.begin(); itr != end; itr++)
		{
			vertMap->pos = (*itr)->GetParticleParameter().pos;
			vertMap->scale = (*itr)->GetParticleParameter().scl.x;
			vertMap->rot = (*itr)->GetParticleParameter().rot;
			vertMap->color = { 
				(*itr)->GetParticleParameter().color.x,
				(*itr)->GetParticleParameter().color.y,
				(*itr)->GetParticleParameter().color.z,
				(*itr)->GetParticleParameter().alpha 
			};
			vertMap++;
		}
		vertBuff->Unmap(0, nullptr);
	}
}

void JimaXengine::GeometoryObject3D::Draw(Camera * camera, std::forward_list<ParticleBase*>& particles, const std::string& filename, const std::string& pipelinename, BILLBOARD billboardType)
{
	HRESULT result;

	// 定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->matV = camera->GetMatView();
	constMap->matP = camera->GetMatProjection();
	switch (billboardType)
	{
	case BILLBOARD::BILLBOARD_NONE:
		constMap->matBillboard = XMMatrixIdentity();
		break;
	case BILLBOARD::BILLBOARD_ALL:
		constMap->matBillboard = camera->GetMatBillboard();
		break;
	case BILLBOARD::BILLBOARD_Y:
		constMap->matBillboard = camera->GetMatBillboardY();
		break;
	default:
		constMap->matBillboard = XMMatrixIdentity();
		break;
	}
	constBuff->Unmap(0, nullptr);

	// 描画コマンド
	dxcommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	dxcommon->GetCommandList()->SetGraphicsRootSignature(rootsignature.Get());
	dxcommon->GetCommandList()->SetPipelineState(pipelines[pipelinename].Get());

	dxcommon->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);

	dxcommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	dxcommon->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	dxcommon->GetCommandList()->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap->GetGPUDescriptorHandleForHeapStart(),
			textureMap[filename],
			dxcommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	dxcommon->GetCommandList()->DrawInstanced((UINT)std::distance(particles.begin(), particles.end()), 1, 0, 0);
}

HRESULT JimaXengine::GeometoryObject3D::InitializeDescriptorHeap()
{
	HRESULT result = S_FALSE;

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = SRV_MAX_COUNT;
	result = dxcommon->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));
	if (FAILED(result)) assert(0);

	return result;
}

HRESULT JimaXengine::GeometoryObject3D::InitializeRootSignature()
{
	HRESULT result = S_FALSE;

	// ルートシグネチャ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	CD3DX12_ROOT_PARAMETER rootparams[2] = {};
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ComPtr<ID3DBlob> rootSigBlob;
	ComPtr<ID3DBlob> errorBlob;
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	if (FAILED(result)) return result;
	result = dxcommon->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
	if (FAILED(result)) return result;

	return result;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> JimaXengine::GeometoryObject3D::InitializeGraphicsPipeline(BlendType type, const std::string& vsname, const std::string& psname, const std::string& gsname, bool depth)
{
	HRESULT result = S_FALSE;

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "ROTATION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// ブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	switch (type)
	{
	case GeometoryObject3D::BlendType::ALPHA:
		// アルファ
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		break;
	case GeometoryObject3D::BlendType::ADD:
		// 加算
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
		break;
	default:
		// アルファ
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		break;
	}

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(ResourceShader::GetShaders(vsname).Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(ResourceShader::GetShaders(psname).Get());
	gpipeline.GS = CD3DX12_SHADER_BYTECODE(ResourceShader::GetShaders(gsname).Get());
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DepthStencilState.DepthEnable = depth;
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpipeline.SampleDesc.Count = 1;
	gpipeline.pRootSignature = rootsignature.Get();

	// グラフィックスパイプラインの生成
	ComPtr<ID3D12PipelineState> pipeline;
	dxcommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipeline));

	return pipeline;
}