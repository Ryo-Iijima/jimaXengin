#pragma once
#include "Scene.h"
#include <unordered_map>

class SceneManager
{
public:

	SceneManager();
	~SceneManager();
	void Add(const std::string& registerName, Scene* scene);
	void Change(const std::string& next);
	void Update();
	void Draw();
	void ClearScenes();

	std::string GetNowScene();
	std::string GetNextScene();
	bool GetIsAllEnd();

private:
	std::unordered_map<std::string, Scene*>scenes;
	Scene* currentScene;
	bool isAllEnd;
};

