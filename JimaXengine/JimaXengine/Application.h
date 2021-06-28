#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"

class Application
{
private:	// 変数
	// 汎用機能
	WinApp* winApp = nullptr;
	DirectXCommon* dxCommon = nullptr;


private:
	// コンストラクタをprivateにして
	// コピーと代入を禁止する
	Application() = default;
	~Application() = default;

	Application(const Application&) = delete;
	void operator=(const Application&) = delete;

public:
	/// <summary>
	/// Applicationのシングルトンインスタンスを得る
	/// </summary>
	/// <returns>インスタンス</returns>
	static Application& Instance();

	void Initialize();
	
	void Finalize();

	void Run();

};

