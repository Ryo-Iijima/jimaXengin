#include "Input.h"
#include <iostream>
#include "../WinApp.h"

WinApp* Input::pWindow = nullptr;

BYTE Input::currentKey[256] = {};
BYTE Input::prevKey[256] = {};
IDirectInput8* Input::dinput = nullptr;
IDirectInputDevice8* Input::devkeyboard = nullptr;

DIMOUSESTATE Input::currentMouse;
DIMOUSESTATE Input::prevMouse;
IDirectInputDevice8* Input::devmouse = nullptr;
Vector2 Input::currentMousePos = {};
Vector2 Input::prevMousePos = {};

XINPUT_STATE Input::xinputCurrent = {};
XINPUT_STATE Input::xinputPrev = {};
int Input::leftDeadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
int Input::rightDeadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

void Input::Initialize(WinApp* window)
{
	pWindow = window;

	HRESULT result;

	//DirectInputオブジェクトの生成
	result = DirectInput8Create(pWindow->GetWndClass().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);

	//キーボードデバイスの生成
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);
	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);
	result = devkeyboard->SetCooperativeLevel(pWindow->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	//マウスデバイスの生成
	result = dinput->CreateDevice(GUID_SysMouse, &devmouse, NULL);
	if (FAILED(result))assert(0);
	result = devmouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))assert(0);
	result = devmouse->SetCooperativeLevel(pWindow->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (FAILED(result))assert(0);

	//ゲームパッドの初期化
	//ZeroMemory・・・内部で使われているものすべてに0を代入する
	ZeroMemory(&xinputCurrent, sizeof(XINPUT_STATE));
	ZeroMemory(&xinputPrev, sizeof(XINPUT_STATE));

#ifdef _DEBUG
	printf("input initialized\n");
#endif // _DEBUG
}

void Input::Update()
{
	//キーボード
	devkeyboard->Acquire();
	memcpy(prevKey, currentKey, sizeof(currentKey));
	devkeyboard->GetDeviceState(sizeof(currentKey), currentKey);

	//マウス
	devmouse->Acquire();
	memcpy(&prevMouse, &currentMouse, sizeof(currentMouse));
	devmouse->GetDeviceState(sizeof(currentMouse), &currentMouse);

	//マウスポジションの更新
	POINT tmpPoint;
	GetCursorPos(&tmpPoint);
	ScreenToClient(pWindow->GetHwnd(), &tmpPoint);
	prevMousePos = currentMousePos;
	currentMousePos = { (float)tmpPoint.x, (float)tmpPoint.y };

	//パッド
	xinputPrev = xinputCurrent;
	XInputGetState(0, &xinputCurrent);
}

void Input::SetDeadZone(int lDeadZone, int rDeadZone)
{
	leftDeadZone = lDeadZone;
	rightDeadZone = rDeadZone;
}

void Input::Finalize()
{
	dinput->Release();
	devkeyboard->Release();
	devmouse->Release();
}

bool Input::KeyPress(const int& keys)
{
	 return (currentKey[keys]);
}

bool Input::KeyTrigger(const int& keys)
{
	return (currentKey[keys] && !prevKey[keys]);
}

bool Input::KeyRelease(const int & keys)
{
	return (!currentKey[keys] && prevKey[keys]);
}

bool Input::MouseButtonPress(const int & keys)
{
	return (currentMouse.rgbButtons[keys]);
}

bool Input::MouseButtonTrigger(const int & keys)
{
	return (currentMouse.rgbButtons[keys] && !prevMouse.rgbButtons[keys]);
}

bool Input::MouseButtonHold(const int & keys)
{
	return (currentMouse.rgbButtons[keys] && prevMouse.rgbButtons[keys]);
}

bool Input::MouseButtonRelease(const int & keys)
{
	return (!currentMouse.rgbButtons[keys] && prevMouse.rgbButtons[keys]);
}

int Input::MouseXMove()
{
	return currentMouse.lX;
}

int Input::MouseYMove()
{
	return currentMouse.lY;
}

int Input::MouseWheelMove()
{
	return currentMouse.lZ;
}

void Input::SetMousePos(Vector2 pos)
{
	RECT tmpRect;
	GetWindowRect(pWindow->GetHwnd(), &tmpRect);

	SetCursorPos((int)(pos.x + tmpRect.left + pWindow->WINDOW_WIDTH), (int)(pos.y + tmpRect.top + pWindow->WINDOW_HEIGHT));
}

Vector2 Input::GetCurrentMousePos()
{
	return currentMousePos;
}

Vector2 Input::GetPrevMousePos()
{
	return prevMousePos;
}

bool Input::PadButtonPress(const int & keys)
{
	return (xinputCurrent.Gamepad.wButtons & keys);
}

bool Input::PadButtonTrigger(const int & keys)
{
	return ((xinputCurrent.Gamepad.wButtons & keys) && !(xinputPrev.Gamepad.wButtons & keys));
}

bool Input::PadButtonRelease(const int & keys)
{
	return (!(xinputCurrent.Gamepad.wButtons & keys) && (xinputPrev.Gamepad.wButtons & keys));
}

