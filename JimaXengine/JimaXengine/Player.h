#pragma once
#include "hidapi/hidapi.h"
#include "math/Vector3.h"

class Camera;
class Model;
class Object3d;
class Player
{
private:
	Model* model = nullptr;
	Object3d* object = nullptr;
	Camera* camera = nullptr;

	Vector3 pos;
	Vector3 vel;
	Vector3 rot;

	hid_device_info* device = nullptr;
	hid_device* dev = nullptr;
	uint8_t buff[0x40];
	size_t size;

	int pre_gyro_x = 0;
	int cur_gyro_x = 0;
	int gyro_x;
	Vector3 accel, gyro;

private:
	void JoyConInitialize();
	void JoyConUpdate();
	void Move();
public:
	Player();
	~Player();

	void Initialize();
	void Update();
	void Draw();

	Camera* GetCamera() { return camera; }
	void DrawAlphaObj();
};