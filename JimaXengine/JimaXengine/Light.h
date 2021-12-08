#pragma once
#include <DirectXMath.h>
#include "DirectXCommon.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>

namespace JimaXengine
{
	class Light
	{
	private:
		using XMVECTOR = DirectX::XMVECTOR;
		template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:
		// 定数バッファ用
		struct ConstBufferData
		{
			XMVECTOR lightv;
			Vector3 lightcolor;
		};

	private:
		static DirectXCommon* dxcommon;
	public:
		static void StaticInitialize(DirectXCommon* dxcommon);

		void Initialize();

		static Light* Create();

		void Update();

		void Draw(UINT root_parameter_index);

		// 定数バッファ転送
		void TransferConstBuffer();
		
		// ライト方向をセット
		void SetDir(const XMVECTOR& arg_dir);
		
		// ライト色をセット
		void SetColor(const Vector3& arg_color);

	private:
		// 定数バッファ
		ComPtr<ID3D12Resource> constBuff;

		// 光線方向
		XMVECTOR dir = { 1,0,0,0 };

		// ライト色
		Vector3 color = { 1,1,1 };

		bool dirty = false;
	};
}