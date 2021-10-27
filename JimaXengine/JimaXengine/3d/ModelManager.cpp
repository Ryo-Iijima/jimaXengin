#include "ModelManager.h"
#include <string>
#include <ios>
#include "DirectXCommon.h";
#include ".//ConsoleOutput.h"
#include "FBXSubClass.h"

using namespace std;
using namespace DirectX;

DirectXCommon*  ModelManager::dxCommon = nullptr;
string ModelManager::modelpass = "Resources/model/";
unordered_map<string, Model> ModelManager::models;

void ModelManager::DebugLog(const std::string& filename, bool isSuccess, bool isLoadModel)
{
#ifdef _DEBUG
	string tmpFilename;
	string str_isLoadModel = "";
	if (isLoadModel)
	{
		str_isLoadModel = "load";
		tmpFilename = modelpass + filename;
	}
	else
	{
		str_isLoadModel = "model create";
		tmpFilename = filename;
	}

	if (isSuccess)
	{
		ConsoleOutputColor(COLOR_GREEN);
		printf("%s %s successed\n", tmpFilename.c_str(), str_isLoadModel.c_str());
		ConsoleOutputColor();
	}
	else
	{
		ConsoleOutputColor(COLOR_RED);
		printf("%s %s failed\n", tmpFilename.c_str(), str_isLoadModel.c_str());
		ConsoleOutputColor();

	}
#endif // _DEBUG
}


void ModelManager::Initialize(DirectXCommon* dxcommon)
{
	ModelManager::dxCommon = dxcommon;
}

HRESULT ModelManager::CreateBuffers(Model* model)
{

	HRESULT result;
	// ���_�f�[�^�S�̂̃T�C�Y
	UINT sizeVB = static_cast<UINT>(sizeof(FBXVertexData) * model->vertices.size());

	// ���_�o�b�t�@����
	result = ModelManager::dxCommon->GetDevice()->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(model->vertBuff.GetAddressOf())
	);
	assert(SUCCEEDED(result));

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	FBXVertexData* vertMap = nullptr;
	result = model->vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	std::copy(model->vertices.begin(), model->vertices.end(), vertMap);
	model->vertBuff->Unmap(0, nullptr);

	// ���_�o�b�t�@�r���[�̍쐬
	model->vbView.BufferLocation = model->vertBuff->GetGPUVirtualAddress();
	model->vbView.SizeInBytes = sizeVB;
	model->vbView.StrideInBytes = sizeof(model->vertices[0]);

	/////////////////////////////////////////////////////////////////////////////////////////////

	// ���_�C���f�b�N�X�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * model->indices.size());
	// �C���f�b�N�X�o�b�t�@����
	result = ModelManager::dxCommon->GetDevice()->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(model->indexBuff.GetAddressOf())
	);
	assert(SUCCEEDED(result));

	// �C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	unsigned short* indexMap = nullptr;
	result = model->indexBuff->Map(0, nullptr, (void**)&indexMap);
	assert(SUCCEEDED(result));

	std::copy(model->indices.begin(), model->indices.end(), indexMap);
	model->indexBuff->Unmap(0, nullptr);
	// �C���f�b�N�X�o�b�t�@�r���[�̍쐬
	model->ibView.BufferLocation = model->indexBuff->GetGPUVirtualAddress();
	model->ibView.Format = DXGI_FORMAT_R16_UINT;
	model->ibView.SizeInBytes = sizeIB;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	// �e�N�X�`���摜�f�[�^
	const DirectX::Image* img = model->materialData.scrachImg.GetImage(0, 0, 0);
	assert(img);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D
	(
		model->materialData.metadata.format,
		model->materialData.metadata.width,
		(UINT)model->materialData.metadata.height,
		(UINT16)model->materialData.metadata.arraySize,
		(UINT16)model->materialData.metadata.mipLevels
	);
	// �e�N�X�`���p�o�b�t�@�̐���
	result = ModelManager::dxCommon->GetDevice()->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(model->texbuff.GetAddressOf())
	);
	assert(SUCCEEDED(result));

	// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	result = model->texbuff->WriteToSubresource
	(
		0,
		nullptr,				// �S�̈�փR�s�[
		img->pixels,			// ���f�[�^�A�h���X
		(UINT)img->rowPitch,	// 1���C���T�C�Y
		(UINT)img->slicePitch	// 1���T�C�Y
	);
	assert(SUCCEEDED(result));

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	// SRV�p�f�X�N���v�^�q�[�v�𐶐�
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;	// �V�F�[�_�[���\�[�X�r���[�p
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// �V�F�[�_�[���猩����悤��
	descHeapDesc.NodeMask = 0;	// �}�X�N0
	descHeapDesc.NumDescriptors = 1;
	result = ModelManager::dxCommon->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(model->descHeapSRV.GetAddressOf()));
	// �V�F�[�_�[���\�[�X�r���[�쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_RESOURCE_DESC resDesc = model->texbuff->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	ModelManager::dxCommon->GetDevice()->CreateShaderResourceView
	(
		model->texbuff.Get(),										// �r���[�Ɗ֘A�t����o�b�t�@
		&srvDesc,													// �e�N�X�`���ݒ���
		model->descHeapSRV->GetCPUDescriptorHandleForHeapStart()	// �q�[�v�̐擪�A�h�X
	);
}


Mesh* ModelManager::GetObjMesh(const std::string& filename)
{
	if (models.find(filename) == models.end())
	{
		return &models["def_cube"];
	}

	return &models[filename];
}
