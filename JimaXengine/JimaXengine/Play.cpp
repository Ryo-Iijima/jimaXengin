#include "Play.h"
#include "Player.h"

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
}

void Play::Update()
{
	player->Update();
}

void Play::Draw()
{
	player->Draw();
}
