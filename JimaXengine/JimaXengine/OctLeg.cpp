#include "OctLeg.h"

JimaXengine::OctLeg::OctLeg(Camera* camera)
{
	pCamera = camera;
}

JimaXengine::OctLeg::OctLeg(Camera* camera, Vector3 pos, Vector3 rot)
{
	pCamera = camera;
	diffPos = pos;
	rotation = rot;
}

JimaXengine::OctLeg::~OctLeg()
{
}

void JimaXengine::OctLeg::Initialize()
{
	object = std::make_unique<Object3d>(pos, scale, rotation, color);
	object->Initialize();
	object->SetModelforBuff("octoleg");
}

void JimaXengine::OctLeg::Update()
{
	pos = rootPos + diffPos;

	object->SetCamera(pCamera);
	object->Update();
}

void JimaXengine::OctLeg::Draw()
{
	object->Draw();
}
