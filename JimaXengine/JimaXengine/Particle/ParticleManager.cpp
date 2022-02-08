#include "ParticleManager.h"

JimaXengine::GeometoryObject3D* JimaXengine::ParticleManager::q_alp_geo = nullptr;
JimaXengine::GeometoryObject3D* JimaXengine::ParticleManager::q_add_geo = nullptr;
JimaXengine::GeometoryObject3D* JimaXengine::ParticleManager::c_alp_geo = nullptr;
JimaXengine::GeometoryObject3D* JimaXengine::ParticleManager::c_add_geo = nullptr;

std::forward_list<JimaXengine::ParticleBase*> JimaXengine::ParticleManager::quad_alpha_particle;
std::forward_list<JimaXengine::ParticleBase*> JimaXengine::ParticleManager::quad_add_particle;
std::forward_list<JimaXengine::ParticleBase*> JimaXengine::ParticleManager::cube_alpha_particle;
std::forward_list<JimaXengine::ParticleBase*> JimaXengine::ParticleManager::cube_add_particle;
std::vector<JimaXengine::ParticleObj*> JimaXengine::ParticleManager::particleObj;

void JimaXengine::ParticleManager::StaticInitialize()
{
	// ジオメトリオブジェクトの初期化
	q_alp_geo = new GeometoryObject3D();
	q_add_geo = new GeometoryObject3D();
	c_alp_geo = new GeometoryObject3D();
	c_add_geo = new GeometoryObject3D();
	q_alp_geo->Initialize();
	q_add_geo->Initialize();
	c_alp_geo->Initialize();
	c_add_geo->Initialize();
}

void JimaXengine::ParticleManager::Finalize()
{
	delete q_alp_geo;
	delete q_add_geo;
	delete c_alp_geo;
	delete c_add_geo;

	DeleteList(quad_alpha_particle);
	DeleteList(quad_add_particle);
	DeleteList(cube_alpha_particle);
	DeleteList(cube_add_particle);

	for (const auto& a : particleObj)
	{
		delete a;
	}
 	particleObj.clear();
}

JimaXengine::ParticleManager::ParticleManager()
{
}

JimaXengine::ParticleManager::~ParticleManager()
{
}

void JimaXengine::ParticleManager::Initialize()
{
	DeleteList(quad_alpha_particle);
	DeleteList(quad_add_particle);
	DeleteList(cube_alpha_particle);
	DeleteList(cube_add_particle);
}

void JimaXengine::ParticleManager::Update()
{
	UpdateList(quad_alpha_particle);
	UpdateList(quad_add_particle);
	UpdateList(cube_alpha_particle);
	UpdateList(cube_add_particle);

	q_alp_geo->Update(quad_alpha_particle);
	q_add_geo->Update(quad_add_particle);
	c_alp_geo->Update(cube_alpha_particle);
	c_add_geo->Update(cube_add_particle);

	UpdateObj();
}

void JimaXengine::ParticleManager::DrawDepth(std::shared_ptr<Camera> camera)
{
	c_alp_geo->Draw(camera.get(), cube_alpha_particle, "white1x1.png", "cube_alpha", GeometoryObject3D::BILLBOARD::BILLBOARD_NONE);

	auto end = particleObj.end();
	for (auto itr = particleObj.begin(); itr != end; itr++)
	{
		(*itr)->Draw(camera.get());
	}
}

void JimaXengine::ParticleManager::DrawDepthAlpha(std::shared_ptr<Camera> camera)
{
	c_add_geo->Draw(camera.get(), cube_add_particle, "white1x1.png", "cube_add", GeometoryObject3D::BILLBOARD::BILLBOARD_NONE);
}

void JimaXengine::ParticleManager::DrawDepthNoneFront(std::shared_ptr<Camera> camera)
{

}

void JimaXengine::ParticleManager::DrawDepthNoneBack(std::shared_ptr<Camera> camera)
{
	q_alp_geo->Draw(camera.get(), quad_alpha_particle, "particle.png", "quad_alpha", GeometoryObject3D::BILLBOARD::BILLBOARD_ALL);
	q_add_geo->Draw(camera.get(), quad_add_particle, "particle.png", "quad_add", GeometoryObject3D::BILLBOARD::BILLBOARD_ALL);
}

void JimaXengine::ParticleManager::AddParticleObj(ParticleObj * particle)
{
	particleObj.push_back(particle);
}

void JimaXengine::ParticleManager::AddQuadAlpha(ParticleBase* parameter)
{
	quad_alpha_particle.push_front(parameter);
}

void JimaXengine::ParticleManager::AddQuadAdd(ParticleBase* parameter)
{
	quad_add_particle.push_front(parameter);
}

void JimaXengine::ParticleManager::AddCubeAlpha(ParticleBase* parameter)
{
	cube_alpha_particle.push_front(parameter);
}

void JimaXengine::ParticleManager::AddCubeAdd(ParticleBase* parameter)
{
	cube_add_particle.push_front(parameter);
}

void JimaXengine::ParticleManager::UpdateList(std::forward_list<ParticleBase*>& list)
{
	list.remove_if([](ParticleBase* obj) { return obj->GetParticleParameter().life <= 0; });
	auto end = list.end();
	for (auto itr = list.begin(); itr != end; itr++)
	{
		(*itr)->Update();
	}
}

void JimaXengine::ParticleManager::AddList(std::forward_list<ParticleBase*>& list1, std::forward_list<ParticleBase*>& list2)
{
	auto end = list2.end();
	for (auto itr = list2.begin(); itr != end; itr++)
	{
		list1.push_front((*itr));
	}
}

void JimaXengine::ParticleManager::UpdateObj()
{
	auto itr = particleObj.begin();
	while (itr != particleObj.end())
	{
		(*itr)->Update();
		if ((*itr)->IsDead())
		{
			delete* itr;
			itr = particleObj.erase(itr);
		}
		else
		{
			itr++;
		}
	}
}

void JimaXengine::ParticleManager::DeleteList(std::forward_list<ParticleBase*>& list)
{
	for (auto itr = list.begin(); itr != list.end(); itr++)
	{
		delete (*itr);
	}
	list.clear();
}
