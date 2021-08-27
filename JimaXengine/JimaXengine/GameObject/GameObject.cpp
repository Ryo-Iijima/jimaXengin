#include "GameObject.h"

GameObject::GameObject()
{
	pEmitter = new ParticleEmitter();
}

GameObject::~GameObject()
{
	delete pEmitter;
}

void GameObject::Dead()
{
	isDead = true;
}

void GameObject::Delete()
{
	isDelete = true;
}

GameObject::TYPE GameObject::GetType()
{
	return TYPE::NONE;
}

GameObject::RENDER_TYPE GameObject::GetRenderType()
{
	return renderType;
}

void GameObject::DrawImGui()
{
}

void GameObject::SilhouetteDraw()
{
}

void GameObject::InitializeParticleEmitter(ParticleManager* pManager)
{
	pEmitter->Initialize(pManager);
}
