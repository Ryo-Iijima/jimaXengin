#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <wrl.h>
#include <unordered_map>
#include "math/Vector3.h"
#include "math/Vector4.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class DirectXCommon;
class Texture
{
public:
	enum class TextureType
	{
		RESOURCE,
		CREATE,
	};

private:

	static DirectXCommon* dxCommon;
	static std::string texturepass;
	static std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12Resource>> texBuffers;
	static std::unordered_map<std::string, DirectX::TexMetadata> metadataMap;
	static std::unordered_map<std::string, Texture::TextureType> types;

	Texture();
	static void DebugLog(const std::string& filename, bool isSuccess, bool isLoadTex);

public:

	~Texture();
	static void Initialize(DirectXCommon* dxCommon);
	static void LoadTexture(const std::string& filename);
	static void CreateSimpleTexture(const std::string& filename, Vector4 color, int texWidth = 128, int texHeight = 128);

	static Microsoft::WRL::ComPtr<ID3D12Resource> GetTexture(const std::string& filename);
	static DirectX::TexMetadata GetMetadata(const std::string& filename);
	static Texture::TextureType GetTextureType(const std::string& filename);

};