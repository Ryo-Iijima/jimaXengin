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

		// �����������ڂ̋����i�ۉe�̃C���f�b�N�X�w��p�j
		int ballNumber;

		bool isHitPlayer = false;	// �v���C���[�ɓ�������
		bool isHitEnemy = false;	// �G�ɓ�������

	private:
		/// <summary>
		/// ���̋���
		/// </summary>
		void Move();

		/// <summary>
		/// ���̃I�u�W�F�N�g�Ƃ̐ڐG����
		/// </summary>
		void HitCheck();

		/// <summary>
		/// ���������ł��邱�Ƃ��{�X�ɒm�点��
		/// </summary>
		void InformBoss();

	public:
		Target(Camera* camera, Vector3 pos = { 0,0,0 }, Vector3 vel = { 0,0,-1 }, Vector3 gool = { 0, 0, -15 }, float speed = 0.1f, int ballnumber = 0);
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