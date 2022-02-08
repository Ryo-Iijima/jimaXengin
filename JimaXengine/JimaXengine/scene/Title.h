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

		Vector2 texPos = { 0,0 };

	private:
		// �w�i�X�N���[��
		void BgScroll();

		// ��ʑJ�ڊ֘A

		std::unique_ptr<Object2d> fadeTex;
		Vector4 fadeTexColor;
		// �J�ڊJ�n�p
		bool stagingStart = false;
		// �J�ډ��o�I��������
		bool stagingEnd = false;
		// �ȈՉ�ʓ]��
		void SimpleStaging();

	public:
		Title(WinApp* winapp);
		~Title();
		void Initialize() override;
		void Update() override;
		void Draw() override;

	};
}