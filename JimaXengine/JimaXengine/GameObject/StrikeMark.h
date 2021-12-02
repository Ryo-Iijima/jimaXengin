#include "GameObject.h"

namespace JimaXengine
{
	class Model;
	class Object3d;

	class StrikeMark : public GameObject
	{
	private:
		Model* model = nullptr;
		std::unique_ptr<Object3d> object;

	public:
		void Initialize() override;
		void Update() override;
		void Draw() override;

		TYPE GetType() override;
		RENDER_TYPE GetRenderType() override;
		void DrawImGui() override;
	};
}
