#pragma once
#include "../GameObject/GameObject.h"

class Model;
class Object3d;
class Target : public GameObject
{
private:
	Model* model = nullptr;
	Object3d* object = nullptr;

	
private:
	void Move();
	void HitCheck();

public:
	Target(Camera* camera);
	~Target();

	void Initialize()override;
	void Update()override;
	void Draw()override;
	GameObject::TYPE GetType()override;
	void DrawImGui()override;

};

