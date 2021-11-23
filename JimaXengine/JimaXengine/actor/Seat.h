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

		// ãqê»
		std::unique_ptr<Object3d> seat;
		Vector3 seatPos;
		Vector3 seatRot;

	public:
		Seat(Camera* camera, Vector3 pos, Vector3 rot = { 0,180,0 });
		~Seat();

		void Draw();
	};

}