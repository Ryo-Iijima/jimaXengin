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

	//�f�B�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV0 = {};
	descRangeSRV0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);	// t0���W�X�^

	CD3DX12_DESCRIPTOR_RANGE descRangeSRV1 = {};
	descRangeSRV1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);	// t1���W�X�^

	//���[�g�p�����[�^(�萔�p�A�e�N�X�`���p)
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

	// �e�N�X�`�����\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		WinApp::WINDOW_WIDTH,
		WinApp::WINDOW_HEIGHT,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	for (int i = 0; i < 2; i++)
	{
		// �e�N�X�`���o�b�t�@�̐���
		result = dxCommon->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor),
			IID_PPV_ARGS(&texBuff[i])
		);
		assert(SUCCEEDED(result));

		{// �e�N�X�`����ԃN���A
			// ��f��
			const UINT pixelCount = WinApp::WINDOW_WIDTH * WinApp::WINDOW_HEIGHT;
			// �摜�P�s���̃f�[�^�T�C�Y
			const UINT rowPitch = sizeof(UINT) * WinApp::WINDOW_WIDTH;
			// �摜�S�̂̃f�[�^�T�C�Y
			const UINT depthPitch = rowPitch * WinApp::WINDOW_HEIGHT;
			// �摜�C���[�W
			UINT* img = new UINT[pixelCount];
			for (int j = 0; j < pixelCount; j++) { img[j] = 0xff0000ff; }

			// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
			result = texBuff[i]->WriteToSubresource(0, nullptr, img, rowPitch, depthPitch);
			assert(SUCCEEDED(result));
			delete[] img;
		}
	}

}

