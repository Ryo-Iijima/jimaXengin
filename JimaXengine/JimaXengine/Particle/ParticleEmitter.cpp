#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include "../general/Random.h"
#include "ParticleObj.h"
#include "DefaultGeometryParticle.h"
#include "HomingGeometryParticle.h"
//#include "GameObjectManager.h"

JimaXengine::ParticleEmitter::ParticleEmitter()
{
}

JimaXengine::ParticleEmitter::~ParticleEmitter()
{
}

void JimaXengine::ParticleEmitter::Initialize(ParticleManager * manager)
{
	this->manager = manager;
}

void JimaXengine::ParticleEmitter::TempleteGeo(const Vector3& pos)
{
	ParticleParam param = {};
	for (int i = 0; i < 10; i++)
	{
		//位置
		param.pos = pos;
		param.vel.x = (float)Random::GetRandom(-1, 1);
		param.vel.y = (float)Random::GetRandom(-1, 1);
		param.vel.z = (float)Random::GetRandom(-1, 1);
		param.vel.Normalize();
		param.acc = Vector3::Zero;
		param.speed = 0.5f;
		//サイズ
		float size = 5.0f;
		param.scl = { size,size,size };
		param.add_scl = Vector3::Zero;
		//回転
		param.rot.x = Random::GetRandom(0, 360);
		param.rot.y = Random::GetRandom(0, 360);
		param.rot.z = Random::GetRandom(0, 360);
		param.add_rot.x = Random::GetRandom(0, 10);
		param.add_rot.y = Random::GetRandom(0, 10);
		param.add_rot.z = Random::GetRandom(0, 10);
		//寿命
		param.life = 60;
		//色
		param.color.x = (Random::GetRandom(0, 255) / 255.0f);
		param.color.y = (Random::GetRandom(0, 255) / 255.0f);
		param.color.z = (Random::GetRandom(0, 255) / 255.0f);
		//透明度
		param.alpha = 1.0f;
		param.add_alpha = 0;
		//パーティクルを増やす
		manager->AddCubeAlpha(new DefaultGeometryParticle(param));
	}
}

void JimaXengine::ParticleEmitter::TempleteObj(const Vector3& pos)
{
	ParticleParam param = {};

	for (int i = 0; i < 15; i++)
	{
		ParticleObj* p = new ParticleObj();
		//位置
		param.pos = pos;
		param.vel.x = (float)Random::GetRandom(-1, 1);
		param.vel.y = (float)Random::GetRandom(-1, 1);
		param.vel.z = (float)Random::GetRandom(-1, 1);
		param.vel.Normalize();
		param.acc = Vector3::Zero;
		param.speed = 0.5f;
		//サイズ
		float size = 5.0f;
		param.scl = { size,size,size };
		param.add_scl = Vector3::Zero;
		//回転

		param.rot.x = Random::GetRandom(0, 360);
		param.rot.y = Random::GetRandom(0, 360);
		param.rot.z = Random::GetRandom(0, 360);
		param.add_rot = Vector3::One;
		//寿命
		param.life = 60;
		//色
		param.color.x = (Random::GetRandom(0, 255) / 255.0f);
		param.color.y = (Random::GetRandom(0, 255) / 255.0f);
		param.color.z = (Random::GetRandom(0, 255) / 255.0f);
		//透明度
		param.alpha = 1.0f;
		param.add_alpha = 0;
		//パーティクルを増やす
		p->Initialize(param, "cube", "white", "Basic3D", "NOBLEND", "DEFAULT");
		manager->AddParticleObj(p);
	}
}

