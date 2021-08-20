#pragma once
#include "scene/Scene.h"

class Object3d;
class Object2d;
class Model;
class Player;
class Play : public Scene
{
private:
	Player* player;
	Object3d* floor;
	Model* model = nullptr;
	
	Object2d* object2d;

public:
	Play(WinApp* winapp);
	~Play();
	void Initialize() override;
	void Update() override;
	void Draw() override;

};