#pragma once
#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <Xinput.h>
#include <DirectXMath.h>

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "xinput.lib")

class WinApp;
class Input
{
private:
	static WinApp* pWindow;

	//�L�[�{�[�h
	static BYTE currentKey[256];
	static BYTE prevKey[256];
	static IDirectInput8* dinput;
	static IDirectInputDevice8* devkeyboard;

	//�}�E�X
	static DIMOUSESTATE currentMouse;
	static DIMOUSESTATE prevMouse;
	static IDirectInputDevice8* devmouse;
	static Vector2 currentMousePos;
	static Vector2 prevMousePos;

	//�p�b�h
	static XINPUT_STATE xinputCurrent;
	static XINPUT_STATE xinputPrev;
	static int leftDeadZone;
	static int rightDeadZone;
public:
	static void Initialize(WinApp* window);
	static void Update();
	static void SetDeadZone(int lDeadZone, int rDeadZone);
	static void Finalize();

	//�L�[�{�[�h
	static bool KeyPress(const int& keys);
	static bool KeyTrigger(const int& keys);
	static bool KeyRelease(const int& keys);

	//�}�E�X
	static bool MouseButtonPress(const int& keys);
	static bool MouseButtonTrigger(const int& keys);
	static bool MouseButtonHold(const int& keys);
	static bool MouseButtonRelease(const int& keys);

	static int MouseXMove();
	static int MouseYMove();
	static int MouseWheelMove();

	static void SetMousePos(Vector2 pos);
	static Vector2 GetCurrentMousePos();
	static Vector2 GetPrevMousePos();

	//PAD(XInput)
	//�{�^��(�L�[�w��)
	static bool PadButtonPress(const int& keys);
	static bool PadButtonTrigger(const int& keys);
	static bool PadButtonRelease(const int& keys);

	//���g���K�[
	static bool PadLTPress();
	static bool PadLTTrigger();
	static bool PadLTRelease();
	//�E�g���K�[
	static bool PadRTPress();
	static bool PadRTTrigger();
	static bool PadRTRelease();

	//���g���K�[(�[�x)
	//�E�g���K�[(�[�x)

	//���X�e�B�b�N(bool)
	static bool PadLeftStickRight();
	static bool PadLeftStickLeft();
	static bool PadLeftStickUp();
	static bool PadLeftStickDown();
	//�E�X�e�B�b�N(bool)
	static bool PadRightStickRight();
	static bool PadRightStickLeft();
	static bool PadRightStickUp();
	static bool PadRightStickDown();

	//���X�e�B�b�N�g���K�[(bool)
	static bool PadLeftStickRightTrigger();
	static bool PadLeftStickLeftTrigger();
	static bool PadLeftStickUpTrigger();
	static bool PadLeftStickDownTrigger();
	//�E�X�e�B�b�N�g���K�[(bool)
	static bool PadRightStickRightTrigger();
	static bool PadRightStickLeftTrigger();
	static bool PadRightStickUpTrigger();
	static bool PadRightStickDownTrigger();

	//���X�e�B�b�N(float)
	static float PadLeftStickPercentageX();
	static float PadLeftStickPercentageY();
	//�E�X�e�B�b�N(float)
	static float PadRightStickPercentageX();
	static float PadRightStickPercentageY();

	//���X�e�B�b�N(�x�N�g��)
	static Vector2 PadLeftStickVector();
	//�E�X�e�B�b�N(�x�N�g��)
	static Vector2 PadRightStickVector();
};