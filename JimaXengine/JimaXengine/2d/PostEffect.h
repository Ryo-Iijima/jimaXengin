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

		//// �C���X�^���X�Ԃŋ��L�������ϐ� ////
		
		static DirectXCommon* dxCommon;
		
		// SRV�p�f�X�N���v�^�q�[�v
		static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapSRV;
		
		// RTV�p�f�X�N���v�^�q�[�v
		static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapRTV;
		
		// DSV�p�f�X�N���v�^�q�[�v
		static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapDSV;

		// ��ʃN���A�J���[
		static const float clearColor[4];

		// �O���t�B�b�N�X�p�C�v���C��
		static std::map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipelines;

		// ���[�g�V�O�l�`��
		static Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;


		//// �e�C���X�^���X���ƂɎ����Ă��Ăق����ϐ� ////

		// ���_�o�b�t�@
		Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;

		// �萔�o�b�t�@
		Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;

		// �萔�o�b�t�@�r���[
		D3D12_VERTEX_BUFFER_VIEW vBView;

		// �e�N�X�`���o�b�t�@
		Microsoft::WRL::ComPtr<ID3D12Resource> texBuff[2];
		
		// �[�x�o�b�t�@
		Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;


	private:

		void DrawCommands(const std::string& filename, const std::string& registername, const std::string& blendtype);

		// ���[�g�V�O�l�`���쐬
		void CreateRootSignature();

		// �e�N�X�`������
		void GenerateTexture();

		// SRV�쐬
		void CreateSRV();
		
		// RTV�쐬
		void CreateRTV();
		
		// �[�x�o�b�t�@����
		void GenerateDepthBuffer();
		
		// DSV�쐬
		void CreateDSV();

		// �p�C�v���C������
		void CreateGraphicsPiplineState(const std::string& vsfilename, const std::string& psfilename, const std::string& registername);

	public:
		PostEffect();

		void Initialize(DirectXCommon* dxcommon);

		void Draw();

		// �`��O����
		void PreDrawScene();

		// �`��㏈��
		void PostDrawScene();

	};

}