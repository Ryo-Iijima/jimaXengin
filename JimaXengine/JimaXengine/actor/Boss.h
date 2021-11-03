#pragma once
#include "../GameObject/GameObject.h"


namespace JimaXengine
{	
	class Camera;
	class Model;
	class Object3d;

	class Boss : public GameObject
	{
	private:
		Model* model;
		std::unique_ptr<Object3d> object;

		int hp;					// ヒットポイント
		bool damaged = false;	// ダメージ点滅用
		int count = 10;			// 点滅時間
		int i = 0;				// 点滅カウント用

	public:
		Boss(Camera* camera);
		~Boss();
		void Initialize()override;
		void Update()override;
		void Draw()override;

		void Dead()override;
		void Delete()override;
		GameObject::TYPE GetType()override;
		GameObject::RENDER_TYPE GetRenderType()override;
		void DrawImGui()override;

		void Damage();	// ダメージを食らう
	};
}