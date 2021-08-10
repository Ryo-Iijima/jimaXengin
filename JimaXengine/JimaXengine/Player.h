#pragma once
#include "3d/Object3d.h"

class Model;
class Object3d;
class Player
{
private:
	Model* model = nullptr;
	Object3d* object = nullptr;

	Vector3 pos;
	Vector3 rot;

public:
	Player();
	~Player();

	void Initialize();
	void Update();
	void Draw();
};

