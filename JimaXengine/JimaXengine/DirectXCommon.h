#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include <vector>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#include "WinApp.h"

#include <DirectXMath.h>
#include <d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

#include <DirectXTex.h>
#pragma comment (lib,"DirectXTex.lib")

#include <d3dx12.h>
using namespace DirectX;

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "camera/Camera.h"

namespace JimaXengine
{
	class DirectXCommon
	{
	private:	// エイリアス
		template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:	// 構造体
		// 頂点データ構造体
		struct Vertex
		{
			Vector3 pos;
			Vector3 normal;
			Vector2 uv;
		};

		struct ConstBfferData
		{
			XMMATRIX matrix;
			XMFLOAT4 color;
		};

	private:	// メンバ変数
		// ウィンドウズアプリケーション管理
		WinApp* winApp = nullptr;

		HRESULT result;

		ComPtr<ID3D12Device> _dev;
		ComPtr<IDXGIFactory6> _dxgiFactory;
		ComPtr<IDXGISwapChain4> _swapchain;
		ComPtr<ID3D12CommandAllocator> _cmdAllocator;
		ComPtr<ID3D12GraphicsCommandList> _cmdList;
		ComPtr<ID3D12CommandQueue> _cmdQueue;
		std::vector<ID3D12Resource*> _backBuffers;
		ComPtr<ID3D12DescriptorHeap> rtvHeaps;
		UINT bbIdx;
		D3D12_RESOURCE_BARRIER barrierDesc = {};
		D3D12_CPU_DESCRIPTOR_HANDLE rtvH;
		D3D12_CPU_DESCRIPTOR_HANDLE dsvH;
		ComPtr<ID3D12Fence> _fence;
		UINT16 _fenceVal = 0;

		float clearColor[4] = { 0.3f,0.3f,0.7f,1.0f };	// 画面クリア色

		ComPtr<ID3D12RootSignature> rootsignature;
		D3D12_VERTEX_BUFFER_VIEW vbView = {};
		D3D12_INDEX_BUFFER_VIEW ibView = {};
		ComPtr<ID3D12DescriptorHeap> basicDescHeap;
		ComPtr<ID3D12PipelineState> _piplineState = nullptr;
		ComPtr<ID3D12Resource> vertBuff = nullptr;
		D3D12_RESOURCE_DESC resdesc = {};
		D3D12_HEAP_PROPERTIES heapprop = {};
		ComPtr<ID3D12Resource> idxBuff = nullptr;
		TexMetadata metadata = {};
		ComPtr<ID3D12Resource> texbuff = nullptr;
		ComPtr<ID3DBlob> vsBlob = nullptr;		// シェーダー保持用
		ComPtr<ID3DBlob> psBlob = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;
		D3D12_DESCRIPTOR_RANGE descTblRange[2] = {};
		D3D12_ROOT_PARAMETER rootparam = {};
		D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
		D3D12_VIEWPORT viewport = {};
		D3D12_RECT scissorrect = {};
		ComPtr<ID3D12Resource> depthBuffer = nullptr;
		ComPtr<ID3D12Resource> constBuff = nullptr;
		ComPtr<ID3D12DescriptorHeap> dsvHeap = nullptr;

		XMMATRIX matWorld;			// ワールド
		XMMATRIX matView;			// ビュー
		XMMATRIX matProjection;		// プロジェクション

		// imgui用ヒープ
		ComPtr<ID3D12DescriptorHeap> _heapForImgui;

		// 頂点座標
		const static unsigned int vertNum = 24;
		Vertex vertices[vertNum] =
		{
			// 前
			{{-1.0f,-1.0f,-1.0f}, {}, {0.0f,1.0f}},
			{{-1.0f, 1.0f,-1.0f}, {}, {0.0f,0.0f}},
			{{ 1.0f,-1.0f,-1.0f}, {}, {1.0f,1.0f}},
			{{ 1.0f, 1.0f,-1.0f}, {}, {1.0f,0.0f}},
			// 後					 
			{{ 1.0f,-1.0f, 1.0f}, {}, {0.0f,1.0f}},
			{{ 1.0f, 1.0f, 1.0f}, {}, {0.0f,0.0f}},
			{{-1.0f,-1.0f, 1.0f}, {}, {1.0f,1.0f}},
			{{-1.0f, 1.0f, 1.0f}, {}, {1.0f,0.0f}},
			// 左					 
			{{-1.0f,-1.0f,-1.0f}, {}, {0.0f,1.0f}},
			{{-1.0f,-1.0f, 1.0f}, {}, {0.0f,0.0f}},
			{{-1.0f, 1.0f,-1.0f}, {}, {1.0f,1.0f}},
			{{-1.0f, 1.0f, 1.0f}, {}, {1.0f,0.0f}},
			// 右					 
			{{ 1.0f,-1.0f,-1.0f}, {}, {0.0f,1.0f}},
			{{ 1.0f, 1.0f,-1.0f}, {}, {0.0f,0.0f}},
			{{ 1.0f,-1.0f, 1.0f}, {}, {1.0f,1.0f}},
			{{ 1.0f, 1.0f, 1.0f}, {}, {1.0f,0.0f}},
			// 上					 
			{{-1.0f, 1.0f,-1.0f}, {}, {0.0f,1.0f}},
			{{-1.0f, 1.0f, 1.0f}, {}, {0.0f,0.0f}},
			{{ 1.0f, 1.0f,-1.0f}, {}, {1.0f,1.0f}},
			{{ 1.0f, 1.0f, 1.0f}, {}, {1.0f,0.0f}},
			// 下
			{{ 1.0f,-1.0f,-1.0f}, {}, {0.0f,1.0f}},
			{{ 1.0f,-1.0f, 1.0f}, {}, {0.0f,0.0f}},
			{{-1.0f,-1.0f,-1.0f}, {}, {1.0f,1.0f}},
			{{-1.0f,-1.0f, 1.0f}, {}, {1.0f,0.0f}},
		};

