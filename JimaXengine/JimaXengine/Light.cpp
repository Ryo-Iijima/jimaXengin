#include "Light.h"
using namespace DirectX;

JimaXengine::DirectXCommon* JimaXengine::Light::dxcommon = nullptr;

void JimaXengine::Light::StaticInitialize(DirectXCommon* dxcommon)
{
	// 再初期化チェック
	assert(!Light::dxcommon);
	// nullptrチェック
	assert(dxcommon);
	
	Light::dxcommon = dxcommon;
}

void JimaXengine::Light::Initialize()
{
	HRESULT result;
	// 定数バッファの生成
	result = dxcommon->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);
	if (FAILED(result)) { assert(0); }

	// 初期値を転送しておく
	TransferConstBuffer();
}

JimaXengine::Light* JimaXengine::Light::Create()
{
	// インスタンス生成
	Light* instance = new Light();
	// 初期化
	instance->Initialize();

	return instance;
}

void JimaXengine::Light::Update()
{
	// 値に更新があったときだけ転送
	if (dirty)
	{
		TransferConstBuffer();
		dirty = false;
	}
}

void JimaXengine::Light::Draw(UINT root_parameter_index)
{
	dxcommon->GetCommandList()->SetGraphicsRootConstantBufferView(root_parameter_index, constBuff->GetGPUVirtualAddress());
}

void JimaXengine::Light::TransferConstBuffer()
{
	HRESULT result;
	// 定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		constMap->lightv = -dir;
		constMap->lightcolor = color;
		constBuff->Unmap(0, nullptr);
	}
}

void JimaXengine::Light::SetDir(const XMVECTOR& arg_dir)
{
	// 正規化してセット
	dir = XMVector3Normalize(arg_dir);
	dirty = true;
}

void JimaXengine::Light::SetColor(const Vector3& arg_color)
{
	color = arg_color;
	dirty = true;
}
