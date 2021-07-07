/// imGui使うためのクラス

#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"

class imGuiDevice
{
private:
	WinApp* winApp = nullptr;
	DirectXCommon* dxCommon = nullptr;

public:

	void Initialize(WinApp* winApp, DirectXCommon* dxCommon);

	void Update();

	void PreDraw();

	void Draw();
};

