#include "background.h"
#include "../3d/FbxLoader.h"
#include "../camera/Camera.h"

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
	model = FbxLoader::GetInstance().LoadModelFromFile("wall");
	wall = std::make_unique<Object3d>();
	wall->Initialize();
	wall->SetModel(model);

	model = FbxLoader::GetInstance().LoadModelFromFile("ground");
	ground = std::make_unique<Object3d>();
	ground->Initialize();
	ground->SetModel(model);
	
	model = FbxLoader::GetInstance().LoadModelFromFile("seat");
	seat = std::make_unique<Object3d>();
	seat->Initialize();
	seat->SetModel(model);
	

	pos = { 0,0,0 };

	wall->SetPosition(pos + Vector3(0, 20, 0));
	wall->SetScale(Vector3(20, 0.01f, 100));
	wall->SetColor(Vector4(0,0,0,1));

	ground->SetPosition(pos + Vector3(0, -10, 0));
	ground->SetScale(Vector3(1,1,1));
	ground->SetColor(Vector4(1, 1, 1, 1));
	ground->SetRotation(Vector3(0, 180, 0));

	seat->SetPosition(pos + Vector3(-20, 0, 0));
	seat->SetScale(Vector3(0.01f, 20, 100));
	seat->SetColor(Vector4(0, 0, 0, 1));


}

void JimaXengine::BackGround::Update()
{
	wall->SetCamera(pCamera);
	wall->Update();
	ground->SetCamera(pCamera);
	ground->Update();
	seat->SetCamera(pCamera);
	seat->Update();
}

void JimaXengine::BackGround::Draw()
{
	//top->Draw();
	ground->Draw();
	//seat->Draw();
}

JimaXengine::GameObject::TYPE JimaXengine::BackGround::GetType()
{
	return GameObject::TYPE::NONE;
}

void JimaXengine::BackGround::DrawImGui()
{
}
