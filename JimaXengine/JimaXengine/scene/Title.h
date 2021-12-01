#pragma once
#include "Scene.h"
#include <memory>
#include "../2d/Object2d.h"
#include "../general/Sound.h"

namespace JimaXengine
{
	class Model;
	class Object3d;
	class Object2d;

	class Title : public Scene
	{
	private:
		struct Bg
		{
			Object2d obj2d;
			Vector2 pos;
			Vector2 vel;
			Vector2 scale;
		};

		std::unique_ptr<Object2d> pushStartTex;
		std::unique_ptr<Object2d> buttonFrontTex;
		Vector2 pushStartTexPos;
		Vector2 pushStartTexScale;
		const int damageTime = 10;	// 点滅時間
		int damageCount = 1;		// 点滅カウント用
		int increase = 1;
		bool selected = false;

		std::unique_ptr<Object2d> titleTex;
		Vector2 titleTexPos;

		Bg bg[4];
		Bg bgBand[2];

		Sound* sound = nullptr;
	private:
		void BgScroll();

	public:
		Title(WinApp* winapp);
		~Title();
		void Initialize() override;
		void Update() override;
		void Draw() override;

	};
}