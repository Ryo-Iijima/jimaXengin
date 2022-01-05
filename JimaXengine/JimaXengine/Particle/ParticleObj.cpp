#include "ParticleObj.h"

JimaXengine::ParticleObj::ParticleObj()
{
}

JimaXengine::ParticleObj::~ParticleObj()
{
}

void JimaXengine::ParticleObj::Initialize(ParticleParam parameter, std::string model, std::string file, std::string shader, std::string blend, std::string pipeline)
{
	obj3d = std::make_unique<Object3d>(param.pos, param.scl, param.rot, Vector4(param.color, param.alpha));
	obj3d->Initialize();

	param = parameter;
	isDead = false;
	modelname = model;
	filename = file;
	shadername = shader;
	blendname = blend;
	pipelinename = pipeline;
}

void JimaXengine::ParticleObj::Update()
{
	param.vel.Normalize();
	param.vel *= param.speed;
	param.vel += param.acc;
	param.pos += param.vel;
	param.scl += param.add_scl;
	param.rot += param.add_rot;
	param.color;
	param.alpha += param.add_alpha;
	param.life--;

	if (param.life <= 0)
	{
		isDead = true;
	}

	if (!isDead)
	{
		obj3d->Update();
	}
}

void JimaXengine::ParticleObj::Draw(Camera* camera)
{
	if (!isDead)
	{
		Vector4 rgba = { param.color.x,param.color.y,param.color.z,param.alpha };
		obj3d->Draw();
	}
}
