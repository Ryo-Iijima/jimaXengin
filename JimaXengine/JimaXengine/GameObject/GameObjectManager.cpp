#include "GameObjectManager.h"
#include "../camera/Camera.h"
#include <algorithm>

JimaXengine::GameObjectManager::GameObjectManager()
{
	pManager = new ParticleManager();
}

JimaXengine::GameObjectManager::~GameObjectManager()
{
	RemoveAll();
	delete pManager;
}

void JimaXengine::GameObjectManager::Initialize()
{
	RemoveAll();
	pManager->Initialize();
}

void JimaXengine::GameObjectManager::Add(GameObject* obj)
{
	obj->SetGameObjectManager(this);
	obj->Initialize();
	obj->InitializeParticleEmitter(pManager);
	gameObjects.push_back(obj);
	SortRenderType();
}

void JimaXengine::GameObjectManager::Insert(GameObject * obj)
{
	obj->SetGameObjectManager(this);
	obj->Initialize();
	obj->InitializeParticleEmitter(pManager);
	insertObjects.push_back(obj);
}

void JimaXengine::GameObjectManager::Update()
{
	for (auto it = gameObjects.begin(); it != gameObjects.end(); ++it)
	{
		if ((*it)->GetIsDead())
		{
			continue;
		}
		(*it)->Update();
	}
	pManager->Update();

	Remove();
	InsertAll();
}

void JimaXengine::GameObjectManager::Draw()
{
	//深度テストありパーティクル
	if (GetPlayer())
	{
		pManager->DrawDepth(GetPlayer()->GetCamera());
	}

	//通常ゲームオブジェクト描画
	{
		auto end = gameObjects.end();
		for (auto it = gameObjects.begin(); it != end; ++it)
		{
			if ((*it)->GetIsDead())
			{
				continue;
			}
			(*it)->Draw();
#ifdef _DEBUG
			(*it)->DrawImGui();
#endif // _DEBUG
		}
	}

	//深度テストありアルファパーティクル
	if (GetPlayer())
	{
		pManager->DrawDepthAlpha(GetPlayer()->GetCamera());
		GetPlayer()->DrawAlphaObj();
		//深度テストなしパーティクル
		pManager->DrawDepthNone(GetPlayer()->GetCamera());
	}
}

void JimaXengine::GameObjectManager::ResetObject()
{
	auto end = gameObjects.end();
	for (auto it = gameObjects.begin(); it != end; ++it)
	{
		(*it)->Initialize();
	}
}

JimaXengine::Player* JimaXengine::GameObjectManager::GetPlayer()
{
	for (auto itr = gameObjects.begin(); itr != gameObjects.end(); ++itr)
	{
		if ((*itr)->GetType() == GameObject::TYPE::PLAYER)
		{
			Player* p = static_cast<Player*>(static_cast<void*>(*itr));
			return p;
		}
	}
	return nullptr;
}

JimaXengine::Boss* JimaXengine::GameObjectManager::GetBoss()
{
	for (auto itr = gameObjects.begin(); itr != gameObjects.end(); ++itr)
	{
		if ((*itr)->GetType() == GameObject::TYPE::BOSS)
		{
			Boss* p = static_cast<Boss*>(static_cast<void*>(*itr));
			return p;
		}
	}
	return nullptr;
}

void JimaXengine::GameObjectManager::SortRenderType()
{
	std::sort(gameObjects.begin(), gameObjects.end(), [](GameObject* obj1, GameObject* obj2) { return obj1->GetRenderType() < obj2->GetRenderType(); });
}

void JimaXengine::GameObjectManager::Remove()
{
	for (auto it = gameObjects.begin(); it != gameObjects.end();)
	{
		if (!(*it)->GetIsDelete()) { ++it; continue; }
		delete (*it);
		it = gameObjects.erase(it);

		int a = 0;
		a++;
	}
}

void JimaXengine::GameObjectManager::RemoveAll()
{
	auto end = gameObjects.end();
	for (auto it = gameObjects.begin(); it != end; ++it)
	{
		delete *it;
	}
	gameObjects.clear();
}

void JimaXengine::GameObjectManager::InsertAll()
{
	if (insertObjects.empty())
		return;

	auto end = insertObjects.end();

	for (auto it = insertObjects.begin(); it != end; ++it)
	{
		Add(*it);
	}
	insertObjects.clear();

	SortRenderType();
}