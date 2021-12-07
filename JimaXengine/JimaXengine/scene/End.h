#pragma once
#include <memory>
#include "Scene.h"
#include "../2d/Object2d.h"

namespace JimaXengine
{
	class Object3d;
	class Object2d;

	class End : public Scene
	{
	private:
		std::unique_ptr<Object2d> bordTex;
		Object2d::DrawTextureData bordTexData;

		std::unique_ptr<Object2d> timeTex;
		Object2d::DrawTextureData timeTexData;

		std::unique_ptr<Object2d> stampTex;
		Object2d::DrawTextureData stampTexData;

	public:
		End(WinApp* winapp);
		~End();
		void Initialize() override;
		void Update() override;
		void Draw() override;

	};
}