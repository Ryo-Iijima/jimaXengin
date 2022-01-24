#include "LightGroup.h"
#include <assert.h>

using namespace DirectX;
using namespace JimaXengine;

// 静的メンバ変数の実体
ID3D12Device* LightGroup::device = nullptr;

void LightGroup::StaticInitialize(ID3D12Device* device)
{
	// 再初期化チェック
	assert(!LightGroup::device);

	// nullptrチェック
	assert(device);

	LightGroup::device = device;
}

LightGroup* LightGroup::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	LightGroup* instance = new LightGroup();

	// 初期化
	instance->Initialize();

	return instance;
}

void LightGroup::Initialize()
{
	// nullptrチェック
	assert(device);

	DefaultLightSetting();

	HRESULT result;
	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	if (FAILED(result)) {
		assert(0);
	}

	// 定数バッファへデータ転送
	TransferConstBuffer();
}

void LightGroup::Update()
{
	// 値の更新があった時だけ定数バッファに転送する
	if (dirty) {
		TransferConstBuffer();
		dirty = false;
	}
}

void LightGroup::Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex)
{
	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(rootParameterIndex, constBuff->GetGPUVirtualAddress());
}

void LightGroup::TransferConstBuffer()
{
	HRESULT result;
	// 定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		
		// 環境光
		constMap->ambientColor = ambientColor;

		// 平行光源
		for (int i = 0; i < DirLightNum; i++) {
			// ライトが有効なら設定を転送
			if (dirLights[i].IsActive()) {
				constMap->dirLights[i].active = 1;
				constMap->dirLights[i].lightv = -dirLights[i].GetLightDir();
				constMap->dirLights[i].lightcolor = dirLights[i].GetLightColor();
			}
			// ライトが無効ならライト色を0に
			else {
				constMap->dirLights[i].active = 0;
			}
		}

		// ポイントライト
		for (int i = 0; i < PointLightNum; i++) {
			// ライトが有効なら設定を転送
			if (pointLights[i].IsActive()) {
				constMap->pointLights[i].active = 1;
				constMap->pointLights[i].lightPos = pointLights[i].GetLightPos();
				constMap->pointLights[i].lightColor = pointLights[i].GetLightColor();
				constMap->pointLights[i].lightAtten = pointLights[i].GetLightAtten();
			}
			// ライトが無効なら色を0に
			else {
				constMap->pointLights[i].active = 0;
			}
		}

		// スポットライト
		for (int i = 0; i < SpotLightNum; i++) {
			// ライトが有効なら設定を転送
			if (spotLights[i].IsActive()) {
				constMap->spotLights[i].active = 1;
				constMap->spotLights[i].lightv = -spotLights[i].GetLightDir();
				constMap->spotLights[i].lightPos = spotLights[i].GetLightPos();
				constMap->spotLights[i].lightColor = spotLights[i].GetLightColor();
				constMap->spotLights[i].lightAtten = spotLights[i].GetLightAtten();
				constMap->spotLights[i].lightFactorAngleCos = spotLights[i].GetLightFactorAngleCos();
			}
			// ライトが無効なら色を0に
			else {
				constMap->spotLights[i].active = 0;
			}
		}

		// 丸影
		for (int i = 0; i < CircleShadowNum; i++) {
			// 有効なら設定を転送
			if (circleShadows[i].IsActive()) {
				constMap->circleShadows[i].active = 1;
				constMap->circleShadows[i].dir = -circleShadows[i].GetDir();
				constMap->circleShadows[i].casterPos = circleShadows[i].GetCasterPos();
				constMap->circleShadows[i].distanceCasterPos = circleShadows[i].GetDistanceCasterLight();
				constMap->circleShadows[i].atten = circleShadows[i].GetAtten();
				constMap->circleShadows[i].factorAngleCos = circleShadows[i].GetFactorAngleCos();
			}
			// 無効なら0に
			else {
				constMap->circleShadows[i].active = 0;
			}
		}

		constBuff->Unmap(0, nullptr);
	}
}

