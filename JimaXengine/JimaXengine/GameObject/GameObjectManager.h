#pragma once
#include"GameObject.h"
#include "../Player.h"
//#include "MobiusParts.h"
//#include "MobiusRing.h"
//#include "MobiusRingManager.h"
//#include "EnemyManager.h"
//#include "Enemy.h"
//#include "ChargingItem.h"
//#include "ItemManager.h"
//#include "FloorClearItem.h"
//#include "PowerUpItem.h"
//#include "ScoreManager.h"
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