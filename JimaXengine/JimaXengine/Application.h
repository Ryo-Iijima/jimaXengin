#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"

class Application
{
private:	// �ϐ�
	// �ėp�@�\
	WinApp* winApp = nullptr;
	DirectXCommon* dxCommon = nullptr;


private:
	// �R���X�g���N�^��private�ɂ���
	// �R�s�[�Ƒ�����֎~����
	Application() = default;
	~Application() = default;

	Application(const Application&) = delete;
	void operator=(const Application&) = delete;

public:
	/// <summary>
	/// Application�̃V���O���g���C���X�^���X�𓾂�
	/// </summary>
	/// <returns>�C���X�^���X</returns>
	static Application& Instance();

	void Initialize();
	
	void Finalize();

	void Run();

};

