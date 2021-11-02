#pragma once
#include "ParticleBase.h"
#include "../GameObject/GameObject.h"

namespace JimaXengine
{
	class HomingGeometryParticle : public ParticleBase
	{
	public:
		HomingGeometryParticle(const ParticleParam& arg_param, GameObject* arg_gameObj);
		~HomingGeometryParticle();
		void Initialize() override;
		void Update() override;

	private:
		GameObject* gameObject;

		int time;
	};
}