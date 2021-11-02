#include "Input.h"
#include <iostream>
#include "../WinApp.h"

JimaXengine::WinApp* JimaXengine::Input::pWindow;

BYTE JimaXengine::Input::currentKey[256] = {};
BYTE JimaXengine::Input::prevKey[256] = {};
IDirectInput8* JimaXengine::Input::dinput = nullptr;
IDirectInputDevice8* JimaXengine::Input::devkeyboard = nullptr;

DIMOUSESTATE JimaXengine::Input::currentMouse;
DIMOUSESTATE JimaXengine::Input::prevMouse;
IDirectInputDevice8* JimaXengine::Input::devmouse = nullptr;
JimaXengine::Vector2 JimaXengine::Input::currentMousePos = {};
JimaXengine::Vector2 JimaXengine::Input::prevMousePos = {};

XINPUT_STATE JimaXengine::Input::xinputCurrent = {};
XINPUT_STATE JimaXengine::Input::xinputPrev = {};
int JimaXengine::Input::leftDeadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
int JimaXengine::Input::rightDeadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

void JimaXengine::Input::Initialize(WinApp* window)
{
	pWindow = window;

	HRESULT result;

	// DirectInputオブジェクトの生成
	result = DirectInput8Create(pWindow->GetWndClass().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);

	// キーボードデバイスの生成
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);
	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);
	result = devkeyboard->SetCooperativeLevel(pWindow->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	// マウスデバイスの生成
	result = dinput->CreateDevice(GUID_SysMouse, &devmouse, NULL);
	if (FAILED(result))assert(0);
	result = devmouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))assert(0);
	result = devmouse->SetCooperativeLevel(pWindow->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (FAILED(result))assert(0);

	//ゲームパッドの初期化
	// ZeroMemory・・・内部で使われているものすべてに0を代入する
	ZeroMemory(&xinputCurrent, sizeof(XINPUT_STATE));
	ZeroMemory(&xinputPrev, sizeof(XINPUT_STATE));

#ifdef _DEBUG
	printf("input initialized\n");
#endif // _DEBUG
}

void JimaXengine::Input::Update()
{
	// キーボード
	devkeyboard->Acquire();
	memcpy(prevKey, currentKey, sizeof(currentKey));
	devkeyboard->GetDeviceState(sizeof(currentKey), currentKey);

	// マウス
	devmouse->Acquire();
	memcpy(&prevMouse, &currentMouse, sizeof(currentMouse));
	devmouse->GetDeviceState(sizeof(currentMouse), &currentMouse);

	// マウスポジションの更新
	POINT tmpPoint;
	GetCursorPos(&tmpPoint);
	ScreenToClient(pWindow->GetHwnd(), &tmpPoint);
	prevMousePos = currentMousePos;
	currentMousePos = { (float)tmpPoint.x, (float)tmpPoint.y };

	// パッド
	xinputPrev = xinputCurrent;
	// リザルトが0以外はエラー
	//DWORD a= XInputGetState(0, &xinputCurrent);
	XInputGetState(0, &xinputCurrent);
}

void JimaXengine::Input::SetDeadZone(int lDeadZone, int rDeadZone)
{
	leftDeadZone = lDeadZone;
	rightDeadZone = rDeadZone;
}

void JimaXengine::Input::Finalize()
{
	dinput->Release();
	devkeyboard->Release();
	devmouse->Release();
}

bool JimaXengine::Input::KeyPress(const int& keys)
{
	 return (currentKey[keys]);
}

bool JimaXengine::Input::KeyTrigger(const int& keys)
{
	return (currentKey[keys] && !prevKey[keys]);
}

bool JimaXengine::Input::KeyRelease(const int & keys)
{
	return (!currentKey[keys] && prevKey[keys]);
}

bool JimaXengine::Input::MouseButtonPress(const int & keys)
{
	return (currentMouse.rgbButtons[keys]);
}

