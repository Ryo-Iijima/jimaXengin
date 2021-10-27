#include "Object2d.h"
#include "Texture.h"
#include "../ResourceShader.h"

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;

DirectXCommon* Object2d::dxCommon = nullptr;
const int Object2d::spriteSRVCount = 512;
XMMATRIX Object2d::spriteMatProjection = {};
UINT Object2d::texNumber = 0;
ComPtr<ID3D12RootSignature> Object2d::spriteRootSignature = {};
map<string, ComPtr<ID3D12PipelineState>> Object2d::pipelines;
ComPtr<ID3D12DescriptorHeap> Object2d::spriteDescHeap = {};
map<string, UINT> Object2d::textureMap = {};

HRESULT Object2d::CreateDescriptorHeap()
{
	HRESULT result;

	//�f�X�N���v�^�[�q�[�v�̐���
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = spriteSRVCount;
	result = Object2d::dxCommon->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&spriteDescHeap));

	return result;
}

HRESULT Object2d::CreateRootSignature()
{
	HRESULT result;

	//�f�B�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV = {};
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	//���[�g�p�����[�^(�萔�p�A�e�N�X�`���p)
	CD3DX12_ROOT_PARAMETER rootParams[2] = {};
	rootParams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

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

	result = Object2d::dxCommon->GetDevice()->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&spriteRootSignature));

	return result;
}

HRESULT Object2d::CreatePipeline(const std::string& vsfilename, const std::string& psfilename, const std::string& registername)
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
	gpipeline.pRootSignature = spriteRootSignature.Get();

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
	result = Object2d::dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&noblend));
	if (FAILED(result))return result;

	pipelines.emplace(registername + "NOBLEND", noblend);

	//-----���C���[�t���[��-----
	ComPtr<ID3D12PipelineState> wireframe;
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	result = Object2d::dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&wireframe));
	if (FAILED(result))return result;
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	blenddesc.BlendEnable = true;

	pipelines.emplace(registername + "WIREFRAME", wireframe);

	//-----���Z-----
	ComPtr<ID3D12PipelineState> add;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA; //�\�[�X�̒l��100%�g��
	blenddesc.DestBlend = D3D12_BLEND_ONE; //�f�X�g�̒l��100%�g��
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	result = Object2d::dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&add));
	if (FAILED(result))return result;

	pipelines.emplace(registername + "ADD", add);

	//-----���Z-----
	ComPtr<ID3D12PipelineState> sub;
	blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT; //�f�X�g����\�[�X�����Z
	blenddesc.SrcBlend = D3D12_BLEND_ONE; //�\�[�X�̒l��100%�g��
	blenddesc.DestBlend = D3D12_BLEND_ONE; //�f�X�g�̒l��100%�g��
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	result = Object2d::dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&sub));
	if (FAILED(result))return result;

	pipelines.emplace(registername + "SUB", sub);

	//-----���]-----
	ComPtr<ID3D12PipelineState> invsrc;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //���Z
	blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR; //1.0f-�f�X�g�J���[�l
	blenddesc.DestBlend = D3D12_BLEND_ZERO; //�g��Ȃ�
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	result = Object2d::dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&invsrc));
	if (FAILED(result))return result;

	pipelines.emplace(registername + "INVSRC", invsrc);

	//-----������-----
	ComPtr<ID3D12PipelineState> alpha;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA; //�\�[�X�̃A���t�@�l
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA; //1.0f-�\�[�X�̃A���t�@�l
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	result = Object2d::dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&alpha));
	if (FAILED(result))return result;

	pipelines.emplace(registername + "ALPHA", alpha);

	ComPtr<ID3D12PipelineState> depthalpha;
	gpipeline.DepthStencilState.DepthEnable = true;
	Object2d::dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&depthalpha));
	pipelines.emplace(registername + "ALPHADEPTH", depthalpha);

	return result;
}

Object2d::Object2d()
{
}

Object2d::~Object2d()
{
}

void Object2d::Initialize(DirectXCommon* dxcommon, WinApp* winapp)
{
	Object2d::dxCommon = dxcommon;

	if (FAILED(CreateDescriptorHeap()))assert(0);
	if (FAILED(CreateRootSignature()))assert(0);
	if (FAILED(CreatePipeline("Sprite/SpriteVertexShader.hlsl", "Sprite/SpritePixelShader.hlsl", "Basic2D")))assert(0);

	//�ˉe�s��̕ϊ�
	spriteMatProjection = XMMatrixOrthographicOffCenterLH(
		0.0f,
		(float)winapp->GetWindowWidth(),
		(float)winapp->GetWindowHeight(),
		0.0f,
		0.0f,
		1.0f);

#ifdef _DEBUG
	printf("object2d initialized\n");
#endif // _DEBUG

}

