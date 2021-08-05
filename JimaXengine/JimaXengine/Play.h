#pragma once
#include "scene/Scene.h"

class Model;
class Object3d;
class Play : public Scene
{
private:

	Model* model = nullptr;
	Object3d* object = nullptr;
	Object3d* object2 = nullptr;

public:
	Play(WinApp* winapp);
	~Play();
	void Initialize() override;
	void Update() override;
	void Draw() override;

};