/// �ǂݍ��񂾃��f����������N���X

#pragma once
#include <string>
#include <DirectXMath.h>
#include <vector>
#include <DirectXTex.h>

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>

#include <fbxsdk.h>

/// <summary>
/// �m�[�h�P���̏��\����
/// </summary>
struct Node
{
	// ���O
	std::string name;
	// ���[�J���X�P�[��
	DirectX::XMVECTOR scaling = { 1,1,1,0 };
	// ���[�J����]�p
	DirectX::XMVECTOR rotation = { 0,0,0,0 };
	// ���[�J���ړ�
	DirectX::XMVECTOR translation = { 0,0,0,1 };
	// ���[�J���ό`�s��
	DirectX::XMMATRIX transform;
	// �O���[�o���ό`�s��
	DirectX::XMMATRIX globalTransform;
	// �e�m�[�h
	Node* parent = nullptr;
};

class Model
{
private:	// �G�C���A�X
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;

	using string = std::string;
	template <class T>using vector = std::vector<T>;

	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

public:		// �t�����h�N���X

	friend class FbxLoader;

public:		// �萔
	// �{�[���C���f�b�N�X�̍ő吔
	static const int MAX_BONE_INDOCES = 4;

public:		// �T�u�N���X

	// FBX���_�f�[�^�\����
	struct FBXVertexData
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
		UINT boneIndex[MAX_BONE_INDOCES];	// �{�[���ԍ�
		float boneWeight[MAX_BONE_INDOCES]; // �{�[���d��
	};

	// FBX�}�e���A���f�[�^�\����
	struct FBXMaterialData
	{
		// �A���r�G���g�W��
		DirectX::XMFLOAT3 ambient = { 1,1,1 };
		// �f�B�t���[�Y�W��
		DirectX::XMFLOAT3 diffuse = { 1,1,1 };
		// �e�N�X�`�����^�f�[�^
		DirectX::TexMetadata metadata = {};
		// �X�N���b�`�C���[�W
		DirectX::ScratchImage scrachImg = {};
	};

	// �{�[���\����
	struct Bone
	{
		std::string name;
		DirectX::XMMATRIX invInitialPose;	// �����p���̋t�s��		
		FbxCluster* fbxCluster;				// �N���X�^�[�iFBX���̃{�[�����j
		Bone(const std::string& name)
		{
			this->name = name;
		}
	};


private:	//�ϐ�
	// ���f����
	std::string name;
	// �m�[�h�z��
	std::vector<Node> nodes;
	// ���b�V�������m�[�h
	Node* meshNode = nullptr;
	// ���_�f�[�^�z��
	std::vector<FBXVertexData> vertices;
	// ���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices;
	// �}�e���A���f�[�^
	FBXMaterialData materialData;
	// �{�[���z�u
	std::vector<Bone> bones;
	// FBX�V�[��
	FbxScene* fbxScene = nullptr;

	// �e��o�b�t�@�E�r���[
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff;
	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texbuff;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	// SRV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	
public:		// �֐�
	~Model();

	// �o�b�t�@����
	void CreateBuffers(ID3D12Device* device);

	void Draw(ID3D12GraphicsCommandList* cmdList);

	// getter
	// ���f���̕ό`�s��擾
	const XMMATRIX& GetModelTransform() { return meshNode->globalTransform; }
	std::vector<Bone>& GetBonse() { return bones; }
	FbxScene* GetFbxScene() { return fbxScene; }
};