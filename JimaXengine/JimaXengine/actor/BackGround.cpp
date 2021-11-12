#include "background.h"
#include "../3d/FbxLoader.h"

JimaXengine::BackGround::BackGround(Camera* camera)
{
	pCamera = camera;

}

JimaXengine::BackGround::~BackGround()
{
	delete model;
}

void JimaXengine::BackGround::Initialize()
{
	//model = FbxLoader::GetInstance().LoadModelFromFile("DefaultBox");
	model = FbxLoader::GetInstance().LoadModelFromFile("ground");
	top = std::make_unique<Object3d>();
	top->Initialize();
	top->SetModel(model);

	bottom = std::make_unique<Object3d>();
	bottom->Initialize();
	bottom->SetModel(model);
	
	left = std::make_unique<Object3d>();
	left->Initialize();
	left->SetModel(model);
	
	right = std::make_unique<Object3d>();
	right->Initialize();
	right->SetModel(model);

	pos = { 0,0,0 };

	top->SetPosition(pos + Vector3(0, 20, 0));
	top->SetScale(Vector3(20, 0.01f, 100));
	top->SetColor(Vector4(0,0,0,1));

	bottom->SetPosition(pos + Vector3(0, -10, 0));
	bottom->SetScale(Vector3(1,1,1));
	bottom->SetColor(Vector4(1, 1, 1, 1));

	left->SetPosition(pos + Vector3(-20, 0, 0));
	left->SetScale(Vector3(0.01f, 20, 100));
	left->SetColor(Vector4(0, 0, 0, 1));

	right->SetPosition(pos + Vector3(20, 0, 0));
	right->SetScale(Vector3(0.01f, 20, 100));
	right->SetColor(Vector4(0, 0, 0, 1));

}

void JimaXengine::BackGround::Update()
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

void JimaXengine::BackGround::Draw()
{
	//top->Draw();
	bottom->Draw();
	//left->Draw();
	//right->Draw();
}

JimaXengine::GameObject::TYPE JimaXengine::BackGround::GetType()
{
	return GameObject::TYPE::NONE;
}

void JimaXengine::BackGround::DrawImGui()
{
}