void JimaXengine::ParticleEmitter::CrashBlock(const Vector3& pos)
{
	ParticleParam param = {};
	for (int i = 0; i < 10; i++)
	{
		//位置
		param.pos = pos;
		param.vel.x = (float)Random::GetRandom(-1, 1) * 0.5f;
		param.vel.y = 1;
		param.vel.z = (float)Random::GetRandom(-1, 1) * 0.5f;
		param.vel.Normalize();
		param.acc.x = 0;
		param.acc.y = -0.05f;
		param.acc.z = 0;
		param.speed = 1.5f;
		//サイズ
		float size = Random::GetRandom(1.0f, 3.0f);
		param.scl = { size,size,size };
		param.add_scl = Vector3::Zero;
		//回転
		param.rot.x = Random::GetRandom(0, 360);
		param.rot.y = Random::GetRandom(0, 360);
		param.rot.z = Random::GetRandom(0, 360);
		param.add_rot = Vector3::One;
		//寿命
		param.life = 60;
		//色
		param.color.x = (115.0f / 255.0f);
		param.color.y = (66.0f / 255.0f);
		param.color.z = (41.0f / 255.0f);
		//透明度
		param.alpha = 1.0f;
		param.add_alpha = 0;
		//パーティクルを増やす
		manager->AddCubeAlpha(new DefaultGeometryParticle(param));
	}
}

void JimaXengine::ParticleEmitter::Drift(const Vector3& pos, const Vector3& vec)
{
	ParticleParam param = {};
	for (int i = 0; i < 1; i++)
	{
		//位置
		param.pos = pos;
		param.vel.x = vec.x + Random::GetRandom(vec.x - 0.1f, vec.x + 0.1f);
		param.vel.y = Random::GetRandom(vec.y - 0.2f, vec.y + 0.1f);
		param.vel.z = vec.z + Random::GetRandom(vec.z - 0.1f, vec.z + 0.1f);
		param.vel.Normalize();
		param.acc.x = 0;
		param.acc.y = -0.005f;
		param.acc.z = 0;
		param.speed = 1.0f;
		//サイズ
		float size = 1.0f;
		param.scl = { size,size,size };
		param.add_scl = Vector3::Zero;
		//回転
		param.rot.x = Random::GetRandom(0, 360);
		param.rot.y = Random::GetRandom(0, 360);
		param.rot.z = Random::GetRandom(0, 360);
		param.add_rot.x = Random::GetRandom(0, 10);
		param.add_rot.y = Random::GetRandom(0, 10);
		param.add_rot.z = Random::GetRandom(0, 10);
		//寿命
		param.life = 180;
		//色
		param.color.x = (255.0f / 255.0f);
		param.color.y = (153.0f / 255.0f);
		param.color.z = (0.0f / 255.0f);
		//透明度
		param.alpha = 1.0f;
		param.add_alpha = -0.01f;
		//パーティクルを増やす
		manager->AddCubeAdd(new DefaultGeometryParticle(param));
	}
}

void JimaXengine::ParticleEmitter::RandomEffect(const Vector3& pos)
{
	ParticleParam param = {};
	for (int i = 0; i < 5; i++)
	{
		//位置
		param.pos = pos;
		param.vel.x = Random::GetRandom(-1.0f, 1.0f);
		param.vel.y = Random::GetRandom(-1.0f, 1.0f);
		param.vel.z = Random::GetRandom(-1.0f, 1.0f);
		param.vel.Normalize();
		//param.acc.x = Random::GetRandom(-1.0f, 1.0f);
		//param.acc.y = Random::GetRandom(-1.0f, 1.0f);
		//param.acc.z = Random::GetRandom(-1.0f, 1.0f);
		//param.acc *= 0.0001f;
		param.speed = 0.001f;
		//サイズ
		float size = 1.0f;
		param.scl = { size,size,size };
		param.add_scl = { -0.05f,-0.05f, -0.05f };
		//回転
		param.rot = Vector3::Zero;
		param.add_rot = Vector3::Zero;
		//寿命
		param.life = 60;
		//色
		param.color.x = 1.0f;
		param.color.y = 0.0f;
		param.color.z = 0.0f;
		//透明度
		param.alpha = 0.05f;
		param.add_alpha = 0.01f;
		//パーティクルを増やす
		manager->AddQuadAdd(new DefaultGeometryParticle(param));
	}
}

