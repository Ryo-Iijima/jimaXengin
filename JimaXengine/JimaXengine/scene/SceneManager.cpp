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
	//�����̃V�[��������ꍇreturn
	if (scenes[registerName] != nullptr)
	{
		return;
	}

	scenes[registerName] = scene;
}

void JimaXengine::SceneManager::Update()
{
	//���݂̃V�[����nullptr�̏ꍇreturn
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
	//�o�^����ĂȂ��V�[�����ł̌Ăяo�����s��ꂽ�ꍇreturn
	if (scenes[name] == nullptr)
	{
		return;
	}

	currentScene = scenes[name];
	currentScene->Initialize();
}

void JimaXengine::SceneManager::Draw()
{	
	//���݂̃V�[����nullptr�̏ꍇreturn
	if (currentScene == nullptr)
	{
		return;
	}

	currentScene->Draw();
	//�V�[���I����true�̎��A���݂̃V�[���������Ă��鎟�̃V�[���̖��O���Q�Ƃ�Change()���s��
	if (currentScene->GetIsEnd())
	{
		Change(currentScene->GetNextScene());
	}
}

void JimaXengine::SceneManager::ClearScenes()
{
	//scenes���ێ����Ă���V�[���̑S����
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
