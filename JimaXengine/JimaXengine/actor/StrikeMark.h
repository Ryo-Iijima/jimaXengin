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

		// —\‘ª‚ð•\Ž¦‚·‚é‹…
		Target* pTarget;

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
