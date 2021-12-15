#pragma once
#include <memory>
#include "../3d/Object3d.h"

namespace JimaXengine
{
	class Seat
	{
	private:

		Camera* pCamera;

		Model* model = nullptr;

		// �q��
		std::unique_ptr<Object3d> seat;
		Vector3 seatPos;
		Vector3 seatScale;
		Vector3 seatRot;
		Vector4 seatColor;

	public:
		Seat(Camera* camera, Vector3 pos, Vector3 rot = { 0,180,0 });
		~Seat();

		void Draw();
	};

}