void JimaXengine::PostEffect::CreateSRV()
{
	HRESULT result;

	// SRV�p�f�X�N���v�^�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 2;
	result = dxCommon->GetDevice()->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV));
	// SRV�p�f�X�N���v�^�q�[�v�𐶐�
	result = dxCommon->GetDevice()->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV));
	assert(SUCCEEDED(result));
	//�V�F�[�_�[���\�[�X�r���[�쐬
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

	// RTV�p�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHeapDesc.NumDescriptors = 2;
	// RTV�p�f�X�N���v�^�q�[�v�𐶐�
	result = dxCommon->GetDevice()->CreateDescriptorHeap(&rtvDescHeapDesc, IID_PPV_ARGS(&descHeapRTV));
	assert(SUCCEEDED(result));

	for (int i = 0; i < 2; i++)
	{
		// �f�X�N���v�^�q�[�v��RTV�쐬
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

	// �[�x�o�b�t�@���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		WinApp::WINDOW_WIDTH,
		WinApp::WINDOW_HEIGHT,
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);
	// �[�x�o�b�t�@�̐���
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

	// DSV�p�f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC dsvDescHeapDesc{};
	dsvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescHeapDesc.NumDescriptors = 1;
	// DSV�p�f�X�N���v�^�q�[�v���쐬
	result = dxCommon->GetDevice()->CreateDescriptorHeap(&dsvDescHeapDesc, IID_PPV_ARGS(&descHeapDSV));
	assert(SUCCEEDED(result));
	// �f�X�N���v�^�q�[�v��DSV�쐬
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

	//���_���C�A�E�g�z��̐錾
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};

	//�p�C�v���C���X�e�[�g
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
	gpipeline.DepthStencilState.DepthEnable = false; //2D�̃X�v���C�g�摜�͐[�x�e�X�g�����Ȃ�����(�R�[�h�̏��Ԃŕ`�悳��Ă���)
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	gpipeline.pRootSignature = rootSignature.Get();

	//�u�����h���[�h
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc = {};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//-----�m�[�u�����h-----
	ComPtr<ID3D12PipelineState> noblend;
	blenddesc.BlendEnable = false;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; //���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; //�\�[�X�̒l��100%�g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; //�f�X�g�̒l��0%�g��
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.BlendState.RenderTarget[1] = blenddesc;
	result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&noblend));
	assert(SUCCEEDED(result));

	pipelines.emplace(registername + "NOBLEND", noblend);

	//-----���C���[�t���[��-----
	ComPtr<ID3D12PipelineState> wireframe;
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.BlendState.RenderTarget[1] = blenddesc;
	result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&wireframe));
	assert(SUCCEEDED(result));
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	blenddesc.BlendEnable = true;

	pipelines.emplace(registername + "WIREFRAME", wireframe);

	//-----���Z-----
	ComPtr<ID3D12PipelineState> add;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA; //�\�[�X�̒l��100%�g��
	blenddesc.DestBlend = D3D12_BLEND_ONE; //�f�X�g�̒l��100%�g��
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.BlendState.RenderTarget[1] = blenddesc;
	result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&add));
	assert(SUCCEEDED(result));

	pipelines.emplace(registername + "ADD", add);

	//-----���Z-----
	ComPtr<ID3D12PipelineState> sub;
	blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT; //�f�X�g����\�[�X�����Z
	blenddesc.SrcBlend = D3D12_BLEND_ONE; //�\�[�X�̒l��100%�g��
	blenddesc.DestBlend = D3D12_BLEND_ONE; //�f�X�g�̒l��100%�g��
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.BlendState.RenderTarget[1] = blenddesc;
	result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&sub));
	assert(SUCCEEDED(result));

	pipelines.emplace(registername + "SUB", sub);

	//-----���]-----
	ComPtr<ID3D12PipelineState> invsrc;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //���Z
	blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR; //1.0f-�f�X�g�J���[�l
	blenddesc.DestBlend = D3D12_BLEND_ZERO; //�g��Ȃ�
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.BlendState.RenderTarget[1] = blenddesc;
	result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&invsrc));
	assert(SUCCEEDED(result));

	pipelines.emplace(registername + "INVSRC", invsrc);

	//-----������-----
	ComPtr<ID3D12PipelineState> alpha;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA; //�\�[�X�̃A���t�@�l
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA; //1.0f-�\�[�X�̃A���t�@�l
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

	// ���_�f�[�^
	JimaXengine::Object2d::SpriteVertex vertices[4] = {
		{{-1.0f,-1.0f,0.0f},{ 0.0f, 1.0f }},
		{{-1.0f,+1.0f,0.0f},{ 0.0f, 0.0f }},
		{{+1.0f,-1.0f,0.0f},{ 1.0f, 1.0f }},
		{{+1.0f,+1.0f,0.0f},{ 1.0f, 0.0f }},
	};

	//���_�o�b�t�@����
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(JimaXengine::Object2d::SpriteVertex) * 4),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));


	// ���_�o�b�t�@�ւ̃f�[�^�]��
	JimaXengine::Object2d::SpriteVertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}
	// ���_�o�b�t�@�r���[�̍쐬
	vBView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vBView.SizeInBytes = sizeof(JimaXengine::Object2d::SpriteVertex) * 4;
	vBView.StrideInBytes = sizeof(JimaXengine::Object2d::SpriteVertex);

	// �萔�o�b�t�@�̐���
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
	//	// �f�X�N���v�^�q�[�v��SRV���쐬
	//	static int tex = 0;
	//	// �e�N�X�`���ԍ���0��1�Ő؂�ւ�
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

	//�萔�o�b�t�@�Ƀf�[�^�]��
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
		// ���\�[�X�o���A��ύX�i�V�F�[�_�[���\�[�X���`��\�j
		dxCommon->GetCommandList()->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(texBuff[i].Get(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				D3D12_RESOURCE_STATE_RENDER_TARGET)
		);
	}

	// �����_�[�^�[�Q�b�g�r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHs[2]; 
	for (int i = 0; i < 2; i++)
	{
		rtvHs[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeapRTV->GetCPUDescriptorHandleForHeapStart(),
			i,
			dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		);
	}

	// �[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = descHeapDSV->GetCPUDescriptorHandleForHeapStart();
	// �����_�[�^�[�Q�b�g���Z�b�g
	dxCommon->GetCommandList()->OMSetRenderTargets(2, rtvHs, false, &dsvH);

	CD3DX12_VIEWPORT viewports[2];
	CD3DX12_RECT scissorRects[2];
	for (int i = 0; i < 2; i++)
	{
		viewports[i] = CD3DX12_VIEWPORT(0.0f, 0.0f, WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT);
		scissorRects[i] = CD3DX12_RECT(0.0f, 0.0f, WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT);
	}
	// �r���[�|�[�g�̐ݒ�
	dxCommon->GetCommandList()->RSSetViewports(2, viewports);
	// �V�U�����O��`�̐ݒ�
	dxCommon->GetCommandList()->RSSetScissorRects(2, scissorRects);

	for (int i = 0; i < 2; i++)
	{
		// �S��ʃN���A
		dxCommon->GetCommandList()->ClearRenderTargetView(rtvHs[i], clearColor, 0, nullptr);
	}
	// �[�x�o�b�t�@�̃N���A
	dxCommon->GetCommandList()->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void JimaXengine::PostEffect::PostDrawScene()
{
	for (int i = 0; i < 2; i++)
	{
		// ���\�[�X�o���A��ύX�i�`��\���V�F�[�_�[���\�[�X�j
		dxCommon->GetCommandList()->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(texBuff[i].Get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
		);
	}
}
