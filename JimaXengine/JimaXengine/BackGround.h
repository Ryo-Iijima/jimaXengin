#pragma once
#include "GameObject/GameObject.h"
#include <memory>

class Model;
class Object3d;

class BackGround : public GameObject
{
private:
	Model* model = nullptr;
	//Object3d* top = nullptr;
	//Object3d* bottom = nullptr;
	//Object3d* left = nullptr;
	//Object3d* right = nullptr;

	std::unique_ptr<Object3d> top;
	std::unique_ptr<Object3d> bottom;
	std::unique_ptr<Object3d> left;
	std::unique_ptr<Object3d> right;

public:
	BackGround(Camera* camera);
	~BackGround();

	void Initialize()override;
	void Update()override;
	void Draw()override;

	GameObject::TYPE GetType()override;
	void DrawImGui()override;

};

