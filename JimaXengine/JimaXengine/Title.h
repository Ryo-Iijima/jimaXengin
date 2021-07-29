#pragma once
#include "scene/Scene.h"

class Model;
class Object3d;
class Title : public Scene
{
private:

	Model* model = nullptr;
	Object3d* object = nullptr;
	Object3d* object2 = nullptr;

public:
	Title(WinApp* winapp);
	~Title();
	void Initialize() override;
	void Update() override;
	void Draw() override;

};

