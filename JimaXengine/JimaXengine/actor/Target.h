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

		// �d�͂��ǂ�
		Vector3 gravity = { 0, -0.005f, 0 };

		// 

		// ���˕Ԃ����ォ
		bool reflection = false;

		// ����������̕��������p
		Player::Racket* hitRacket = nullptr;

		// �R�[�X�\���\�����o������
		bool markInserted = false;

		Vector3 gool;

		// �O�Ղ̐F
		const Vector3 normalLocusColor = { 1, 0, 0 };
		const Vector3 reflectLocusColor = { 0, 0, 1 };
		Vector3 locusColor;	

		bool isHitPlayer;	// �v���C���[�ɓ�������
		bool isHitEnemy;	// �G�ɓ�������

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