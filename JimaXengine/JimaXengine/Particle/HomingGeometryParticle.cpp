#include "HomingGeometryParticle.h"

HomingGeometryParticle::HomingGeometryParticle(const ParticleParam& arg_param, GameObject* arg_gameObj)
{
	param = arg_param;
	gameObject = arg_gameObj;
	time = 0;
}

HomingGeometryParticle::~HomingGeometryParticle()
{
}

void HomingGeometryParticle::Initialize()
{
}

void HomingGeometryParticle::Update()
{
	const int CLASH_TIME = 45;

	time++;
	if (time <= CLASH_TIME)
	{
		param.vel += param.acc;
		param.pos += param.vel * param.speed;
		param.scl += param.add_scl;
		param.rot += param.add_rot;
		param.life -= 1;
		param.alpha += param.add_alpha;
	}
	if (time > CLASH_TIME)
	{
		Vector3 tempPos = gameObject->GetPos();
		Vector3 vector = tempPos - param.pos;
		vector.Normalize();

		param.vel = vector;
		param.vel += param.acc;
		param.pos += param.vel * param.speed;
		param.scl += param.add_scl;
		param.rot += param.add_rot;
		param.life -= 1;
		param.alpha += param.add_alpha;
	}
}