bool JimaXengine::Input::MouseButtonTrigger(const int & keys)
{
	return (currentMouse.rgbButtons[keys] && !prevMouse.rgbButtons[keys]);
}

bool JimaXengine::Input::MouseButtonHold(const int & keys)
{
	return (currentMouse.rgbButtons[keys] && prevMouse.rgbButtons[keys]);
}

bool JimaXengine::Input::MouseButtonRelease(const int & keys)
{
	return (!currentMouse.rgbButtons[keys] && prevMouse.rgbButtons[keys]);
}

int JimaXengine::Input::MouseXMove()
{
	return currentMouse.lX;
}

int JimaXengine::Input::MouseYMove()
{
	return currentMouse.lY;
}

int JimaXengine::Input::MouseWheelMove()
{
	return currentMouse.lZ;
}

void JimaXengine::Input::SetMousePos(Vector2 pos)
{
	RECT tmpRect;
	GetWindowRect(pWindow->GetHwnd(), &tmpRect);

	SetCursorPos((int)(pos.x + tmpRect.left + pWindow->WINDOW_WIDTH), (int)(pos.y + tmpRect.top + pWindow->WINDOW_HEIGHT));
}

JimaXengine::Vector2 JimaXengine::Input::GetCurrentMousePos()
{
	return currentMousePos;
}

JimaXengine::Vector2 JimaXengine::Input::GetPrevMousePos()
{
	return prevMousePos;
}

bool JimaXengine::Input::PadButtonPress(const int & keys)
{
	return (xinputCurrent.Gamepad.wButtons & keys);
}

bool JimaXengine::Input::PadButtonTrigger(const int & keys)
{
	return ((xinputCurrent.Gamepad.wButtons & keys) && !(xinputPrev.Gamepad.wButtons & keys));
}

bool JimaXengine::Input::PadButtonRelease(const int & keys)
{
	return (!(xinputCurrent.Gamepad.wButtons & keys) && (xinputPrev.Gamepad.wButtons & keys));
}

