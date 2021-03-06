#pragma once
#include "fbxsdk.h"

#pragma warning(disable:26451 26495 26812 )

#include <d3d12.h>
#include <d3dx12.h>

#include <string>

#include "Model.h"

namespace JimaXengine
{
	class FbxLoader
	{
	private:	// エイリアス
		using string = std::string;

	private:

		// 頂点数数える用
		struct Int2
		{
			Int2() {};
			Int2(int x, int y)
			{
				this->x = x;
				this->y = y;
			};

			int x;
			int y;
		};

	public:		// 定数
		// ファイルパスの前半の共通部分
		static const string BASE_DIRECTORY_MODEL;
		static const string BASE_DIRECTORY_TEX;
		// マテリアル用デフォルトテクスチャ
		static const string DEFAULT_TEXTURE_FILENAME;

	private:	// 変数
		ID3D12Device* device = nullptr;
		FbxManager* fbxmanager = nullptr;
		FbxImporter* fbxImporter = nullptr;

	private:	// シングルトンの決めごと
		// コンストラクタをprivateにして
		// コピーと代入を禁止する
		FbxLoader() = default;
		~FbxLoader() = default;
		FbxLoader(const FbxLoader& obj) = delete;
		void operator=(const FbxLoader& obj) = delete;

		static FbxLoader* instance;

		int vertexCount = 0;
		// ポリゴン番号、頂点番号保存用
		std::vector<Int2> data;
		// コントロールポイントの番号
		std::vector<int> verticesControlPointNum;

	private:	// 関数

		/// <summary>
		/// ノード構成を再帰的に解析
		/// </summary>
		/// <param name="model">読み込み先のモデルオブジェクト</param>
		/// <param name="fbxNode">解析対象のノード</param>
		void ParseNodeRecursive(Model* model, FbxNode* fbxNode, Node* parent = nullptr);

		/// <summary>
		/// メッシュ読み取り
		/// </summary>
		/// <param name="model">読み込み先のモデルオブジェクト</param>
		/// <param name="fbxNode">解析対象のノード</param>
		void ParseMesh(Model* model, FbxNode* fbxNode);

		// メッシュ読み取り用のサブ関数
		// 頂点読み取り
		void ParseMeshVertices(Model* model, FbxMesh* fbxMesh);
		// 面情報読み取り
		void ParseMeshFaces(Model* model, FbxMesh* fbxMesh);
		// マテリアル読み取り
		void ParseMaterial(Model* model, FbxNode* fbxNode);
		// ディレクトリを含んだファイルパスからファイル名だけを抽出する
		std::string ExtractFileName(const std::string& path);
		// テクスチャ読み取り
		void LoadTexture(Model* model, const std::string& fullpath);
		// スキニング情報の読み取り
		void ParseSkin(Model* model, FbxMesh* fbxMesh);

		// モデル保持用
		std::unordered_map<std::string, Model*> models;

	public:		// 関数
		/// <summary>
		/// シングルトンインスタンスの取得
		/// </summary>
		/// <returns>インスタンス</returns>
		static FbxLoader& GetInstance();

		void Initialize(ID3D12Device* device);

		void Finalize();

		// 先読み用モデルロード
		void LoadModelFromFiletoBuff(const string& modelName);

		// モデルバッファからデータを取得
		Model* GetFbxModel(const std::string& modelName);

		/// <summary>
		/// FBX行列をXMMATRIXに変換
		/// </summary>
		/// <param name="dst">書き込み先</param>
		/// <param name="src">元となるFBX行列</param>
		static void ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, FbxAMatrix& src);

	};
}