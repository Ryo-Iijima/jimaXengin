#pragma once
#include <memory>
#include "Scene.h"
#include "../2d/Object2d.h"
#include "../NumberDrawer.h"

namespace JimaXengine
{
	class Object3d;
	class Object2d;

	class End : public Scene
	{
	private:
		// Šî”Õ
		std::unique_ptr<Object2d> bordTex;
		Object2d::DrawTextureData bordTexData;

		// ŠÔ
		int time;
		std::unique_ptr<NumberDrawer> timeTex;
		Object2d::DrawTextureData timeTexData;

		// ”í’e”
		int damage;
		std::unique_ptr<NumberDrawer> damageTex;
		Object2d::DrawTextureData damageTexData;

		// ‘Å—¦
		int battingAverage;
		std::unique_ptr<NumberDrawer> battingAverageTex;
		Object2d::DrawTextureData battingAverageTexData;

		// ‘‡
		int overallScore;
		std::unique_ptr<NumberDrawer> overallScoreTex;
		Object2d::DrawTextureData overallScoreTexData;

		// ƒ‰ƒ“ƒN
		std::unique_ptr<Object2d> stampTex;
		Object2d::DrawTextureData stampTexData;

	public:
		End(WinApp* winapp);
		~End();
		void Initialize() override;
		void Update() override;
		void Draw() override;

	};
}