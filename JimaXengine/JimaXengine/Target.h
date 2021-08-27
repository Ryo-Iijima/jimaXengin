#pragma once
#include "math/Vector3.h"

class Model;
class Object3d;
class Target
{
private:
	Model* model = nullptr;
	Object3d* object = nullptr;

	Vector3 pos;
	Vector3 vel;
	Vector3 rot;

private:
	void Move();

public:
	Target();
	~Target();

	void Initialize();
	void Update();
	void Draw();

};

