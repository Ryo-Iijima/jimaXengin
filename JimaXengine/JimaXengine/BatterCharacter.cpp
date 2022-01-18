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
	// �L�����N�^�[
	characterObject = std::make_unique<Object3d>(pos, scale, rotation, color);
	characterObject->Initialize();;
	characterObject->SetModelforBuff("test_robo");
	scale = Vector3(1.0f, 1.0f, 1.0f);
	rotation = Vector3(0, 180, 0);

	// �o�b�g
	batObject = std::make_unique<Object3d>(pos, batScale, rotation, color);
	batObject->Initialize();;
	batObject->SetModelforBuff("bat");
	batScale = Vector3(1.0f, 1.0f, 1.0f);
}

void JimaXengine::BatterCharacter::Update()
{
	// �L�����N�^�[
	characterObject->SetCamera(pCamera);
	characterObject->Update();

	// �o�b�g
	batObject->SetCamera(pCamera);
	batObject->Update();
}

void JimaXengine::BatterCharacter::Draw()
{
	// �L�����N�^�[
	characterObject->Draw();

	// �o�b�g
	batObject->Draw();
}

JimaXengine::GameObject::TYPE JimaXengine::BatterCharacter::GetType()
{
	return GameObject::TYPE::PLAYER;
}

void JimaXengine::BatterCharacter::DrawImGui()
{
}

void JimaXengine::BatterCharacter::SetAnimationFrame(const int start, const int end, bool loop)
{
	characterObject->SetAnimationFrame(start, end);
	characterObject->SetAnimationLoop(loop);
	batObject->SetAnimationFrame(start, end);
	batObject->SetAnimationLoop(loop);
}
