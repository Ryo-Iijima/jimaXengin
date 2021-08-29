#pragma once
#include "scene/Scene.h"
#include "GameObject/GameObjectManager.h"

class Object3d;
class Object2d;
class Model;
class Player;
class Play : public Scene
{
private:
	//Player* player;
	Object3d* floor;
	Model* model = nullptr;
	
	Object2d* object2d;


	Camera* camera = nullptr;

	GameObjectManager* oManager = nullptr;

public:
	Play(WinApp* winapp);
	~Play();
	void Initialize() override;
	void Update() override;
	void Draw() override;

};