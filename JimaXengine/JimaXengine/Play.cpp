#include "Play.h"
#include "Player.h"
#include "3d/Object3d.h"
#include "3d/FbxLoader.h"

Play::Play(WinApp* winapp)
{
}

Play::~Play()
{
}

void Play::Initialize()
{
	player = new Player();
	player->Initialize();

	model = FbxLoader::GetInstance().LoadModelFromFile("cube");
	floor = new Object3d();
	floor->Initialize();
	floor->SetModel(model);
	floor->SetScale(Vector3(10, 0.01f, 10));
}

void Play::Update()
{
	player->Update();
	floor->Update();
}

void Play::Draw()
{
	player->Draw();
	floor->Draw();
}
