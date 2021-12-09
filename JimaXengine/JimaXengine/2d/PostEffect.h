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

		//�C���X�^���X�Ԃŋ��L�������ϐ�
		
		static DirectXCommon* dxCommon;

		// SRV�p�f�X�N���v�^�q�[�v
		static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapSRV;

		//�e�C���X�^���X���ƂɎ����Ă��Ăق����ϐ�
		// �e�N�X�`���o�b�t�@
		Microsoft::WRL::ComPtr<ID3D12Resource> texBuff;

	private:
		void DrawCommands(const std::string& filename, const std::string& registername, const std::string& blendtype);

	public:
		PostEffect();

		void Initialize(DirectXCommon* dxcommon);

		void Draw();
	};

}