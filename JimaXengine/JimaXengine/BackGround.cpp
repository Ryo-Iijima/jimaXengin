#include "background.h"
#include "3d/FbxLoader.h"

BackGround::BackGround(Camera* camera)
{
	pCamera = camera;

}

BackGround::~BackGround()
{
	delete top;
	delete bottom;
	delete left;
	delete right;

	delete model;
}

void BackGround::Initialize()
{
	model = FbxLoader::GetInstance().LoadModelFromFile("enemy");
	top = new Object3d;
	top->Initialize();
	top->SetModel(model);

	bottom = new Object3d;
	bottom->Initialize();
	bottom->SetModel(model);
	
	left = new Object3d;
	left->Initialize();
	left->SetModel(model);
	
	right = new Object3d;
	right->Initialize();
	right->SetModel(model);

	pos = { 0,0,0 };

	top->SetPosition(pos + Vector3(0, 10, 0));
	bottom->SetPosition(pos + Vector3(0, -10, 0));
	left->SetPosition(pos + Vector3(-10, 0, 0));
	right->SetPosition(pos + Vector3(10, 0, 0));

}

void BackGround::Update()
{
	top->SetCamera(pCamera);
	top->Update();
	bottom->SetCamera(pCamera);
	bottom->Update();
	left->SetCamera(pCamera);
	left->Update();
	right->SetCamera(pCamera);
	right->Update();
}

void BackGround::Draw()
{
	top->Draw();
	bottom->Draw();
	left->Draw();
	right->Draw();
}

void BackGround::SilhouetteDraw()
{
}

GameObject::TYPE BackGround::GetType()
{
	return GameObject::TYPE::NONE;
}

void BackGround::DrawImGui()
{
}
