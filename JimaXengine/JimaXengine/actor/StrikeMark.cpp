#include "StrikeMark.h"
#include "../3d/FbxLoader.h"
#include "../GameObject/GameObjectManager.h"
#include "Target.h"

JimaXengine::StrikeMark::StrikeMark(Camera* pcamera, Target* ptarget)
	:pTarget(ptarget)
{
	pCamera = pcamera;
}

JimaXengine::StrikeMark::~StrikeMark()
{
}

void JimaXengine::StrikeMark::Initialize()
{
	object = std::make_unique<Object3d>();
	object->Initialize();
	object->SetModelforBuff("ball");
	object->SetScale(Vector3(0.3f, 0.3f, 0.01f));
	object->SetColor(Vector4(1, 0, 0, 1.0f));
}

void JimaXengine::StrikeMark::Update()
{
	// Œ³‚Ì‹…‚ªÁ‚¦‚½‚çŽ©•ª‚àÁ‚¦‚é
	if (pTarget->GetIsDead())
	{
		Dead();
		Delete();
	}

	// ‹…“¹‚©‚çˆÊ’u‚ð—\‘z
	object->SetPosition(oManager->GetPlayer()->GetPos());

	object->SetCamera(pCamera);
	object->Update();
}

void JimaXengine::StrikeMark::Draw()
{
	object->Draw();
}

JimaXengine::GameObject::TYPE JimaXengine::StrikeMark::GetType()
{
	return TYPE();
}

JimaXengine::GameObject::RENDER_TYPE JimaXengine::StrikeMark::GetRenderType()
{
	return RENDER_TYPE();
}

void JimaXengine::StrikeMark::DrawImGui()
{
}
