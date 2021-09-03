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
	//Camera* camera = nullptr;

	//Vector3 pos;
	//Vector3 vel;
	//Vector3 rot;

	hid_device_info* device = nullptr;
	hid_device* dev = nullptr;
	uint8_t buff[0x40];
	size_t size;

	int pre_gyro_x = 0;
	int cur_gyro_x = 0;
	int gyro_x;
	Vector3 accel, gyro;

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
	void SilhouetteDraw() override;
	GameObject::TYPE GetType()override;
	void DrawImGui()override;

	void SetCamera(Camera* camera) { pCamera = camera; }
	Camera* GetCamera();
	void DrawAlphaObj();
};