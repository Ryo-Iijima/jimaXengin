#include "Title.h"
#include "3d/Object3d.h"
#include "3d/FbxLoader.h"

Title::Title(WinApp* winapp)
{
}

Title::~Title()
{
	delete object;
	delete object2;
	delete model;

}

void Title::Initialize()
{
	model = FbxLoader::GetInstance().LoadModelFromFile("boneTest");
	// オブジェクトの生成とモデルのセット
	object = new Object3d;
	object->Initialize();
	object->SetModel(model);
	object2 = new Object3d;
	object2->Initialize();
	object2->SetModel(model);

}

void Title::Update()
{
	object->SetPosition(Vector3(10, 0, 0));
	object->Update();

	object2->SetPosition(Vector3(0, 0, 0));
	object2->Update();

}

void Title::Draw()
{
	//object->Draw(dxCommon->GetCommandList());
	object->Draw();
	object2->Draw();

}
