#include "Seat.h"
#include "../3d/FbxLoader.h"

JimaXengine::Seat::Seat(std::shared_ptr<Camera> camera, Vector3 pos, Vector3 rot)
{
	pCamera = camera;
	seatPos = pos;
	seatScale = Vector3(1, 1, 1);
	seatRot = rot;
	seatColor = Vector4(1, 1, 1, 1);

	seat = std::make_unique<Object3d>(seatPos, seatScale, seatRot, seatColor);
	seat->Initialize();
	seat->SetModelforBuff("seat");
}

JimaXengine::Seat::~Seat()
{
}

void JimaXengine::Seat::Draw()
{
	seat->SetCamera(pCamera);
	seat->Update();

	seat->Draw();
}
