/// �ǂݍ��񂾃��f����������N���X

#pragma once
#include <string>
#include <DirectXMath.h>
#include <vector>
#include <DirectXTex.h>

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
public:		// �t�����h�N���X

	friend class FbxLoader;

public:		// �T�u�N���X

	// FBX���_�f�[�^�\����
	struct FBXVertexData
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
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
};