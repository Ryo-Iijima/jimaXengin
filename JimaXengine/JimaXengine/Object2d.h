#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <vector>
#include <DirectXTex.h>
#include <wrl.h>
#include <d3dx12.h>
#include <random>
#include <map>

#include "WinApp.h"
#include "DirectXCommon.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"


class Object2d
{
public:
	struct SpriteVertex
	{
		Vector3 pos;
		Vector2 uv;
	};
	struct SpriteConstBufferData
	{
		Vector4 color;
		DirectX::XMMATRIX matWP;
	};
private:
	//インスタンス間で共有したい変数
	static DirectXCommon* dxCommon;
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> spriteDescHeap;
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> spriteRootSignature;
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipelines;
	static const int spriteSRVCount;
	static UINT texNumber;
	static std::map<std::string, UINT> textureMap;
	static DirectX::XMMATRIX spriteMatProjection;

	static HRESULT CreateDescriptorHeap();
	static HRESULT CreateRootSignature();
	static HRESULT CreatePipeline(const std::string& vsfilename, const std::string& psfilename, const std::string& registername);

	//各インスタンスごとに持っていてほしい変数
	Microsoft::WRL::ComPtr<ID3D12Resource> spriteVertBuff;
	Microsoft::WRL::ComPtr<ID3D12Resource> spriteConstBuff;
	D3D12_VERTEX_BUFFER_VIEW spriteVBView;
	DirectX::XMMATRIX matWorld;

	void MatrixUpdate(Vector2 position, float angle, Vector2 scale);
	void TransferConstBuffer(Vector4 color);
	void DrawCommands(const std::string& filename, const std::string& registername, const std::string& blendtype);

public:
	Object2d();
	~Object2d();
	static void Initialize(DirectXCommon* dxcommon, WinApp* winapp);
	static void LoadTexture(const std::string& filename);
	void CreateSprite();

	void DrawOriginal(const std::string& filename,
		Vector2 position,
		float angle,
		Vector2 scale,
		std::string blendtype,
		Vector2 anchor = { 0.0f,0.0f },
		Vector4 color = { 1,1,1,1 });

	void DrawRect(const std::string& filename,
		Vector2 position,//画像の表示座標
		Vector2 texpos,//画像切り出しの左上座標
		Vector2 texlength,//左上からの長さ
		Vector2 size,//ピクセル単位での大きさ
		float angle,
		std::string blendtype,
		Vector2 anchor = { 0.0f,0.0f },
		Vector4 color = { 1,1,1,1 });

};

