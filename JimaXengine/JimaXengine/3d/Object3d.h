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
#include "../Light.h"

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
		Vector4 color = { 1,1,1,1 };
		// ローカルスケール
		XMFLOAT3 scale = { 1,1,1 };
		// ローカル回転角
		XMFLOAT3 rotation = { 0,0,0 };
		// ローカル座標
		XMFLOAT3 position = { 0,0,0 };
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
		static ComPtr<ID3D12PipelineState> piplineState;

		//ライト
		static Light* light;

	public:		// static関数
		static void StaticInitialize(DirectXCommon* dxcommon, WinApp* winapp);

		static void CreateGraphicsPipline();

		// setter
		static void SetDevice(ID3D12Device* device) { Object3d::_dev = device; }
		static void SetLight(Light* plight) { light = plight; }

	private:	// 変数

		ComPtr<ID3D12Resource> constBufferTranceform;
		ComPtr<ID3D12Resource> constBufferSkin;

		Vector3 eye;
		Vector3 target;
		Vector3 up;

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

	private:	// 関数
		/// <summary>
		/// アニメーション再生
		/// </summary>
		void PlayAnimation();

	public:		// 関数

		void Initialize();

		void Update();

		void Draw();

		// setter
		void SetModelforBuff(const std::string& modelName);

		void SetColor(Vector4& color) { this->color = color; }
		void SetScale(Vector3& scale) { this->scale = scale; }
		void SetRotation(Vector3& rotation) { this->rotation = rotation; }
		void SetPosition(Vector3& position) { this->position = position; }

		void SetCamera(Camera* camera) { this->camera = camera; }
	};
}