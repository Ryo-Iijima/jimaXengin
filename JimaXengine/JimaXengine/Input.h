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

	//キーボード
	static BYTE currentKey[256];
	static BYTE prevKey[256];
	static IDirectInput8* dinput;
	static IDirectInputDevice8* devkeyboard;

	//マウス
	static DIMOUSESTATE currentMouse;
	static DIMOUSESTATE prevMouse;
	static IDirectInputDevice8* devmouse;
	static Vector2 currentMousePos;
	static Vector2 prevMousePos;

	//パッド
	static XINPUT_STATE xinputCurrent;
	static XINPUT_STATE xinputPrev;
	static int leftDeadZone;
	static int rightDeadZone;
public:
	static void Initialize(WinApp* window);
	static void Update();
	static void SetDeadZone(int lDeadZone, int rDeadZone);
	static void Finalize();

	//キーボード
	static bool KeyPress(const int& keys);
	static bool KeyTrigger(const int& keys);
	static bool KeyRelease(const int& keys);

	//マウス
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
	//ボタン(キー指定)
	static bool PadButtonPress(const int& keys);
	static bool PadButtonTrigger(const int& keys);
	static bool PadButtonRelease(const int& keys);

	//左トリガー
	static bool PadLTPress();
	static bool PadLTTrigger();
	static bool PadLTRelease();
	//右トリガー
	static bool PadRTPress();
	static bool PadRTTrigger();
	static bool PadRTRelease();

	//左トリガー(深度)
	//右トリガー(深度)

	//左スティック(bool)
	static bool PadLeftStickRight();
	static bool PadLeftStickLeft();
	static bool PadLeftStickUp();
	static bool PadLeftStickDown();
	//右スティック(bool)
	static bool PadRightStickRight();
	static bool PadRightStickLeft();
	static bool PadRightStickUp();
	static bool PadRightStickDown();

	//左スティックトリガー(bool)
	static bool PadLeftStickRightTrigger();
	static bool PadLeftStickLeftTrigger();
	static bool PadLeftStickUpTrigger();
	static bool PadLeftStickDownTrigger();
	//右スティックトリガー(bool)
	static bool PadRightStickRightTrigger();
	static bool PadRightStickLeftTrigger();
	static bool PadRightStickUpTrigger();
	static bool PadRightStickDownTrigger();

	//左スティック(float)
	static float PadLeftStickPercentageX();
	static float PadLeftStickPercentageY();
	//右スティック(float)
	static float PadRightStickPercentageX();
	static float PadRightStickPercentageY();

	//左スティック(ベクトル)
	static Vector2 PadLeftStickVector();
	//右スティック(ベクトル)
	static Vector2 PadRightStickVector();
};