		// インデックス情報
		const static unsigned short indicesNum = 36;
		unsigned short indices[indicesNum] =
		{
			// 前
			0,1,2,
			2,1,3,
			// 後
			4,5,6,
			6,5,7,
			// 左
			8,9,10,
			10,9,11,
			// 右
			12,13,14,
			14,13,15,
			// 上
			16,17,18,
			18,17,19,
			// 下
			20,21,22,
			22,21,23,
		};


		Camera* camera = nullptr;

	public:		// メンバ関数

		~DirectXCommon();

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="win"></param>
		void Initialize(WinApp* win);

		/// <summary>
		/// 後処理
		/// </summary>
		void Finalize();

		/// <summary>
		/// 描画前準備
		/// </summary>
		void PreDraw();

		/// <summary>
		/// 描画後処理
		/// </summary>
		void PostDraw();

		/// <summary>
		/// 描画
		/// </summary>
		void ClearRenderTarget();

		// getter
		ID3D12Device* GetDevice() { return _dev.Get(); }
		ID3D12GraphicsCommandList* GetCommandList() { return _cmdList.Get(); }

		// imgui用のヒープ生成
		ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeapForImgui();
		ComPtr<ID3D12DescriptorHeap> GetHeapForImgui();

	private:	// メンバ関数
		/// <summary>
		/// DXGIデバイス初期化
		/// </summary>
		/// <returns>成否</returns>
		bool InitializeDXGIDevice();

		/// <summary>
		/// コマンド関連初期化
		/// </summary>
		/// <returns>成否</returns>
		bool InitializeCommand();

		/// <summary>
		/// スワップチェーンの生成
		/// </summary>
		/// <returns>成否</returns>
		bool CreateSwapChain();

		/// <summary>
		/// レンダーターゲット生成
		/// </summary>
		/// <returns>成否</returns>
		bool CreateFinalRenderTargets();

		/// <summary>
		/// フェンス生成
		/// </summary>
		/// <returns>成否</returns>
		bool CreateFence();

		/// <summary>
		/// 深度バッファの生成
		/// </summary>
		/// <returns>成否</returns>
		bool GenerateDepthBuffer();

		/// <summary>
		/// 深度バッファビューの生成
		/// </summary>
		/// <returns>成否</returns>
		bool GenerateDepthBufferView();


		/// <summary>
		/// 頂点バッファの生成
		/// </summary>
		/// <returns>成否</returns>
		bool GenerateVertexBuffer();

		/// <summary>
		/// 頂点情報のマップ
		/// </summary>
		/// <returns>成否</returns>
		bool MapVertexBuffer();

		/// <summary>
		/// 頂点バッファビューの作成
		/// </summary>
		void CreateVertexBufferView();

		/// <summary>
		/// インデックスバッファの生成
		/// </summary>
		/// <returns>成否</returns>
		bool GenerateIndexBuffer();

		/// <summary>
		/// インデックス情報のマップ
		/// </summary>
		/// <returns>成否</returns>
		bool MapIndexBuffer();

		/// <summary>
		/// インデックスバッファビューの作成
		/// </summary>
		void CreateIndexBufferView();

		/// <summary>
		/// テクスチャバッファの生成
		/// </summary>
		/// <returns>成否</returns>
		bool GenerateTextureBuffer();

		/// <summary>
		/// 定数バッファビューの生成
		/// </summary>
		/// <returns>成否</returns>
		bool GenerateConstBufferView();

		/// <summary>
		/// 行列計算
		/// </summary>
		void CalculateMat();

		/// <summary>
		/// シェーダーリソースビューの作成
		/// </summary>
		/// <returns>成否</returns>
		bool CreateTextureShaderResourceView();

		/// <summary>
		/// シェーダーの読み込みと生成
		/// </summary>
		/// <returns>成否</returns>
		bool LoadShader();

		/// <summary>
		/// グラフィックパイプラインステートオブジェクトの生成
		/// </summary>
		/// <returns>成否</returns>
		bool CreateGPipelineStateObject();

		/// <summary>
		/// ルートパラメーターの設定
		/// </summary>
		void SetUpRootParameter();

		/// <summary>
		/// サンプラーの設定
		/// </summary>
		/// <returns>成否</returns>
		void SetUpSampler();

		/// <summary>
		/// ルートシグネチャの作成
		/// </summary>
		/// <returns>成否</returns>
		bool CreatRootSignature();

		/// <summary>
		/// ビューポートの設定
		/// </summary>
		void SetUpViewport();

		/// <summary>
		/// シザー矩形の設定
		/// </summary>
		void SetUpScissorrect();
	};
}