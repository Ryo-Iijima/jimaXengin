#include "DefaultGeometryParticle.h"

JimaXengine::DefaultGeometryParticle::DefaultGeometryParticle(const ParticleParam& arg_param)
{
	param = arg_param;
}

JimaXengine::DefaultGeometryParticle::~DefaultGeometryParticle()
{
}

void JimaXengine::DefaultGeometryParticle::Initialize()
{
}

void JimaXengine::DefaultGeometryParticle::Update()
{
	param.vel += param.acc;
	param.pos += param.vel * param.speed;
	param.scl += param.add_scl;
	if (param.scl.x <= 0)param.scl.x = 0;
	if (param.scl.y <= 0)param.scl.y = 0;
	if (param.scl.z <= 0)param.scl.z = 0;

	param.rot += param.add_rot;
	param.life -= 1;
	param.alpha += param.add_alpha;
}