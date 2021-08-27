#pragma once
#include "../math/Vector3.h"
//#include "ParticleParam.h"

class GameObject;
class ParticleManager;
class ParticleEmitter
{
public:
	ParticleEmitter();
	~ParticleEmitter();
	void Initialize(ParticleManager* manager);

	// �e���v���[�g�W�I���g���p�[�e�B�N��(�������m�F�A���Ăяo���p)
	void TempleteGeo(const Vector3& pos);
	// �e���v���[�g���b�V���p�[�e�B�N��(�������m�F�A���Ăяo���p)
	// �`����w�肵�����ꍇ�̂�(�d��)
	void TempleteObj(const Vector3& pos);

	// �G�ɓ����������A�₪�ӂ��U��
	void CrashBlock(const Vector3& pos);
	// �ړ����A�ΉԓI��(������)
	void Drift(const Vector3& pos, const Vector3& vec);
	// �r���{�[�h�̂悭�킩������
	void RandomEffect(const Vector3& pos);
	// �A�C�e���̎���ɏo���G�t�F�N�g
	void Moya(const Vector3& pos, const Vector3& color = Vector3::One);
	// ����ɏo��G�t�F�N�g
	void SphereEffect(const Vector3& pos, const Vector3& color = Vector3::One);
	// ����ɏo��G�t�F�N�g(�A�C�e�������O)
	void PreSphereEffect(const Vector3& pos, const Vector3& color = Vector3::One);
	// �A�C�e���擾���̃G�t�F�N�g
	void RandomCube(const Vector3& pos);
	// �A�C�e���擾���̃G�t�F�N�g
	void RandomCubeHoming(const Vector3& pos, GameObject* gmanager);
	// �Ǐ]
	void Homing(const Vector3& pos, GameObject* gmanager);
	// �h�����ӂ��ƂԓI��
	void AwayDrill(const Vector3& pos);
	// �v���C���[���G
	void InvincibleEffect(const Vector3& pos);

private:
	ParticleManager* manager = nullptr;
};