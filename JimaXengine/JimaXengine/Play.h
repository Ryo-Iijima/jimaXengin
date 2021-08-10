#pragma once
#include "scene/Scene.h"

class Player;
class Play : public Scene
{
private:
	Player* player;

public:
	Play(WinApp* winapp);
	~Play();
	void Initialize() override;
	void Update() override;
	void Draw() override;

};