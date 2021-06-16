#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

#include "WinApp.h"
#include "DirectXCommon.h"

//// @drief �R���\�[����ʂɃt�H�[�}�b�g�t���������\��
//// @param format �t�H�[�}�b�g�i%d�Ƃ�%f�Ƃ��́j
//// @param �ϒ�����
//// @remarks ���̊֐��̓f�o�b�O�p�ł�
//void DebugOutputFormatStrings(const char* format, ...) 
//{
//#ifdef _DEBUG
//	va_list valist;
//	va_start(valist, format);
//	printf(format, valist);
//#endif // _DEBUG
//}

#ifdef _DEBUG
int main()
{
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#endif // _DEBUG
	//DebugOutputFormatStrings("Show window test.");
	//getchar();

	// �ėp�@�\
	WinApp* winApp = nullptr;
	DirectXCommon* dxCommon = nullptr;

	// �Q�[���E�B���h�E�̍쐬
	winApp = new WinApp();
	winApp->CreateGameWindow();

	// DirectX����������
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);


	///////////////////////////////
	// ���C�����[�v
	///////////////////////////////
	while (true)
	{
		// ���b�Z�[�W����
		if (winApp->ProcessMessage())
		{
			break;
		}

		// �`��O����
		dxCommon->PreDraw();

		// �`��㏈��
		dxCommon->PostDraw();

#pragma endregion
	}
	// �e��������
	delete dxCommon;

	// �Q�[���E�B���h�E�̔j��
	winApp->DestroyGameWindow();
	delete winApp;

	return 0;
}