void LightGroup::DefaultLightSetting()
{
	dirLights[0].SetActive(true);
	dirLights[0].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights[0].SetLightDir({ 0.0f, -1.0f, 0.0f, 0 });

	dirLights[1].SetActive(true);
	dirLights[1].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights[1].SetLightDir({ +0.5f, +0.1f, +0.2f, 0 });

	dirLights[2].SetActive(true);
	dirLights[2].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights[2].SetLightDir({ -0.5f, +0.1f, -0.2f, 0 });
}

void LightGroup::SetAmbientColor(const Vector3& color)
{
	ambientColor = color;
	dirty = true;
}

void LightGroup::SetDirLightActive(int index, bool active)
{
	assert(0 <= index && index < DirLightNum);

	dirLights[index].SetActive(active);
}

void LightGroup::SetDirLightDir(int index, const XMVECTOR& lightdir)
{
	assert(0 <= index && index < DirLightNum);

	dirLights[index].SetLightDir(lightdir);
	dirty = true;
}

void LightGroup::SetDirLightColor(int index, const Vector3& lightcolor)
{
	assert(0 <= index && index < DirLightNum);

	dirLights[index].SetLightColor(lightcolor);
	dirty = true;
}

void JimaXengine::LightGroup::SetPointLightActive(int index, bool active)
{
	assert(0 <= index && index < PointLightNum);

	pointLights[index].SetActive(active);
}

void JimaXengine::LightGroup::SetPointLightPos(int index, const Vector3& lightpos)
{
	assert(0 <= index && index < PointLightNum);

	pointLights[index].SetLightPos(lightpos);
	dirty = true;
}

void JimaXengine::LightGroup::SetPointLightColor(int index, const Vector3& lightcolor)
{
	assert(0 <= index && index < PointLightNum);

	pointLights[index].SetLightColor(lightcolor);
	dirty = true;
}

void JimaXengine::LightGroup::SetPointLightAtten(int index, const Vector3& lightAtten)
{
	assert(0 <= index && index < PointLightNum);

	pointLights[index].SetLightAtten(lightAtten);
	dirty = true;
}

void JimaXengine::LightGroup::SetSpotLightActive(int index, bool active)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights[index].SetActive(active);
}

void JimaXengine::LightGroup::SetSpotDir(int index, const XMVECTOR& lightdir)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights[index].SetLightDir(lightdir);
	dirty = true;
}

void JimaXengine::LightGroup::SetSpotLightPos(int index, const Vector3& lightpos)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights[index].SetLightPos(lightpos);
	dirty = true;
}

void JimaXengine::LightGroup::SetSpotLightColor(int index, const Vector3& lightcolor)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights[index].SetLightColor(lightcolor);
	dirty = true;
}

void JimaXengine::LightGroup::SetSpotLightAtten(int index, const Vector3& lightAtten)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights[index].SetLightAtten(lightAtten);
	dirty = true;
}

void JimaXengine::LightGroup::SetSpotLightFactorAngle(int index, const Vector2& lightFactorAngle)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights[index].SetLightFactorAngle(lightFactorAngle);
	dirty = true;
}

void JimaXengine::LightGroup::SetCircleShadowActiv(int index, bool activ)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows[index].SetActive(activ);
}

void JimaXengine::LightGroup::SetCircleShadowCasterPos(int index, const Vector3& casterpos)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows[index].SetCasterPos(casterpos);
	dirty = true;
}

void JimaXengine::LightGroup::SetCircleShadowDir(int index, const XMVECTOR& dir)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows[index].SetDir(dir);
	dirty = true;
}

void JimaXengine::LightGroup::SetCircleShadowDistanceCasterLight(int index, float distancecasterlight)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows[index].SetDistanceCasterLight(distancecasterlight);
	dirty = true;
}

void JimaXengine::LightGroup::SetCircleShadowAtten(int index, const Vector3& atten)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows[index].SetAtten(atten);
	dirty = true;
}

void JimaXengine::LightGroup::SetCircleShadowFactorAngle(int index, const Vector2& factorangle)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows[index].SetFactorAngle(factorangle);
	dirty = true;
}
