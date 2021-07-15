// main����@�\������Ă���
// �V���O���g���̃N���X

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
private:	// �ϐ�
	// �ėp�@�\
	WinApp* winApp = nullptr;
	DirectXCommon* dxCommon = nullptr;
	
	FPSManager* fpsManager = nullptr;

	Model* model = nullptr;
	Object3d* object = nullptr;

	Input* input = nullptr;

	Sound* sound = nullptr;

	ImGuiDevice* imguiDev = nullptr;

private:	// �V���O���g���̂��߂̂���
	// �R���X�g���N�^��private�ɂ���
	// �R�s�[�Ƒ�����֎~����
	Application() = default;
	~Application() = default;
	Application(const Application&) = delete;
	void operator=(const Application&) = delete;

public:		// �֐�
	/// <summary>
	/// Application�̃V���O���g���C���X�^���X�𓾂�
	/// </summary>
	/// <returns>�C���X�^���X</returns>
	static Application& GetInstance();

	void Initialize();
	
	void Finalize();

	void Run();

};

