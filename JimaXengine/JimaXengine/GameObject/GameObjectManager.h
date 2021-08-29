#pragma once
#include"GameObject.h"
#include "../Player.h"
#include "../Target.h"

#include "../Particle/ParticleManager.h"

class GameObjectManager
{
public:
	GameObjectManager();
	~GameObjectManager();

	void Initialize();
	void Add(GameObject* obj);
	void Insert(GameObject* obj);
	void Update();
	void Draw();
	void ResetObject();
	Player* GetPlayer();	
	ParticleManager* GetParticleManager() { return pManager; }

private:
	std::vector<GameObject*> gameObjects;
	std::vector<GameObject*> insertObjects;

	ParticleManager* pManager;

	void SortRenderType();
	void Remove();
	void RemoveAll();
	void InsertAll();
};