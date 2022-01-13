#include "BatterCharacter.h"

JimaXengine::BatterCharacter::BatterCharacter(Camera* camera)
{
	pCamera = camera;
}

JimaXengine::BatterCharacter::~BatterCharacter()
{
}

void JimaXengine::BatterCharacter::Initialize()
{
	// キャラクター
	characterObject = std::make_unique<Object3d>(pos, scale, rotation, color);
	characterObject->Initialize();;
	//characterObject->SetModelforBuff("test_robo");
	characterObject->SetModelforBuff("tex_test_20x1");

	// バット
	batObject = std::make_unique<Object3d>(pos, batScale, rotation, color);
	batObject->Initialize();;
	batObject->SetModelforBuff("tex_test_20x1");
	batScale = Vector3(1.0f, 1.0f, 1.0f);
}

void JimaXengine::BatterCharacter::Update()
{
	// キャラクター
	characterObject->SetCamera(pCamera);
	characterObject->Update();

	// バット
	batObject->SetCamera(pCamera);
	batObject->Update();
}

void JimaXengine::BatterCharacter::Draw()
{
	// キャラクター
	characterObject->Draw();

	// バット
	//batObject->Draw();
}

JimaXengine::GameObject::TYPE JimaXengine::BatterCharacter::GetType()
{
	return GameObject::TYPE::PLAYER;
}

void JimaXengine::BatterCharacter::DrawImGui()
{
}
