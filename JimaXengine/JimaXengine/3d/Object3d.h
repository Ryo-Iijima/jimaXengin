#pragma once

#include "Model.h"
#include "../camera/Camera.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>
#include "../math/Vector4.h"
#include "../general/Input.h"
#include "../DirectXCommon.h"
#include "../LightGroup.h"

namespace JimaXengine
{
	class Object3d
	{
	private:	// エイリアス
		using XMFLOAT2 = DirectX::XMFLOAT2;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMFLOAT4 = DirectX::XMFLOAT4;
		using XMMATRIX = DirectX::XMMATRIX;
		using TexMetadata = DirectX::TexMetadata;
		using ScratchImage = DirectX::ScratchImage;

		using string = std::string;
		template <class T>using vector = std::vector<T>;

		template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:		// 定数
		// ボーンの最大数
		static const int MAX_BONES = 256;

		enum class ViewName
		{
			transform,
			texture,
			skin,
			light,
			material,
		};
	public:		// サブクラス
		// 座標変換行列(CB0)用構造体
		struct TransformData
		{
			Vector4 color;
			XMMATRIX viewproj;
			XMMATRIX world;
			XMFLOAT3 cameraPos;
		};
		// スキニング情報(CB3)用構造体
		struct SkinData
		{
			XMMATRIX bones[MAX_BONES];
		};
	protected:

		// 色
		Vector4& color;
		// ローカルスケール
		XMFLOAT3& scale;
		// ローカル回転角
		XMFLOAT3& rotation;
		// ローカル座標
		XMFLOAT3& position;

		// ローカルワールド変換行列
		XMMATRIX matWorld;
		// モデル
		Model* model = nullptr;
		// カメラ
		Camera* camera = nullptr;

	private:	// static変数

		static DirectXCommon* dxCommon;

		static ID3D12Device* _dev;
		// ルートシグネチャ
		static ComPtr<ID3D12RootSignature> rootSignature;
		// パイプラインステート
		static ComPtr<ID3D12PipelineState> pipelineState;

		//ライト
		static LightGroup* lightGroup;

		// ビルボードかどうか
		bool isBillboard = false;

	public:		// static関数
		static void StaticInitialize(DirectXCommon* dxcommon, WinApp* winapp);

		static void CreateGraphicsPipeline();

		// setter
		static void SetDevice(ID3D12Device* device) { Object3d::_dev = device; }
		static void SetLightGroup(LightGroup* plight) { lightGroup = plight; }

	private:	// 変数

		ComPtr<ID3D12Resource> constBufferTransform;
		ComPtr<ID3D12Resource> constBufferSkin;

		// アニメーション再生関連
		// 1フレームの時間
		FbxTime frameTime;
		// アニメーション開始時間
		FbxTime startTime;
		// アニメーション終了時間
		FbxTime endTime;
		// 現在時間
		FbxTime currentTime;
		// アニメーション再生中
		bool isPlay = false;
		// 指定した終了時間まで再生した
		bool isEndAnimation = false;
		// アニメーションをループする
		bool isAnimationLoop = true;
		

	private:	// 関数
		// アニメーション関連初期化
		void AnimationInit();
		// アニメーション再生
		bool PlayAnimation(bool loop);

	public:		// 関数

		Object3d();
		Object3d(Vector3& pos, Vector3& scale, Vector3& rot, Vector4& color);

		void Initialize();

		void Update();

		void Draw();

		// setter
		void SetModelforBuff(const std::string& modelName);
		void SetCamera(Camera* camera) { this->camera = camera; }
		void SetIsBillboard(bool isbillbord) { isBillboard = isbillbord; }

		void SetAnimationLoop(bool loop) { isAnimationLoop = loop; }
		void SetAnimationFrame(const int start, const int end, const int frametime = 1);

		// getter
		bool GetIsEndAnimation() { return isEndAnimation; }
		LightGroup* GetUsingLightGroup() { return lightGroup; }
	};
}