bool Input::PadLTPress()
{
	return (xinputCurrent.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}

bool Input::PadLTTrigger()
{
	return ((xinputCurrent.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) && 
		!(xinputPrev.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD));
}

bool Input::PadLTRelease()
{
	return (!(xinputCurrent.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) &&
		(xinputPrev.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD));
}

bool Input::PadRTPress()
{
	return (xinputCurrent.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}

bool Input::PadRTTrigger()
{
	return ((xinputCurrent.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) &&
		!(xinputPrev.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD));;
}

bool Input::PadRTRelease()
{
	return (!(xinputCurrent.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) &&
		(xinputPrev.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD));
}

bool Input::PadLeftStickRight()
{
	return (xinputCurrent.Gamepad.sThumbLX >= leftDeadZone);
}

bool Input::PadLeftStickLeft()
{
	return (xinputCurrent.Gamepad.sThumbLX <= -leftDeadZone);
}

bool Input::PadLeftStickUp()
{
	return (xinputCurrent.Gamepad.sThumbLY >= leftDeadZone);
}

bool Input::PadLeftStickDown()
{
	return (xinputCurrent.Gamepad.sThumbLY <= -leftDeadZone);
}

bool Input::PadRightStickRight()
{
	return (xinputCurrent.Gamepad.sThumbRX >= rightDeadZone);
}

bool Input::PadRightStickLeft()
{
	return (xinputCurrent.Gamepad.sThumbRX <= -rightDeadZone);
}

bool Input::PadRightStickUp()
{
	return (xinputCurrent.Gamepad.sThumbRY >= rightDeadZone);
}

bool Input::PadRightStickDown()
{
	return (xinputCurrent.Gamepad.sThumbRY <= -rightDeadZone);
}

bool Input::PadLeftStickRightTrigger()
{
	return ((xinputCurrent.Gamepad.sThumbLX >= leftDeadZone) && !(xinputPrev.Gamepad.sThumbLX >= leftDeadZone));
}

bool Input::PadLeftStickLeftTrigger()
{
	return ((xinputCurrent.Gamepad.sThumbLX <= -leftDeadZone) && !(xinputPrev.Gamepad.sThumbLX <= -leftDeadZone));
}

bool Input::PadLeftStickUpTrigger()
{
	return ((xinputCurrent.Gamepad.sThumbLY >= leftDeadZone) && !(xinputPrev.Gamepad.sThumbLY >= leftDeadZone));
}

bool Input::PadLeftStickDownTrigger()
{
	return ((xinputCurrent.Gamepad.sThumbLY <= -leftDeadZone) && !(xinputPrev.Gamepad.sThumbLY <= -leftDeadZone));
}

bool Input::PadRightStickRightTrigger()
{
	return ((xinputCurrent.Gamepad.sThumbRX >= rightDeadZone) && !(xinputPrev.Gamepad.sThumbRX >= rightDeadZone));
}

bool Input::PadRightStickLeftTrigger()
{
	return ((xinputCurrent.Gamepad.sThumbRX <= -rightDeadZone) && !(xinputPrev.Gamepad.sThumbRX <= -rightDeadZone));
}

bool Input::PadRightStickUpTrigger()
{
	return ((xinputCurrent.Gamepad.sThumbRY >= rightDeadZone) && !(xinputPrev.Gamepad.sThumbRY >= rightDeadZone));
}

bool Input::PadRightStickDownTrigger()
{
	return ((xinputCurrent.Gamepad.sThumbRY <= -rightDeadZone) && !(xinputPrev.Gamepad.sThumbRY <= -rightDeadZone));
}

float Input::PadLeftStickPercentageX()
{
	return xinputCurrent.Gamepad.sThumbLX < 0 ? -((float)xinputCurrent.Gamepad.sThumbLX / (float)SHRT_MIN) : ((float)xinputCurrent.Gamepad.sThumbLX / (float)SHRT_MAX);
}

float Input::PadLeftStickPercentageY()
{
	return xinputCurrent.Gamepad.sThumbLY < 0 ? -((float)xinputCurrent.Gamepad.sThumbLY / (float)SHRT_MIN) : ((float)xinputCurrent.Gamepad.sThumbLY / (float)SHRT_MAX);
}

float Input::PadRightStickPercentageX()
{
	return xinputCurrent.Gamepad.sThumbRX < 0 ? -((float)xinputCurrent.Gamepad.sThumbRX / (float)SHRT_MIN) : ((float)xinputCurrent.Gamepad.sThumbRX / (float)SHRT_MAX);
}

float Input::PadRightStickPercentageY()
{
	return xinputCurrent.Gamepad.sThumbRY < 0 ? -((float)xinputCurrent.Gamepad.sThumbRY / (float)SHRT_MIN) : ((float)xinputCurrent.Gamepad.sThumbRY / (float)SHRT_MAX);
}

Vector2 Input::PadLeftStickVector()
{
	Vector2 vec;
	vec.x = PadLeftStickPercentageX();
	vec.y = -PadLeftStickPercentageY();
	return vec.Normalize();
}

Vector2 Input::PadRightStickVector()
{
	Vector2 vec;
	vec.x = PadRightStickPercentageX();
	vec.y = -PadRightStickPercentageY();
	return vec.Normalize();
}
