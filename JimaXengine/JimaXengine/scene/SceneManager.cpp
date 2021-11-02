#include "SceneManager.h"

JimaXengine::SceneManager::SceneManager()
{
	ClearScenes();
}

JimaXengine::SceneManager::~SceneManager()
{
	ClearScenes();
}

void JimaXengine::SceneManager::Add(const std::string& registerName, Scene * scene)
{
	//同名のシーンがある場合return
	if (scenes[registerName] != nullptr)
	{
		return;
	}

	scenes[registerName] = scene;
}

void JimaXengine::SceneManager::Update()
{
	//現在のシーンがnullptrの場合return
	if (currentScene == nullptr)
	{
		return;
	}

	currentScene->Update();
	if (currentScene->GetIsAllEnd()) {
		isAllEnd = true;
	}
}

void JimaXengine::SceneManager::Change(const std::string& name)
{
	//登録されてないシーン名での呼び出しが行われた場合return
	if (scenes[name] == nullptr)
	{
		return;
	}

	currentScene = scenes[name];
	currentScene->Initialize();
}

void JimaXengine::SceneManager::Draw()
{	
	//現在のシーンがnullptrの場合return
	if (currentScene == nullptr)
	{
		return;
	}

	currentScene->Draw();
	//シーン終了がtrueの時、現在のシーンが持っている次のシーンの名前を参照しChange()を行う
	if (currentScene->GetIsEnd())
	{
		Change(currentScene->GetNextScene());
	}
}

void JimaXengine::SceneManager::ClearScenes()
{
	//scenesが保持しているシーンの全消去
	for (auto itr = scenes.begin(); itr != scenes.end(); ++itr)
	{
		delete (*itr).second;
		(*itr).second = nullptr;
	}
	scenes.clear();
}

std::string JimaXengine::SceneManager::GetNowScene()
{
	return currentScene->GetNowScene();
}

std::string JimaXengine::SceneManager::GetNextScene()
{
	return currentScene->GetNextScene();
}

bool JimaXengine::SceneManager::GetIsAllEnd()
{
	return isAllEnd;
}
