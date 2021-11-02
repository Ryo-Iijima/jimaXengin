#pragma once
#include "../3d/Object3d.h"
#include "ParticleParam.h"

class Camera;
namespace JimaXengine
{
	class ParticleObj
	{
	public:
		ParticleObj();
		~ParticleObj();
		void Initialize(ParticleParam parameter,
			std::string model,
			std::string file,
			std::string shader,
			std::string blend,
			std::string pipeline);
		void Update();
		void Draw(Camera* camera);
		bool IsDead() { return isDead; }

	private:
		Object3d obj3d;
		ParticleParam param;
		bool isDead;
		std::string modelname;
		std::string filename;
		std::string shadername;
		std::string blendname;
		std::string pipelinename;
	};
}