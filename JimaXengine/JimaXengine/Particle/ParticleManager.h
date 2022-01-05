#pragma once
#include <vector>
#include "../camera/Camera.h"
#include "GeometoryObject3D.h"
#include "ParticleObj.h"
#include "DefaultGeometryParticle.h"
#include "ParticleBase.h"

namespace JimaXengine
{
	class ParticleManager
	{
	public:
		static void StaticInitialize();
		static void Finalize();

		// �p�[�e�B�N���̑���
		static int ParticleCount()
		{
			int a = (int)(std::distance(quad_alpha_particle.begin(), quad_alpha_particle.end()));
			int b = (int)(std::distance(quad_add_particle.begin(), quad_add_particle.end()));
			int c = (int)(std::distance(cube_alpha_particle.begin(), cube_alpha_particle.end()));
			int d = (int)(std::distance(cube_add_particle.begin(), cube_add_particle.end()));
			return a + b + c + d;
		}

		ParticleManager();
		~ParticleManager();
		void Initialize();
		void Update();
		// �e�ݒ育�Ƃ̕`��
		void DrawDepth(Camera* camera);
		void DrawDepthAlpha(Camera* camera);
		void DrawDepthNone(Camera* camera);
		// �e��ނ��Ƃ̒ǉ��iEmitter����D���Ȃ̂�I��ŌĂԁj
		void AddParticleObj(ParticleObj* particle);
		void AddQuadAlpha(ParticleBase* parameter);
		void AddQuadAdd(ParticleBase* parameter);
		void AddCubeAlpha(ParticleBase* parameter);
		void AddCubeAdd(ParticleBase* parameter);

	private:
		// �p�[�e�B�N���ɓ���������ꍇ������Update
		static void UpdateList(std::forward_list<ParticleBase*>& list);

		// list1��list2��ǉ�
		static void AddList(std::forward_list<ParticleBase*>& list1, std::forward_list<ParticleBase*>& list2);
		// objct3d�ł̍X�V
		static void UpdateObj();
		// forward_list�̍폜
		static void DeleteList(std::forward_list<ParticleBase*>& list);

		static GeometoryObject3D* q_alp_geo;
		static GeometoryObject3D* q_add_geo;
		static GeometoryObject3D* c_alp_geo;
		static GeometoryObject3D* c_add_geo;

		static std::vector<ParticleObj*> particleObj;
		static std::forward_list<ParticleBase*> quad_alpha_particle;
		static std::forward_list<ParticleBase*> quad_add_particle;
		static std::forward_list<ParticleBase*> cube_alpha_particle;
		static std::forward_list<ParticleBase*> cube_add_particle;
	};
}