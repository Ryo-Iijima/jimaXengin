#pragma once
#include "2d/Object2d.h"
#include <memory>
#include <vector>

namespace JimaXengine
{
	class NumberDrawer
	{
	private:
		int& number;								// �`�悵�������l
		int digit;									// ����
		std::vector<std::unique_ptr<Object2d>> tex;	// �������̃X�v���C�g�Ǘ�
		std::vector<int> eachNum;					// �e���̐��l

		Object2d::DrawTextureData data;

		Vector2 referrencePos;		// ��ʒu

	public:
		NumberDrawer(int& number, int digit, Object2d::DrawTextureData data);

		void Initialize();

		void Update();

		void Draw();

	};
}