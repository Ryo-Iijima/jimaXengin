#include "DefaultGeometryParticle.h"

DefaultGeometryParticle::DefaultGeometryParticle(const ParticleParam& arg_param)
{
	param = arg_param;
}

DefaultGeometryParticle::~DefaultGeometryParticle()
{
}

void DefaultGeometryParticle::Initialize()
{
}

void DefaultGeometryParticle::Update()
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