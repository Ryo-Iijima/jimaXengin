#include "../GameObject/GameObject.h"

namespace JimaXengine
{
	class Model;
	class Object3d;
	class Target;

	class StrikeMark : public GameObject
	{
	private:
		std::unique_ptr<Object3d> object;

		// �\����\�����鋅
		Target* pTarget;

		// �\���͈͂̒��S
		Vector3 center;
		// �\���͈͂̕�
		float rad;

		// �ړ�����
		const float upLimit = 6.0f;
		const float downLimit = 0.6f;
		const float sideLimit = 7.5f;

		bool isPassed;

		// �߂Â��ɂ�T�C�Y���i��
		void ShrinkSize();

		// ���̈ʒu���g���[�X
		void TraceBall();

		// �ʉ߂������`�F�b�N
		void PassCheck();

		// ������������`�F�b�N
		void DeadConditions();

		// �ʒu��͈͓��Ɏ��߂�
		void WithinRange();

	public:
		StrikeMark(Camera* pcamera, Target* ptarget);
		~StrikeMark();

		void Initialize() override;
		void Update() override;
		void Draw() override;

		TYPE GetType() override;
		RENDER_TYPE GetRenderType() override;
		void DrawImGui() override;
	};
}
