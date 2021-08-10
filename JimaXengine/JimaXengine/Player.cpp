#include "Player.h"
#include "3d/Object3d.h"
#include "3d/FbxLoader.h"

Player::Player()
{

}

Player::~Player()
{
	delete object;
	delete model;
}

void Player::Initialize()
{
	model = FbxLoader::GetInstance().LoadModelFromFile("cube");
	// �I�u�W�F�N�g�̐����ƃ��f���̃Z�b�g
	object = new Object3d;
	object->Initialize();
	object->SetModel(model);

	object->SetPosition(Vector3(10, 0, 0));


}

void Player::Update()
{
	object->SetRotation(Vector3(30,30,30));
	object->Update();
}

void Player::Draw()
{
	object->Draw();
}