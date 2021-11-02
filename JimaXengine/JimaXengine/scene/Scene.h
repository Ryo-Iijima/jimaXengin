#pragma once
#include <string>
#include "../general/Input.h"

class WinApp;
namespace JimaXengine
{
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

		WinApp* winApp;
		bool isEnd;
		bool isAllEnd;
		std::string nowScene;
		std::string nextScene;
	};
}