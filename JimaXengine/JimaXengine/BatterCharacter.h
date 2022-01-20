#pragma once
#include "GameObject/GameObject.h"

namespace JimaXengine
{
	class Camera;
	class Object3d;

	class BatterCharacter : public GameObject
	{
	private:
		std::unique_ptr <Object3d> characterObject;

		std::unique_ptr <Object3d> batObject;
		Vector3 batScale;

	public:
		BatterCharacter(Camera* camera);
		~BatterCharacter();

		void Initialize()override;
		void Update()override;
		void Draw()override;
		GameObject::TYPE GetType()override;
		void DrawImGui()override;

		// setter
		void SetPosition(Vector3 position) { pos = position; }
		/// <summary>
		/// キャラクターのアニメーションフレーム設定
		/// </summary>
		/// <param name="start">開始フレーム</param>
		/// <param name="end">終了フレーム</param>
		/// <param name="loop">ループするか</param>
		void SetAnimationFrame(const int start, const int end, bool loop = true, const int frametime = 1);

		// getter
		Object3d* GetCharacterObject() { return characterObject.get(); }
		Object3d* GetBatObject() { return batObject.get(); }
	};
}