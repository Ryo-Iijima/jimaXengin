#pragma once
#include "Scene.h"

class Model;
class Object3d;
class Object2d;

class Title : public Scene
{
private:
	Object2d* pushStartTex = nullptr;
	Vector2 pushStartTexPos;

	Object2d* titleTex = nullptr;
	Vector2 titleTexPos;

	Object2d* bg = nullptr;
	Vector2 bgPos;

public:
	Title(WinApp* winapp);
	~Title();
	void Initialize() override;
	void Update() override;
	void Draw() override;

};