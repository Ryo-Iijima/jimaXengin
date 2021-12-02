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
			Vector2 uvPos;
			Vector2 vel;
			Vector2 scale;
		};

		// �{�^���֘A
		std::unique_ptr<Object2d> pushStartTex;
		std::unique_ptr<Object2d> buttonFrontTex;
		Vector2 pushStartTexPos;
		Vector2 pushStartTexScale;
		const int damageTime = 10;	// �_�Ŏ���
		int damageCount = 1;		// �_�ŃJ�E���g�p
		int increase = 1;
		bool selected = false;

		// �^�C�g��
		std::unique_ptr<Object2d> titleTex;
		Vector2 titleTexPos;
		Vector2 titleTexScale;

		Bg bg;
		Bg band;

		Sound* sound = nullptr;

		Vector2 texPos = { 0,0 };

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