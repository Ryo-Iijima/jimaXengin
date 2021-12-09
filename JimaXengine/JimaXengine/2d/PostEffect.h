#pragma once
#include "Object2d.h"

namespace JimaXengine
{
	class PostEffect : public Object2d
	{
	private:
		std::string filename;
		Vector2 position;
		Vector2 size;
		float angle;
		Vector2 scale;
		std::string blendtype;
		Vector2 anchor;
		Vector4 color;

		//インスタンス間で共有したい変数
		
		static DirectXCommon* dxCommon;

		// SRV用デスクリプタヒープ
		static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapSRV;

		//各インスタンスごとに持っていてほしい変数
		// テクスチャバッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> texBuff;

	private:
		void DrawCommands(const std::string& filename, const std::string& registername, const std::string& blendtype);

	public:
		PostEffect();

		void Initialize(DirectXCommon* dxcommon);

		void Draw();
	};

}