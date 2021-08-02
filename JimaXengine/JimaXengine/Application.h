#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "3d/Object3d.h"
#include "general/Sound.h"
#include "general/imGuiDevice.h"
#include "general/FPSManager.h"
#include "general/Input.h"
#include "scene/SceneManager.h"

class Application
{
private:

	WinApp* winApp = nullptr;
	DirectXCommon* dxCommon = nullptr;
	FPSManager* fpsManager = nullptr;
	Input* input = nullptr;
	Sound* sound = nullptr;
	SceneManager* sceneManager = nullptr;
	ImGuiDevice* imguiDev = nullptr;

private:

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

