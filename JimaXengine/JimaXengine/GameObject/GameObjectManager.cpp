#include "GameObjectManager.h"
#include "../camera/Camera.h"
#include <algorithm>

GameObjectManager::GameObjectManager()
{
	pManager = new ParticleManager();
}

GameObjectManager::~GameObjectManager()
{
	RemoveAll();
	delete pManager;
}

void GameObjectManager::Initialize()
{
	RemoveAll();
	pManager->Initialize();
}

void GameObjectManager::Add(GameObject* obj)
{
	obj->SetGameObjectManager(this);
	obj->Initialize();
	obj->InitializeParticleEmitter(pManager);
	gameObjects.push_back(obj);
	SortRenderType();
}

void GameObjectManager::Insert(GameObject * obj)
{
	obj->SetGameObjectManager(this);
	obj->Initialize();
	obj->InitializeParticleEmitter(pManager);
	insertObjects.push_back(obj);
}

void GameObjectManager::Update()
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

void GameObjectManager::Draw()
{
	//深度テストありパーティクル
	pManager->DrawDepth(GetPlayer()->GetCamera());

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
	pManager->DrawDepthAlpha(GetPlayer()->GetCamera());

	GetPlayer()->DrawAlphaObj();

	//深度テストなしパーティクル
	pManager->DrawDepthNone(GetPlayer()->GetCamera());
}

void GameObjectManager::ResetObject()
{
	auto end = gameObjects.end();
	for (auto it = gameObjects.begin(); it != end; ++it)
	{
		(*it)->Initialize();
	}
}

Player* GameObjectManager::GetPlayer()
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

void GameObjectManager::SortRenderType()
{
	std::sort(gameObjects.begin(), gameObjects.end(), [](GameObject* obj1, GameObject* obj2) { return obj1->GetRenderType() < obj2->GetRenderType(); });
}

void GameObjectManager::Remove()
{
	for (auto it = gameObjects.begin(); it != gameObjects.end();)
	{
		if (!(*it)->GetIsDelete()) { ++it; continue; }
		delete (*it);
		it = gameObjects.erase(it);
	}
}

void GameObjectManager::RemoveAll()
{
	auto end = gameObjects.end();
	for (auto it = gameObjects.begin(); it != end; ++it)
	{
		delete *it;
	}
	gameObjects.clear();
}

void GameObjectManager::InsertAll()
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