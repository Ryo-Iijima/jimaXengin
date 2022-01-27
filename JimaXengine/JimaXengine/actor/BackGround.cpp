#include "background.h"
#include "../3d/FbxLoader.h"
#include "../camera/Camera.h"

JimaXengine::BackGround::BackGround(Camera* camera)
{
	pCamera = camera;

}

JimaXengine::BackGround::~BackGround()
{
	delete model;
}

void JimaXengine::BackGround::Initialize()
{
	// レンダータイプ設定
	renderType = RENDER_TYPE::RENDER_TYPE_BACKGROUND;

	// モデルのセット
	wall = std::make_unique<Object3d>(wallPos, wallScale, wallRot, wallColor);
	wall->Initialize();
	wall->SetModelforBuff("wall");

	ground = std::make_unique<Object3d>(groundPos, groundScale, groundRot, groundColor);
	ground->Initialize();
	ground->SetModelforBuff("ground");
		
	groundDart = std::make_unique<Object3d>(groundDartPos, groundDartScale, groundDartRot, groundDartColor);
	groundDart->Initialize();
	groundDart->SetModelforBuff("DefaultPlane");
	//groundDart->SetPosition(Vector3(0, -0.1f, 0));
	//groundDart->SetScale(Vector3(100, 100, 100));
	groundDartPos = Vector3(0, -0.1f, 0);
	groundDartScale = Vector3(100, 100, 100);
	groundDartRot = Vector3(0, 0, 0);
	groundDartColor = Vector4(1, 1, 1, 1);

#pragma region 壁
	wallPos = Vector3(0, 2.5f, 40);
	wallRot = Vector3(0, 180, 0);
	wallScale = Vector3(3, 1, 1);
	wallColor = Vector4(1, 1, 1, 1);

	//wall->SetPosition(wallPos);
	//wall->SetScale(Vector3(2, 1, 1));
	//wall->SetColor(Vector4(1, 1, 1, 1));
	//wall->SetRotation(wallRot);


#pragma endregion

#pragma region 地面
	groundPos = Vector3(0, 0, 0);
	groundScale = Vector3(1, 1, 1);
	groundRot = Vector3(0, 180, 0);
	groundColor = Vector4(1, 1, 1, 1);

	//ground->SetPosition(groundPos);
	//ground->SetScale(groundScale);
	//ground->SetColor(Vector4(1, 1, 1, 1));
	//ground->SetRotation(groundRot);

#pragma endregion

#pragma region 客席
	for (int i = 0; i < sizeof(seat) / sizeof(seat[0]); i++)
	{
		seat[i] = std::make_unique<Seat>(pCamera, Vector3((float)(i * 10 - 70), 5, 45), Vector3(0, 180, 0));
	}
#pragma endregion 

}

void JimaXengine::BackGround::Update()
{
	//float a = 0.2f;
	//if (Input::KeyPress(DIK_UP)) seatPos.y += a;
	//if (Input::KeyPress(DIK_DOWN)) seatPos.y -= a;
	//if (Input::KeyPress(DIK_LEFT)) seatPos.z -= a;
	//if (Input::KeyPress(DIK_RIGHT)) seatPos.z += a;


#pragma region 壁
	wall->SetCamera(pCamera);
	wall->Update();
#pragma endregion

#pragma region 地面
	ground->SetCamera(pCamera);
	ground->Update();

	groundDart->SetCamera(pCamera);
	groundDart->Update();

#pragma endregion 

}

void JimaXengine::BackGround::Draw()
{
	wall->Draw();
	ground->Draw();
	groundDart->Draw();

	for (int i = 0; i < sizeof(seat) / sizeof(seat[0]); i++)
	{
		seat[i]->Draw();
	}
}

JimaXengine::GameObject::TYPE JimaXengine::BackGround::GetType()
{
	return GameObject::TYPE::BACKGROUND;
}

void JimaXengine::BackGround::DrawImGui()
{
}
