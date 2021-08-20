#pragma once
#include "3d/Object3d.h"
#include "hidapi/hidapi.h"

class Model;
class Object3d;
class Player
{
private:
	Model* model = nullptr;
	Object3d* object = nullptr;

	Vector3 pos;
	Vector3 vel;
	Vector3 rot;

	hid_device_info* device = nullptr;
	hid_device* dev = nullptr;
	uint8_t buff[0x40];
	size_t size;

	float pre_gyro_x = 0;
	float cur_gyro_x = 0;
	float gyro_x;
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
};