void JimaXengine::ParticleEmitter::Moya(const Vector3& pos, const Vector3& color)
{
	ParticleParam param = {};
	for (int i = 0; i < 1; i++)
	{
		//位置
		float range = 5.5f;
		param.pos.x = Random::GetRandom(pos.x - range, pos.x + range);
		param.pos.y = Random::GetRandom(pos.y - range, pos.y + range);
		param.pos.z = Random::GetRandom(pos.z - range, pos.z + range);
		param.vel.x = Random::GetRandom(-1.0f, 1.0f);
		param.vel.y = Random::GetRandom(-1.0f, 1.0f);
		param.vel.z = Random::GetRandom(-1.0f, 1.0f);
		param.vel.Normalize();
		param.acc.x = 0;
		param.acc.y = 0;
		param.acc.z = 0;
		param.speed = 0.1f;
		//サイズ
		float size = Random::GetRandom(1.5f, 2.5f);
		param.scl = { size,size,size };
		param.add_scl = { (-size * 0.01f),(-size * 0.01f),(-size * 0.01f) };
		//回転
		param.rot.x = Random::GetRandom(0, 360);
		param.rot.y = Random::GetRandom(0, 360);
		param.rot.z = Random::GetRandom(0, 360);
		param.add_rot = { 2,2,2 };
		//寿命
		param.life = 60;
		//色
		param.color = color;
		//透明度
		param.alpha = 1.0f;
		param.add_alpha = -0.015f;
		//パーティクルを増やす
		manager->AddCubeAdd(new DefaultGeometryParticle(param));
	}
}

void JimaXengine::ParticleEmitter::SphereEffect(const Vector3& pos, const Vector3& color)
{
	ParticleParam param = {};
	for (int i = 0; i < 100; i++)
	{
		//位置
		param.pos = pos;
		param.vel.x = Random::GetRandom(-1.0f, 1.0f);
		param.vel.y = Random::GetRandom(-1.0f, 1.0f);
		param.vel.z = Random::GetRandom(-1.0f, 1.0f);
		param.vel.Normalize();
		param.acc = Vector3::Zero;
		param.speed = 0.4f;
		//サイズ
		float size = 1.5f;
		param.scl = { size,size,size };
		param.add_scl = { (-size * 0.01f),(-size * 0.01f),(-size * 0.01f) };
		//回転
		param.rot.x = Random::GetRandom(0, 360);
		param.rot.y = Random::GetRandom(0, 360);
		param.rot.z = Random::GetRandom(0, 360);
		param.add_rot.x = Random::GetRandom(-1, 1);
		param.add_rot.y = Random::GetRandom(-1, 1);
		param.add_rot.z = Random::GetRandom(-1, 1);
		//寿命
		param.life = 60;
		//色
		param.color = color;
		//透明度
		param.alpha = 1.0f;
		param.add_alpha = -0.03f;
		//パーティクルを増やす
		manager->AddCubeAdd(new DefaultGeometryParticle(param));
	}
}

void JimaXengine::ParticleEmitter::PreSphereEffect(const Vector3& pos, const Vector3& color)
{
	ParticleParam param = {};
	for (int i = 0; i < 100; i++)
	{
		//位置
		param.pos = pos;
		param.vel.x = Random::GetRandom(-1.0f, 1.0f);
		param.vel.y = Random::GetRandom(-1.0f, 1.0f);
		param.vel.z = Random::GetRandom(-1.0f, 1.0f);
		param.vel.Normalize();
		param.acc = Vector3::Zero;
		param.speed = 0.4f;
		//サイズ
		float size = 0.5f;
		param.scl = { size,size,size };
		param.add_scl = { (-size * 0.01f),(-size * 0.01f),(-size * 0.01f) };
		//回転
		param.rot.x = Random::GetRandom(0, 360);
		param.rot.y = Random::GetRandom(0, 360);
		param.rot.z = Random::GetRandom(0, 360);
		param.add_rot.x = Random::GetRandom(-10, 10);
		param.add_rot.y = Random::GetRandom(-10, 10);
		param.add_rot.z = Random::GetRandom(-10, 10);
		//寿命
		param.life = 40;
		//色
		param.color = color;
		//透明度
		param.alpha = 1.0f;
		param.add_alpha = -0.05f;
		//パーティクルを増やす
		manager->AddCubeAdd(new DefaultGeometryParticle(param));
	}
}

