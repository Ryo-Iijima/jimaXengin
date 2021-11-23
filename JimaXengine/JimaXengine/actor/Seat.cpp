#include "Seat.h"
#include "../3d/FbxLoader.h"

JimaXengine::Seat::Seat(Camera* camera, Vector3 pos, Vector3 rot)
{
	pCamera = camera;
	seatPos = pos;
	seatRot = rot;

	model = FbxLoader::GetInstance().LoadModelFromFile("seat");
	seat = std::make_unique<Object3d>();
	seat->Initialize();
	seat->SetModel(model);

	seat->SetPosition(seatPos);
	seat->SetScale(Vector3(1, 1, 1));
	seat->SetColor(Vector4(1, 1, 1, 1));
	seat->SetRotation(seatRot);
}

JimaXengine::Seat::~Seat()
{
}

void JimaXengine::Seat::Draw()
{
	seat->SetPosition(seatPos);
	seat->SetRotation(seatRot);
	seat->SetCamera(pCamera);
	seat->Update();

	seat->Draw();
}
