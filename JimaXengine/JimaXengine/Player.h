#pragma once
#include "hidapi/hidapi.h"
#include "GameObject/GameObject.h"

class Camera;
class Model;
class Object3d;

class Player : public GameObject
{
private:
	Model* model = nullptr;
	Object3d* object = nullptr;

	Object3d* layObj = nullptr;

	hid_device_info* device = nullptr;
	hid_device* dev = nullptr;
	uint8_t buff[0x40];
	size_t size;
	
	struct Int3
	{
		short x;
		short y;
		short z;
	};

	Int3 accel, gyro;

	Vector3 eye, target;

private:
	void JoyConInitialize();
	void JoyConUpdate();
	void Move();
public:
	Player(Camera* camera);
	~Player();

	void Initialize()override;
	void Update()override;
	void Draw()override;
	GameObject::TYPE GetType()override;
	void DrawImGui()override;

	void SetCamera(Camera* camera) { pCamera = camera; }
	Camera* GetCamera();
	void DrawAlphaObj();
};