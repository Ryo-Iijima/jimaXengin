#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

#include "WinApp.h"
#include "DirectXCommon.h"
#include "Application.h"

#include "fbxsdk.h"

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

	FbxManager* fbxmanager = FbxManager::Create();

	auto& app = Application::Instance();

	app.Initialize();

	app.Run();

	app.Finalize();

	return 0;
}
