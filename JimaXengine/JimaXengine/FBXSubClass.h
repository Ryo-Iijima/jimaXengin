#pragma once

static const int MAX_BONE_INDOCES = 4;

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
