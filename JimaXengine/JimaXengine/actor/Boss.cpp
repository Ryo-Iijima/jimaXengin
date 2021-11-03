#include "Boss.h"
#include "../3d/FbxLoader.h"

JimaXengine::Boss::Boss(Camera* camera)
{
	pCamera = camera;
}

JimaXengine::Boss::~Boss()
{
}

void JimaXengine::Boss::Initialize()
{
	model = FbxLoader::GetInstance().LoadModelFromFile("DefaultBox");
	object = std::make_unique<Object3d>();
	object->Initialize();
	object->SetModel(model);

	pos = Vector3(0, 0, 50);
	object->SetPosition(pos);
	object->SetScale(Vector3(5, 5, 5));

	aabb3dCol.maxPos = Vector3(pos.x + 5, pos.y + 5, pos.z + 5);
	aabb3dCol.minPos = Vector3(pos.x - 5, pos.y - 5, pos.z - 5);
}

void JimaXengine::Boss::Update()
{
	// ダメージ受けてたら点滅する
	if (damaged)
	{
		i++;
		if (i > count)
		{
			i = 0;
			damaged = false;
		}
		if (i % 2 == 0)
		{
			object->SetColor(Vector4(1, 0, 0, 1));
		}
		else
		{
			object->SetColor(Vector4(1, 1, 1, 1));
		}
	}
	else
	{
		object->SetColor(Vector4(1, 1, 1, 1));
	}

	object->SetPosition(pos);
	aabb3dCol.maxPos = Vector3(pos.x + 5, pos.y + 5, pos.z + 5);
	aabb3dCol.minPos = Vector3(pos.x - 5, pos.y - 5, pos.z - 5);

	object->SetCamera(pCamera);
	object->Update();
}

void JimaXengine::Boss::Draw()
{
	object->Draw();
}

void JimaXengine::Boss::Dead()
{
}

void JimaXengine::Boss::Delete()
{
}

JimaXengine::GameObject::TYPE JimaXengine::Boss::GetType()
{
	return GameObject::TYPE::BOSS;
}

JimaXengine::GameObject::RENDER_TYPE JimaXengine::Boss::GetRenderType()
{
	return RENDER_TYPE();
}

void JimaXengine::Boss::DrawImGui()
{
}

void JimaXengine::Boss::Damage()
{
	damaged = true;
	hp--;
}
