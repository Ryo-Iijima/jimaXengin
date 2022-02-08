#pragma once
#include <memory>
#include "../3d/Object3d.h"

namespace JimaXengine
{
	class Seat
	{
	private:

		std::shared_ptr<Camera> pCamera;

		// ãqê»
		std::unique_ptr<Object3d> seat;
		Vector3 seatPos;
		Vector3 seatScale;
		Vector3 seatRot;
		Vector4 seatColor;

	public:
		Seat(std::shared_ptr<Camera> camera, Vector3 pos, Vector3 rot = { 0,180,0 });
		~Seat();

		void Draw();
	};

}