#pragma once
#include "ParticleBase.h"
#include "ParticleParam.h"

namespace JimaXengine
{
	class DefaultGeometryParticle : public ParticleBase
	{
	public:
		DefaultGeometryParticle(const ParticleParam& arg_param);
		~DefaultGeometryParticle();
		void Initialize() override;
		void Update() override;

	private:
	};
}