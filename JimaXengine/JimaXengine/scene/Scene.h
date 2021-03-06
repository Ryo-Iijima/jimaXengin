#pragma once
#include <string>
#include "../general/Input.h"
#include <memory>

namespace JimaXengine
{
	class WinApp;

	class Scene
	{
	public:

		Scene() = default;
		virtual ~Scene() = default;
		virtual void Initialize() = 0;
		virtual void Update() = 0;
		virtual void Draw() = 0;
		bool GetIsEnd() { return isEnd; }
		bool GetIsAllEnd() { return isAllEnd; }
		const std::string& GetNowScene() { return nowScene; }
		const std::string& GetNextScene() { return nextScene; }

	protected:

		virtual void ShutDown() { isEnd = true; }
		virtual void ShutDownAll() { isAllEnd = true; }

		std::unique_ptr<WinApp> winApp;
		bool isEnd;
		bool isAllEnd;
		std::string nowScene;
		std::string nextScene;
	};
}