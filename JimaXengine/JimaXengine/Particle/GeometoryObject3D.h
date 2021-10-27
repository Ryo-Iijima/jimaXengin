#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <forward_list>
#include <map>
#include <unordered_map>
#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../camera/Camera.h"
#include "ParticleParam.h"
#include "../DirectXCommon.h"
#include "ParticleBase.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class GeometoryObject3D
{
public:
	struct VertexPos
	{
		Vector3 pos;
		float scale;
		Vector3 rot;
		Vector4 color;
	};
	struct ConstBufferData
	{
		XMMATRIX matV;
		XMMATRIX matP;
		XMMATRIX matBillboard;
	};
	enum class BILLBOARD
	{
		BILLBOARD_NONE,
		BILLBOARD_ALL,
		BILLBOARD_Y,
	};
	enum class BlendType
	{
		ALPHA,
		ADD,
	};

public:
	static bool PreInitialize(DirectXCommon* dxcommon);
	static void LoadTexture(const std::string& filename);

	void Initialize();
	void Update(std::forward_list<ParticleBase*>& particles);
	void Draw(Camera* camera, std::forward_list<ParticleBase*>& particles, const std::string& filename, const std::string& pipelinename, BILLBOARD billboardType = BILLBOARD::BILLBOARD_NONE);

private:
	static HRESULT InitializeDescriptorHeap();
	static HRESULT InitializeRootSignature();
	static ComPtr<ID3D12PipelineState> InitializeGraphicsPipeline(BlendType type, const std::string& vsname, const std::string& psname, const std::string& gsname, bool depth);
	
	static const int VERTEX_MAX_COUNT = 10000;
	static const int SRV_MAX_COUNT = 256;
	
	static DirectXCommon* dxcommon;
	static ComPtr<ID3D12DescriptorHeap> descHeap;
	static ComPtr<ID3D12RootSignature> rootsignature;
	static std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> pipelines;
	static std::map<std::string, int> textureMap;
	static int texNumber;

	ComPtr<ID3D12Resource> vertBuff;
	D3D12_VERTEX_BUFFER_VIEW vbView;
	VertexPos vertices[VERTEX_MAX_COUNT];
	ComPtr<ID3D12Resource> constBuff;
};