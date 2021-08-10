#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

#include "WinApp.h"
#include "DirectXCommon.h"
#include "Application.h"

#include "lib/fbx_sdk/include/fbxsdk.h"


#include "hidapi/hidapi.h"

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

	hid_device_info* device = hid_enumerate(0, 0);

	while (device)
	{
		hid_device* dev = hid_open(device->vendor_id, device->product_id, device->serial_number);
 		printf("\nproduct_id: %ls", device->product_string);
 		device = device->next;
	}
	hid_free_enumeration(device);


	////////////////////////////////////////////////////////////////////////////////////

	FbxManager* fbxManager = FbxManager::Create();

	auto& app = Application::GetInstance();

	app.Initialize();

	app.Run();

	app.Finalize();

	return 0;
}