void Object2d::LoadTexture(const std::string& filename)
{
	//�V�F�[�_�[���\�[�X�r���[�쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

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
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	Object2d::dxCommon->GetDevice()->CreateShaderResourceView(
		Texture::GetTexture(filename).Get(),
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			spriteDescHeap->GetCPUDescriptorHandleForHeapStart(),
			texNumber,
			Object2d::dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	textureMap.emplace(filename, texNumber);

	texNumber++;
}

void Object2d::CreateSprite()
{
	HRESULT result;

	result = S_FALSE;

	//���_�o�b�t�@����
	result = Object2d::dxCommon->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(SpriteVertex) * 4),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&spriteVertBuff));

	//���_�o�b�t�@�r���[�̍쐬
	spriteVBView.BufferLocation = spriteVertBuff->GetGPUVirtualAddress();
	spriteVBView.SizeInBytes = (sizeof(SpriteVertex) * 4);
	spriteVBView.StrideInBytes = sizeof(SpriteVertex);

	//�萔�o�b�t�@�̐���
	result = Object2d::dxCommon->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(SpriteConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&spriteConstBuff));
}

void Object2d::DrawOriginal(const std::string& filename, Vector2 position, float angle, Vector2 scale, string blendtype, Vector2 anchor, Vector4 color)
{
	D3D12_RESOURCE_DESC resDesc = Texture::GetTexture(filename)->GetDesc();
	float width = (float)resDesc.Width;
	float height = (float)resDesc.Height;

	float left = (0.0f - anchor.x) * width;
	float right = (1.0f - anchor.x) * width;
	float top = (0.0f - anchor.y) * height;
	float bottom = (1.0f - anchor.y) * height;

	SpriteVertex vertices[4];
	enum { LB, LT, RB, RT };
	vertices[LB] = { {  left, bottom, 0.0f },{ 0.0f, 1.0f } };
	vertices[LT] = { {  left,    top, 0.0f },{ 0.0f, 0.0f } };
	vertices[RB] = { { right, bottom, 0.0f },{ 1.0f, 1.0f } };
	vertices[RT] = { { right,    top, 0.0f },{ 1.0f, 0.0f } };

	SpriteVertex* vertMap = nullptr;
	spriteVertBuff->Map(0, nullptr, (void**)&vertMap);
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];
	}
	spriteVertBuff->Unmap(0, nullptr);

	MatrixUpdate(position, angle, scale);
	TransferConstBuffer({ color.x,color.y,color.z,color.w });
	DrawCommands(filename, "Basic2D", blendtype);
}

void Object2d::DrawRect(const std::string& filename, Vector2 position, Vector2 texpos, Vector2 texlength, Vector2 size, float angle, string blendtype, Vector2 anchor, Vector4 color)
{
	D3D12_RESOURCE_DESC resDesc = Texture::GetTexture(filename)->GetDesc();
	float width = (float)resDesc.Width;
	float height = (float)resDesc.Height;

	//position
	float left = (0.0f - anchor.x) * size.x;
	float right = (1.0f - anchor.x) * size.x;
	float top = (0.0f - anchor.y) * size.y;
	float bottom = (1.0f - anchor.y) * size.y;

	//UV
	float tex_left = texpos.x / width;
	float tex_right = (texpos.x + texlength.x) / width;
	float tex_top = texpos.y / height;
	float tex_bottom = (texpos.y + texlength.y) / height;

	SpriteVertex vertices[4];
	enum { LB, LT, RB, RT };
	vertices[LB] = { {  left, bottom, 0.0f },{ tex_left, tex_bottom } };
	vertices[LT] = { {  left,    top, 0.0f },{ tex_left, tex_top } };
	vertices[RB] = { { right, bottom, 0.0f },{ tex_right, tex_bottom } };
	vertices[RT] = { { right,    top, 0.0f },{ tex_right, tex_top } };

	SpriteVertex* vertMap = nullptr;
	spriteVertBuff->Map(0, nullptr, (void**)&vertMap);
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];
	}
	spriteVertBuff->Unmap(0, nullptr);

	MatrixUpdate(position, angle, { 1,1 });
	TransferConstBuffer({ color.x,color.y,color.z,color.w });
	DrawCommands(filename, "Basic2D", blendtype);
}

void Object2d::MatrixUpdate(Vector2 position, float angle, Vector2 scale)
{
	matWorld = XMMatrixIdentity();
	matWorld *= XMMatrixScaling(scale.x, scale.y, 1.0f);
	matWorld *= XMMatrixRotationZ(XMConvertToRadians(angle));
	matWorld *= XMMatrixTranslation(position.x, position.y, 0.0f);
}

void Object2d::TransferConstBuffer(Vector4 color)
{
	//�萔�o�b�t�@�Ƀf�[�^�]��
	SpriteConstBufferData* constMap = nullptr;
	spriteConstBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = color;
	constMap->matWP = matWorld * spriteMatProjection;
	spriteConstBuff->Unmap(0, nullptr);
}

void Object2d::DrawCommands(const std::string& filename, const std::string& registername, const std::string& blendtype)
{
	Object2d::dxCommon->GetCommandList()->SetPipelineState(pipelines[registername + blendtype].Get());
	Object2d::dxCommon->GetCommandList()->SetGraphicsRootSignature(spriteRootSignature.Get());
	Object2d::dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D12DescriptorHeap* ppHeaps[] = { spriteDescHeap.Get() };
	Object2d::dxCommon->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	Object2d::dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &spriteVBView);

	Object2d::dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, spriteConstBuff->GetGPUVirtualAddress());
	Object2d::dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(
		spriteDescHeap->GetGPUDescriptorHandleForHeapStart(),
		textureMap[filename],
		Object2d::dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	Object2d::dxCommon->GetCommandList()->DrawInstanced(4, 1, 0, 0);
}