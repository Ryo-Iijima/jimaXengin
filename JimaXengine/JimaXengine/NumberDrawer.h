#pragma once
#include "2d/Object2d.h"
#include <memory>
#include <vector>

namespace JimaXengine
{
	class NumberDrawer
	{
	private:
		int& number;								// 描画したい数値
		int digit;									// 桁数
		std::vector<std::unique_ptr<Object2d>> tex;	// 桁数分のスプライト管理
		std::vector<int> eachNum;					// 各桁の数値

		Object2d::DrawTextureData data;

		Vector2 referrencePos;		// 基準位置

	public:
		NumberDrawer(int& number, int digit, Object2d::DrawTextureData data);

		void Initialize();

		void Update();

		void Draw();

	};
}