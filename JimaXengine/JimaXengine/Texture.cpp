#include "Texture.h"
#include "DirectXCommon.h"

#include "Object2d.h"
#include "3d/Object3d.h"
#include "general/General.h"
#include "ConsoleOutput.h"

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;

DirectXCommon* Texture::dxCommon = nullptr;
string Texture::texturepass = "Resources/texture/";
unordered_map<string, ComPtr<ID3D12Resource>> Texture::texBuffers;
unordered_map<string, TexMetadata> Texture::metadataMap;
unordered_map<string, Texture::TextureType> Texture::types;

Texture::Texture()
{
}

void Texture::DebugLog(const std::string& filename, bool isSuccess, bool isLoadTex)
{
#ifdef _DEBUG
	string tmpFilename = "";
	string str_isLoadTex = "";
	if (isLoadTex)
	{
		str_isLoadTex = "load";
		tmpFilename = texturepass + filename;
	}
	else
	{
		str_isLoadTex = "texture create";
		tmpFilename = filename;
	}

	if (isSuccess)
	{
		ConsoleOutputColor(COLOR_GREEN);
		printf("%s %s successed\n", tmpFilename.c_str(), str_isLoadTex.c_str());
		ConsoleOutputColor();
	}
	else
	{
		ConsoleOutputColor(COLOR_RED);
		printf("%s %s failed\n", tmpFilename.c_str(), str_isLoadTex.c_str());
		ConsoleOutputColor();
	}
#endif // _DEBUG
}

Texture::~Texture()
{
}

void Texture::Initialize(DirectXCommon* dxCommon)
{
	Texture::dxCommon = dxCommon;

	CreateSimpleTexture("white", { 1,1,1,1 });
}

void Texture::LoadTexture(const std::string& filename)
{
	HRESULT result;
	ComPtr<ID3D12Resource> texbuff;
	TexMetadata metadata;
	ScratchImage scratchImg;

	// WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		General::StringToWString(Texture::texturepass + filename).c_str(),
		WIC_FLAGS_NONE,
		&metadata,
		scratchImg);

	// �t�@�C�����Ȃ��ꍇ�ǂݍ��ݒ��~
	if (FAILED(result))
	{
		DebugLog(filename, false, true);
		return;
	}

	// ���f�[�^���o
	const Image* img = scratchImg.GetImage(0, 0, 0);

	// �摜�ɍ��킹���ݒ�
	CD3DX12_RESOURCE_DESC texResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels);

	// �e�N�X�`���o�b�t�@�̐���
	result = Texture::dxCommon->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff));
	if (FAILED(result))
	{
		DebugLog(filename, false, true);
		return;
	}

	// �e�N�X�`���o�b�t�@�Ƀf�[�^��]��
	result = texbuff->WriteToSubresource(
		0,
		nullptr,
		img->pixels,
		(UINT)img->rowPitch,
		(UINT)img->slicePitch);
	if (FAILED(result))
	{
		DebugLog(filename, false, true);
		return;
	}

	// �t�@�C���̓ǂݍ��݂��I��������Ƃ��o��
	DebugLog(filename, true, true);

	texBuffers.emplace(filename, texbuff);
	metadataMap.emplace(filename, metadata);
	types.emplace(filename, TextureType::RESOURCE);

	Object2d::LoadTexture(filename);
	// Object3d::LoadResourceTexture(filename);
}

void Texture::CreateSimpleTexture(const std::string& filename, Vector4 color, int texWidth, int texHeight)
{
	HRESULT result;
	ComPtr<ID3D12Resource> texbuff;
	const int texDataCount = texWidth * texHeight; // �z��̗v�f��

	Vector4* texturedata = new Vector4[texDataCount]; // �K����ŊJ������

	// �S�s�N�Z���̐F��������
	for (int i = 0; i < texDataCount; i++)
	{
		texturedata[i] = color;
	}

	D3D12_HEAP_PROPERTIES texHeapProp{}; // �e�N�X�`���q�[�v�ݒ�
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	D3D12_RESOURCE_DESC texResDesc{}; // ���\�[�X�ݒ�
	texResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2D�e�N�X�`���p
	texResDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // RGBA�t�H�[�}�b�g
	texResDesc.Width = texWidth;
	texResDesc.Height = texHeight;
	texResDesc.DepthOrArraySize = 1;
	texResDesc.MipLevels = 1;
	texResDesc.SampleDesc.Count = 1;

	// GPU���\�[�X�̐���
	result = Texture::dxCommon->GetDevice()->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, //�e�N�X�`���p�ݒ�
		nullptr,
		IID_PPV_ARGS(&texbuff));
	if (FAILED(result))
	{
		DebugLog(filename, false, false);
		return;
	}

	// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	result = texbuff->WriteToSubresource(
		0,
		nullptr,							// �S�̈�փR�s�[
		texturedata,						// ���f�[�^�A�h���X
		sizeof(Vector4) * texWidth,			// 1���C���T�C�Y
		sizeof(Vector4) * texDataCount);	// �S�T�C�Y
	if (FAILED(result))
	{
		DebugLog(filename, false, false);
		return;
	}

	// �ǂݍ��ݐ���
	DebugLog(filename, true, false);

	texBuffers.emplace(filename, texbuff);
	types.emplace(filename, TextureType::CREATE);

	Object2d::LoadTexture(filename);
	//Object3d::LoadResourceTexture(filename);

	delete[] texturedata;
}

Microsoft::WRL::ComPtr<ID3D12Resource> Texture::GetTexture(const std::string& filename)
{
	if (texBuffers.find(filename) == texBuffers.end())
	{
		return texBuffers["white"];
	}

	return texBuffers[filename];
}

DirectX::TexMetadata Texture::GetMetadata(const std::string& filename)
{
	if (metadataMap.find(filename) == metadataMap.end())
	{
		return metadataMap["white"];
	}

	return metadataMap[filename];
}

Texture::TextureType Texture::GetTextureType(const std::string& filename)
{
	if (types.find(filename) == types.end())
	{
		return types["white"];
	}

	return types[filename];
}