void JimaXengine::ParticleEmitter::RandomCube(const Vector3& pos)
{
	ParticleParam param = {};
	for (int i = 0; i < 7; i++)
	{
		//位置
		param.pos = pos;
		param.vel.x = (float)Random::GetRandom(-1, 1);
		param.vel.y = 1;
		param.vel.z = (float)Random::GetRandom(-1, 1);
		param.vel.Normalize();
		param.acc.x = 0;
		param.acc.y = -0.05f;
		param.acc.z = 0;
		param.speed = 1.5f;
		//サイズ
		float size = Random::GetRandom(0.5f, 1.5f);
		param.scl = { size,size,size };
		param.add_scl = Vector3::Zero;
		//回転
		param.rot.x = Random::GetRandom(0, 360);
		param.rot.y = Random::GetRandom(0, 360);
		param.rot.z = Random::GetRandom(0, 360);
		param.add_rot.x = Random::GetRandom(-10, 10);
		param.add_rot.y = Random::GetRandom(-10, 10);
		param.add_rot.z = Random::GetRandom(-10, 10);
		//寿命
		param.life = 60;
		//色
		param.color.x = (255.0f / 255.0f);
		param.color.y = (0.0f / 255.0f);
		param.color.z = (0.0f / 255.0f);
		//透明度
		param.alpha = 0.5f;
		param.add_alpha = 0.0f;
		//パーティクルを増やす
		manager->AddCubeAlpha(new DefaultGeometryParticle(param));
	}
}

void JimaXengine::ParticleEmitter::RandomCubeHoming(const Vector3& pos, GameObject* gameObj)
{
	ParticleParam param = {};
	for (int i = 0; i < 7; i++)
	{
		//位置
		param.pos = pos;
		param.vel.x = (float)Random::GetRandom(-1, 1);
		param.vel.y = 1;
		param.vel.z = (float)Random::GetRandom(-1, 1);
		param.vel.Normalize();
		param.acc.x = 0;
		param.acc.y = -0.05f;
		param.acc.z = 0;
		param.speed = 1.75f;
		//サイズ
		float size = Random::GetRandom(0.5f, 1.5f);
		param.scl = { size,size,size };
		param.add_scl = Vector3::Zero;
		//回転
		param.rot.x = Random::GetRandom(0, 360);
		param.rot.y = Random::GetRandom(0, 360);
		param.rot.z = Random::GetRandom(0, 360);
		param.add_rot.x = Random::GetRandom(-10, 10);
		param.add_rot.y = Random::GetRandom(-10, 10);
		param.add_rot.z = Random::GetRandom(-10, 10);
		//寿命
		param.life = 120;
		//色
		param.color.x = (0.0f / 255.0f);
		param.color.y = (255.0f / 255.0f);
		param.color.z = (255.0f / 255.0f);
		//透明度
		param.alpha = 1.0f;
		param.add_alpha = 0.0f;
		//パーティクルを増やす
		manager->AddCubeAlpha(new HomingGeometryParticle(param, gameObj));
	}
}

