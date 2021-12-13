#pragma once
#include "scene/Scene.h"
#include "GameObject/GameObjectManager.h"

namespace JimaXengine
{
	class TestScene : public Scene
	{
	private:
		std::unique_ptr<Camera> camera;
		GameObjectManager* oManager = nullptr;
		Light* light = nullptr;

	public:
		TestScene();
		~TestScene();
		void Initialize() override;
		void Update() override;
		void Draw() override;

	};
}
