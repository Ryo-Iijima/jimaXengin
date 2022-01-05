#include "StrikeMark.h"
#include "../3d/FbxLoader.h"
#include "../GameObject/GameObjectManager.h"
#include "Target.h"
#include "../collision/Collision.h"

void JimaXengine::StrikeMark::ShrinkSize()
{
	float d = Vector3::Distance(pTarget->GetGool(), pTarget->GetPos());
	scale = Vector3().One * d / 15.0f;
	if (scale.x < 0.3f || scale.y < 0.2f)
	{
		scale = { 0.3f, 0.3f, 0.3f };
	}
}

void JimaXengine::StrikeMark::TraceBall()
{
	pos.x = pTarget->GetPos().x;
	pos.y = pTarget->GetPos().y;
	pos.z = oManager->GetPlayer()->GetPos().z;

	// 予測を正方形の範囲に収める(TODO)
	WithinRange();
}

void JimaXengine::StrikeMark::PassCheck()
{
	float limit = -13.0f;	// 打てるライン
	if (pTarget->GetPos().z < limit)
	{
		isPassed = true;
	}
}

void JimaXengine::StrikeMark::DeadConditions()
{
	// 跳ね返すか、元の球が消えたら自分も消える
	if (pTarget->GetReflection() || pTarget->GetIsDelete())
	{
		Dead();
		Delete();
	}
}

void JimaXengine::StrikeMark::WithinRange()
{
	float ignore = 1.0f;	// 無視する基準
	if (fabs(pos.x) > ignore)
	{
		pos.x = 1.0f / pos.x;
	}
	if (fabs(pos.y) > ignore)
	{
		pos.y = 1.0f / pos.y;
	}

	pos.x += center.x;
	pos.y += center.y;
}

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
	object = std::make_unique<Object3d>(pos, scale, rotation, color);
	object->Initialize();
	object->SetModelforBuff("ball");
	object->SetIsBillboard(true);

	scale = Vector3(0.3f, 0.3f, 0.01f);
	color = Vector4(1, 0.5f, 0.5f, 1.0f);

	pos = oManager->GetPlayer()->GetPos();
	center = oManager->GetPlayer()->GetPos();
	rad = 3.0f;

	isPassed = false;

}

void JimaXengine::StrikeMark::Update()
{
	// 消滅条件チェック
	DeadConditions();

	// 通過したかチェック
	PassCheck();

	if (!isPassed)
	{
		// 球の位置をトレース
		TraceBall();

		// 近づくにつれサイズを絞る
		ShrinkSize();
	}


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
