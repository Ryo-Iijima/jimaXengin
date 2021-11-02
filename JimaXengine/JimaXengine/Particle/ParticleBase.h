#pragma once
#include "ParticleParam.h"

namespace JimaXengine
{
	class ParticleBase
	{
	public:
		ParticleBase() = default;
		virtual ~ParticleBase() = default;
		virtual void Initialize() = 0;
		virtual void Update() = 0;
		const ParticleParam& GetParticleParameter() { return param; }

	protected:
		ParticleParam param;
	};
}