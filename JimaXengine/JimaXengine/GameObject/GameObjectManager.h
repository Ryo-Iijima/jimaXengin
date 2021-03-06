#pragma once
#include"GameObject.h"
#include "../actor/Player.h"
#include "../actor/Target.h"
#include "../actor/Boss.h"
#include "../actor/BackGround.h"

#include "../Particle/ParticleManager.h"

namespace JimaXengine
{
	class GameObjectManager
	{
	public:
		GameObjectManager();
		~GameObjectManager();

		void Initialize();
		void Add(GameObject* obj);
		void Insert(GameObject* obj);	// 動的追加の場合
		void Update();
		void Draw();
		void ResetObject();
		Player* GetPlayer();
		Boss* GetBoss();
		ParticleManager* GetParticleManager() { return pManager.get(); }

	private:
		std::vector<GameObject*> gameObjects;
		std::vector<GameObject*> insertObjects;

		std::shared_ptr<ParticleManager> pManager;

		void SortRenderType();
		void Remove();
		void RemoveAll();
		void InsertAll();
	};
}