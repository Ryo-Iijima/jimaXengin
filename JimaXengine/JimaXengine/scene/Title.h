#pragma once
#include <memory>
#include "Scene.h"
#include "../math/Vector4.h"
#include "../2d/Object2d.h"

namespace JimaXengine
{
	class Object3d;
	class Object2d;
	class Sound;

	class Title : public Scene
	{
	private:
		struct Bg
		{
			Object2d obj2d;
			Vector2 pos;
			Vector2 uvPos;
			Vector2 vel;
			Vector2 scale;
		};

		// ボタン関連
		std::unique_ptr<Object2d> pushStartTex;
		std::unique_ptr<Object2d> buttonFrontTex;
		Vector2 pushStartTexPos;
		Vector2 pushStartTexScale;
		const int damageTime = 10;	// 点滅時間
		int damageCount = 1;		// 点滅カウント用
		int increase = 1;
		bool selected = false;

		// タイトル
		std::unique_ptr<Object2d> titleTex;
		Vector2 titleTexPos;
		Vector2 titleTexScale;

		Bg bg;
		Bg band;

		Vector2 texPos = { 0,0 };

	private:
		// 背景スクロール
		void BgScroll();

		// 画面遷移関連

		std::unique_ptr<Object2d> fadeTex;
		Vector4 fadeTexColor;
		// 遷移開始用
		bool stagingStart = false;
		// 遷移演出終了したか
		bool stagingEnd = false;
		// 簡易画面転換
		void SimpleStaging();

	public:
		Title(WinApp* winapp);
		~Title();
		void Initialize() override;
		void Update() override;
		void Draw() override;

	};
}