bool JimaXengine::Input::PadLTPress()
{
	return (xinputCurrent.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}

bool JimaXengine::Input::PadLTTrigger()
{
	return ((xinputCurrent.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) && 
		!(xinputPrev.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD));
}

bool JimaXengine::Input::PadLTRelease()
{
	return (!(xinputCurrent.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) &&
		(xinputPrev.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD));
}

bool JimaXengine::Input::PadRTPress()
{
	return (xinputCurrent.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}

bool JimaXengine::Input::PadRTTrigger()
{
	return ((xinputCurrent.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) &&
		!(xinputPrev.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD));;
}

bool JimaXengine::Input::PadRTRelease()
{
	return (!(xinputCurrent.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) &&
		(xinputPrev.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD));
}

bool JimaXengine::Input::PadLeftStickRight()
{
	return (xinputCurrent.Gamepad.sThumbLX >= leftDeadZone);
}

bool JimaXengine::Input::PadLeftStickLeft()
{
	return (xinputCurrent.Gamepad.sThumbLX <= -leftDeadZone);
}

bool JimaXengine::Input::PadLeftStickUp()
{
	return (xinputCurrent.Gamepad.sThumbLY >= leftDeadZone);
}

bool JimaXengine::Input::PadLeftStickDown()
{
	return (xinputCurrent.Gamepad.sThumbLY <= -leftDeadZone);
}

bool JimaXengine::Input::PadRightStickRight()
{
	return (xinputCurrent.Gamepad.sThumbRX >= rightDeadZone);
}

bool JimaXengine::Input::PadRightStickLeft()
{
	return (xinputCurrent.Gamepad.sThumbRX <= -rightDeadZone);
}

bool JimaXengine::Input::PadRightStickUp()
{
	return (xinputCurrent.Gamepad.sThumbRY >= rightDeadZone);
}

bool JimaXengine::Input::PadRightStickDown()
{
	return (xinputCurrent.Gamepad.sThumbRY <= -rightDeadZone);
}

bool JimaXengine::Input::PadLeftStickRightTrigger()
{
	return ((xinputCurrent.Gamepad.sThumbLX >= leftDeadZone) && !(xinputPrev.Gamepad.sThumbLX >= leftDeadZone));
}

bool JimaXengine::Input::PadLeftStickLeftTrigger()
{
	return ((xinputCurrent.Gamepad.sThumbLX <= -leftDeadZone) && !(xinputPrev.Gamepad.sThumbLX <= -leftDeadZone));
}

bool JimaXengine::Input::PadLeftStickUpTrigger()
{
	return ((xinputCurrent.Gamepad.sThumbLY >= leftDeadZone) && !(xinputPrev.Gamepad.sThumbLY >= leftDeadZone));
}

bool JimaXengine::Input::PadLeftStickDownTrigger()
{
	return ((xinputCurrent.Gamepad.sThumbLY <= -leftDeadZone) && !(xinputPrev.Gamepad.sThumbLY <= -leftDeadZone));
}

bool JimaXengine::Input::PadRightStickRightTrigger()
{
	return ((xinputCurrent.Gamepad.sThumbRX >= rightDeadZone) && !(xinputPrev.Gamepad.sThumbRX >= rightDeadZone));
}

bool JimaXengine::Input::PadRightStickLeftTrigger()
{
	return ((xinputCurrent.Gamepad.sThumbRX <= -rightDeadZone) && !(xinputPrev.Gamepad.sThumbRX <= -rightDeadZone));
}

bool JimaXengine::Input::PadRightStickUpTrigger()
{
	return ((xinputCurrent.Gamepad.sThumbRY >= rightDeadZone) && !(xinputPrev.Gamepad.sThumbRY >= rightDeadZone));
}

bool JimaXengine::Input::PadRightStickDownTrigger()
{
	return ((xinputCurrent.Gamepad.sThumbRY <= -rightDeadZone) && !(xinputPrev.Gamepad.sThumbRY <= -rightDeadZone));
}

float JimaXengine::Input::PadLeftStickPercentageX()
{
	return xinputCurrent.Gamepad.sThumbLX < 0 ? -((float)xinputCurrent.Gamepad.sThumbLX / (float)SHRT_MIN) : ((float)xinputCurrent.Gamepad.sThumbLX / (float)SHRT_MAX);
}

float JimaXengine::Input::PadLeftStickPercentageY()
{
	return xinputCurrent.Gamepad.sThumbLY < 0 ? -((float)xinputCurrent.Gamepad.sThumbLY / (float)SHRT_MIN) : ((float)xinputCurrent.Gamepad.sThumbLY / (float)SHRT_MAX);
}

float JimaXengine::Input::PadRightStickPercentageX()
{
	return xinputCurrent.Gamepad.sThumbRX < 0 ? -((float)xinputCurrent.Gamepad.sThumbRX / (float)SHRT_MIN) : ((float)xinputCurrent.Gamepad.sThumbRX / (float)SHRT_MAX);
}

float JimaXengine::Input::PadRightStickPercentageY()
{
	return xinputCurrent.Gamepad.sThumbRY < 0 ? -((float)xinputCurrent.Gamepad.sThumbRY / (float)SHRT_MIN) : ((float)xinputCurrent.Gamepad.sThumbRY / (float)SHRT_MAX);
}

JimaXengine::Vector2 JimaXengine::Input::PadLeftStickVector()
{
	Vector2 vec;
	vec.x = PadLeftStickPercentageX();
	vec.y = -PadLeftStickPercentageY();
	return vec.Normalize();
}

JimaXengine::Vector2 JimaXengine::Input::PadRightStickVector()
{
	Vector2 vec;
	vec.x = PadRightStickPercentageX();
	vec.y = -PadRightStickPercentageY();
	return vec.Normalize();
}
