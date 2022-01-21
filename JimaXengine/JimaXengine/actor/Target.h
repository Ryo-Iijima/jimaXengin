#pragma once
#include "../GameObject/GameObject.h"
#include "Player.h"

namespace JimaXengine
{
	class Model;
	class Object3d;
	class StrikeMark;

	class Target : public GameObject
	{
	private:
		Object3d* object = nullptr;

		// 重力もどき
		Vector3 gravity = { 0, -0.005f, 0 };

		// 

		// 跳ね返した後か
		bool reflection = false;

		// 当たった後の方向調整用
		Player::Racket* hitRacket = nullptr;

		// コース予測表示を出したか
		bool markInserted = false;

		Vector3 gool;

		// 軌跡の色
		const Vector3 normalLocusColor = { 1, 0, 0 };
		const Vector3 reflectLocusColor = { 0, 0, 1 };
		Vector3 locusColor;	

		bool isHitPlayer;	// プレイヤーに当たった
		bool isHitEnemy;	// 敵に当たった

	private:
		void Move();
		void HitCheck();

	public:
		Target(Camera* camera, Vector3 pos = { 0,0,0 }, Vector3 vel = { 0,0,-1 }, Vector3 gool = {0, 0, -15}, float speed = 0.1f);
		~Target();

		void Initialize()override;
		void Update()override;
		void Draw()override;
		GameObject::TYPE GetType()override;
		void DrawImGui()override;

		bool GetReflection(){return reflection; }
		Vector3 GetGool() { return gool; }
	};
}