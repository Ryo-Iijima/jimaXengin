#pragma once
#include "Object2d.h"

namespace JimaXengine
{
	class PostEffect
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

		//// インスタンス間で共有したい変数 ////
		
		static DirectXCommon* dxCommon;
		
		// SRV用デスクリプタヒープ
		static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapSRV;
		
		// RTV用デスクリプタヒープ
		static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapRTV;
		
		// DSV用デスクリプタヒープ
		static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapDSV;

		// 画面クリアカラー
		static const float clearColor[4];

		// グラフィックスパイプライン
		static std::map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipelines;

		// ルートシグネチャ
		static Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;


		//// 各インスタンスごとに持っていてほしい変数 ////

		// 頂点バッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;

		// 定数バッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;

		// 定数バッファビュー
		D3D12_VERTEX_BUFFER_VIEW vBView;

		// テクスチャバッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> texBuff[2];
		
		// 深度バッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;


	private:

		void DrawCommands(const std::string& filename, const std::string& registername, const std::string& blendtype);

		// ルートシグネチャ作成
		void CreateRootSignature();

		// テクスチャ生成
		void GenerateTexture();

		// SRV作成
		void CreateSRV();
		
		// RTV作成
		void CreateRTV();
		
		// 深度バッファ生成
		void GenerateDepthBuffer();
		
		// DSV作成
		void CreateDSV();

		// パイプライン生成
		void CreateGraphicsPiplineState(const std::string& vsfilename, const std::string& psfilename, const std::string& registername);

	public:
		PostEffect();

		void Initialize(DirectXCommon* dxcommon);

		void Draw();

		// 描画前処理
		void PreDrawScene();

		// 描画後処理
		void PostDrawScene();

	};

}