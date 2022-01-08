#include "GameObject.h"

JimaXengine::GameObject::GameObject()
{
	pEmitter = new ParticleEmitter();
}

JimaXengine::GameObject::~GameObject()
{
	delete pEmitter;
}

void JimaXengine::GameObject::Dead()
{
	isDead = true;
}

void JimaXengine::GameObject::Delete()
{
	isDelete = true;
}

JimaXengine::GameObject::TYPE JimaXengine::GameObject::GetType()
{
	return TYPE::NONE;
}

JimaXengine::GameObject::RENDER_TYPE JimaXengine::GameObject::GetRenderType()
{
	return renderType;
}

void JimaXengine::GameObject::DrawImGui()
{
}

void JimaXengine::GameObject::ColPosSet()
{
}

void JimaXengine::GameObject::InitializeParticleEmitter(ParticleManager* pManager)
{
	pEmitter->Initialize(pManager);
}
