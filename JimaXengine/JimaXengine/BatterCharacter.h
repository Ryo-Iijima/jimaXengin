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
		/// �L�����N�^�[�̃A�j���[�V�����t���[���ݒ�
		/// </summary>
		/// <param name="start">�J�n�t���[��</param>
		/// <param name="end">�I���t���[��</param>
		/// <param name="loop">���[�v���邩</param>
		void SetAnimationFrame(const int start, const int end, bool loop = true, const int frametime = 1);

		// getter
		Object3d* GetCharacterObject() { return characterObject.get(); }
		Object3d* GetBatObject() { return batObject.get(); }
	};
}