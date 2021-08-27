#include "Target.h"
#include "3d/Object3d.h"
#include "3d/FbxLoader.h"

void Target::Move()
{

}

Target::Target()
{
}

Target::~Target()
{
}

void Target::Initialize()
{
	model = FbxLoader::GetInstance().LoadModelFromFile("cube");
	object = new Object3d;
	object->Initialize();
	object->SetModel(model);

}

void Target::Update()
{
	object->SetPosition(pos);
	object->Update();
}

void Target::Draw()
{
	object->Draw();
}
