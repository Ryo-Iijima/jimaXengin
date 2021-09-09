#pragma once
#include "GameObject/GameObject.h"

class Model;
class Object3d;

class BackGround : public GameObject
{
private:
	Model* model = nullptr;
	Object3d* top = nullptr;
	Object3d* bottom = nullptr;
	Object3d* left = nullptr;
	Object3d* right = nullptr;

public:
	BackGround(Camera* camera);
	~BackGround();

	void Initialize()override;
	void Update()override;
	void Draw()override;
	void SilhouetteDraw() override;
	GameObject::TYPE GetType()override;
	void DrawImGui()override;

};

