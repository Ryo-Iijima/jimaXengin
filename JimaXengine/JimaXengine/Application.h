// mainから機能を避けてきた
// シングルトンのクラス

#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "3d/Object3d.h"
#include "general/Sound.h"
#include "general/imGuiDevice.h"
#include "general/FPSManager.h"
#include "general/Input.h"

class Application
{
private:	// 変数
	// 汎用機能
	WinApp* winApp = nullptr;
	DirectXCommon* dxCommon = nullptr;
	
	FPSManager* fpsManager = nullptr;

	Model* model = nullptr;
	Object3d* object = nullptr;

	Input* input = nullptr;

	Sound* sound = nullptr;

	ImGuiDevice* imguiDev = nullptr;

private:	// シングルトンのためのこと
	// コンストラクタをprivateにして
	// コピーと代入を禁止する
	Application() = default;
	~Application() = default;
	Application(const Application&) = delete;
	void operator=(const Application&) = delete;

public:		// 関数
	/// <summary>
	/// Applicationのシングルトンインスタンスを得る
	/// </summary>
	/// <returns>インスタンス</returns>
	static Application& GetInstance();

	void Initialize();
	
	void Finalize();

	void Run();

};

