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

		// 予測を表示する球
		Target* pTarget;

		// 表示範囲の中心
		Vector3 center;
		// 表示範囲の幅
		float rad;

		// 移動制限
		const float upLimit = 6.0f;
		const float downLimit = 0.6f;
		const float sideLimit = 7.5f;

		bool isPassed;

		// 近づくにつれサイズを絞る
		void ShrinkSize();

		// 球の位置をトレース
		void TraceBall();

		// 通過したかチェック
		void PassCheck();

		// 消失する条件チェック
		void DeadConditions();

		// 位置を範囲内に収める
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