void JimaXengine::ParticleEmitter::Homing(const Vector3& pos, GameObject* gameObj)
{
	ParticleParam param = {};
	for (int i = 0; i < 10; i++)
	{
		//位置
		param.pos = pos;
		param.vel.x = (float)Random::GetRandom(-1, 1) * 0.5f;
		param.vel.y = 1;
		param.vel.z = (float)Random::GetRandom(-1, 1) * 0.5f;
		param.vel.Normalize();
		param.acc.x = 0;
		param.acc.y = -0.05f;
		param.acc.z = 0;
		param.speed = 1.5f;
		//サイズ
		float size = Random::GetRandom(0.5, 2.0f);
		param.scl = { size,size,size };
		param.add_scl = Vector3::Zero;
		//回転
		param.rot.x = Random::GetRandom(0, 360);
		param.rot.y = Random::GetRandom(0, 360);
		param.rot.z = Random::GetRandom(0, 360);
		param.add_rot = Vector3::One;
		//寿命
		param.life = 120;
		//色
		param.color.x = (115.0f / 255.0f);
		param.color.y = (66.0f / 255.0f);
		param.color.z = (41.0f / 255.0f);
		//透明度
		param.alpha = 1.0f;
		param.add_alpha = 0;
		//パーティクルを増やす
		manager->AddCubeAlpha(new HomingGeometryParticle(param, gameObj));
	}
}

void JimaXengine::ParticleEmitter::AwayDrill(const Vector3& pos)
{
	ParticleParam param = {};
	for (int i = 0; i < 1; i++)
	{
		ParticleObj* p = new ParticleObj();
		//位置
		param.pos = pos;
		param.vel.x = (float)Random::GetRandom(-1, 1);
		param.vel.y = 1;
		param.vel.z = (float)Random::GetRandom(-1, 1);
		param.vel.Normalize();
		param.acc = Vector3::Zero;
		param.acc.y = -0.05f;
		param.speed = 1.5f;
		//サイズ
		float size = 5.0f;
		param.scl = { size,size,size };
		param.add_scl = { -size * 0.01f, -size * 0.01f, -size * 0.01f };
		//回転

		param.rot.x = Random::GetRandom(0, 360);
		param.rot.y = Random::GetRandom(0, 360);
		param.rot.z = Random::GetRandom(0, 360);
		param.add_rot.x = Random::GetRandom(-10, 10);
		param.add_rot.y = Random::GetRandom(-10, 10);
		param.add_rot.z = Random::GetRandom(-10, 10);
		//寿命
		param.life = 100;
		//色
		param.color = Vector3::One;
		//透明度
		param.alpha = 1.0f;
		param.add_alpha = 0;
		//パーティクルを増やす
		p->Initialize(param, "drill.obj", "drill_tex.png", "Basic3D", "NOBLEND", "DEFAULT");
		manager->AddParticleObj(p);
	}
}

void JimaXengine::ParticleEmitter::InvincibleEffect(const Vector3& pos)
{
	ParticleParam param = {};
	for (int i = 0; i < 5; i++)
	{
		//位置
		float range = 3.0f;
		param.pos.x = Random::GetRandom(pos.x - range, pos.x + range);
		param.pos.y = Random::GetRandom(pos.y - range, pos.y + range);
		param.pos.z = Random::GetRandom(pos.z - range, pos.z + range);
		param.vel = pos - param.pos;
		param.vel.Normalize();
		param.speed = 0.1f;
		//サイズ
		float size = 3.0f;
		float addSize = -size * 0.025f;
		param.scl = { size,size,size };
		param.add_scl = { addSize,addSize,addSize };
		//回転
		param.rot.x = Random::GetRandom(0, 360);
		param.rot.y = Random::GetRandom(0, 360);
		param.rot.z = Random::GetRandom(0, 360);
		param.add_rot.x = Random::GetRandom(-5, 5);
		param.add_rot.y = Random::GetRandom(-5, 5);
		param.add_rot.z = Random::GetRandom(-5, 5);
		//寿命
		param.life = 60;
		//色
		param.color.x = 255.0f / 255.0f;
		param.color.y = 165.0f / 255.0f;
		param.color.z = 0.0f / 255.0f;
		//透明度
		param.alpha = 1.0f;
		param.add_alpha = -0.015f;
		//パーティクルを増やす
		manager->AddCubeAdd(new DefaultGeometryParticle(param));
	}
}