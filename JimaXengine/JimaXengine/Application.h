#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "3d/Object3d.h"
#include "general/Sound.h"
#include "general/imGuiDevice.h"
#include "general/FPSManager.h"
#include "general/Input.h"
#include "scene/SceneManager.h"
#include "2d/PostEffect.h"
#include <memory>

namespace JimaXengine
{
	class Application
	{
	private:
		std::unique_ptr<WinApp> winApp;
		
		std::unique_ptr<DirectXCommon> dxCommon;
		
		std::unique_ptr<FPSManager> fpsManager;
		
		std::unique_ptr<Input> input;
		
		std::unique_ptr<SceneManager> sceneManager;
		
		ImGuiDevice imguiDev;
		
		std::unique_ptr<PostEffect> postEffect;

	private:
		// ƒVƒ“ƒOƒ‹ƒgƒ“
		Application() = default;
		~Application() = default;
		Application(const Application&) = delete;
		void operator=(const Application&) = delete;

	public:

		static Application& GetInstance();
		void Initialize();
		void Finalize();
		void Run();

	};

}