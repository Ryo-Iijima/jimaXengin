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
	// 頂点データ全体のサイズ
	UINT sizeVB = static_cast<UINT>(sizeof(FBXVertexData) * model->vertices.size());

	// 頂点バッファ生成
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

	// 頂点バッファへのデータ転送
	FBXVertexData* vertMap = nullptr;
	result = model->vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	std::copy(model->vertices.begin(), model->vertices.end(), vertMap);
	model->vertBuff->Unmap(0, nullptr);

	// 頂点バッファビューの作成
	model->vbView.BufferLocation = model->vertBuff->GetGPUVirtualAddress();
	model->vbView.SizeInBytes = sizeVB;
	model->vbView.StrideInBytes = sizeof(model->vertices[0]);

	/////////////////////////////////////////////////////////////////////////////////////////////

	// 頂点インデックス全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * model->indices.size());
	// インデックスバッファ生成
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

	// インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = model->indexBuff->Map(0, nullptr, (void**)&indexMap);
	assert(SUCCEEDED(result));

	std::copy(model->indices.begin(), model->indices.end(), indexMap);
	model->indexBuff->Unmap(0, nullptr);
	// インデックスバッファビューの作成
	model->ibView.BufferLocation = model->indexBuff->GetGPUVirtualAddress();
	model->ibView.Format = DXGI_FORMAT_R16_UINT;
	model->ibView.SizeInBytes = sizeIB;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	// テクスチャ画像データ
	const DirectX::Image* img = model->materialData.scrachImg.GetImage(0, 0, 0);
	assert(img);
	// リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D
	(
		model->materialData.metadata.format,
		model->materialData.metadata.width,
		(UINT)model->materialData.metadata.height,
		(UINT16)model->materialData.metadata.arraySize,
		(UINT16)model->materialData.metadata.mipLevels
	);
	// テクスチャ用バッファの生成
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

	// テクスチャバッファにデータ転送
	result = model->texbuff->WriteToSubresource
	(
		0,
		nullptr,				// 全領域へコピー
		img->pixels,			// 元データアドレス
		(UINT)img->rowPitch,	// 1ラインサイズ
		(UINT)img->slicePitch	// 1枚サイズ
	);
	assert(SUCCEEDED(result));

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	// SRV用デスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;	// シェーダーリソースビュー用
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// シェーダーから見えるように
	descHeapDesc.NodeMask = 0;	// マスク0
	descHeapDesc.NumDescriptors = 1;
	result = ModelManager::dxCommon->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(model->descHeapSRV.GetAddressOf()));
	// シェーダーリソースビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_RESOURCE_DESC resDesc = model->texbuff->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	ModelManager::dxCommon->GetDevice()->CreateShaderResourceView
	(
		model->texbuff.Get(),										// ビューと関連付けるバッファ
		&srvDesc,													// テクスチャ設定情報
		model->descHeapSRV->GetCPUDescriptorHandleForHeapStart()	// ヒープの先頭アドス
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
