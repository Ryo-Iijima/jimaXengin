#include "Application.h"
#include "3d/FbxLoader.h"
#include "3d/Object3d.h"
#include "2d/Object2d.h"
#include "TestScene.h"
#include "scene/Title.h"
#include "scene/Play.h"
#include "scene/End.h"
#include "2d/Texture.h"
#include "DirectionalLight.h"
#include "ResourceShader.h"

JimaXengine::Application& JimaXengine::Application::GetInstance()
{
	static Application instance;
	return instance;
}

void JimaXengine::Application::Initialize()
{
	// ゲームウィンドウの作成
	winApp = std::make_unique<WinApp>();
	winApp->CreateGameWindow();

	// DirectX初期化処理
	dxCommon = std::make_unique<DirectXCommon>();
	dxCommon->Initialize(winApp.get());

	ResourceShader::CompileShader("FBX/FBXVS.hlsl", "main", "vs_5_0");
	ResourceShader::CompileShader("FBX/FBXPS.hlsl", "main", "ps_5_0");
	ResourceShader::CompileShader("Sprite/SpritePixelShader.hlsl", "main", "ps_5_0");
	ResourceShader::CompileShader("Sprite/SpriteVertexShader.hlsl", "main", "vs_5_0");
	ResourceShader::CompileShader("PostEffectTest/PostEffectTestPS.hlsl", "main", "ps_5_0");
	ResourceShader::CompileShader("PostEffectTest/PostEffectTestVS.hlsl", "main", "vs_5_0");
	ResourceShader::CompileShader("Geometory/ParticleVS.hlsl", "main", "vs_5_0");
	ResourceShader::CompileShader("Geometory/CubePS.hlsl", "main", "ps_5_0");
	ResourceShader::CompileShader("Geometory/CubeGS.hlsl", "main", "gs_5_0");
	ResourceShader::CompileShader("Geometory/QuadPS.hlsl", "main", "ps_5_0");
	ResourceShader::CompileShader("Geometory/QuadGS.hlsl", "main", "gs_5_0");

	// FPS管理
	fpsManager = std::make_unique<FPSManager>();
	fpsManager->Initialize(60.0f);

	// デバイス渡す
	FbxLoader::GetInstance().Initialize(dxCommon->GetDevice());
	// 
	Object3d::StaticInitialize(dxCommon.get(), winApp.get());
	Object3d::SetDevice(dxCommon->GetDevice());
	Object3d::CreateGraphicsPipeline();
	//
	Object2d::Initialize(dxCommon.get());

	postEffect = std::make_unique<PostEffect>();
	postEffect->Initialize(dxCommon.get());
	//
	GeometoryObject3D::PreInitialize(dxCommon.get());
	//
	Texture::Initialize(dxCommon.get());
	//
	Sound::Initialize();
	//
	ParticleManager::StaticInitialize();
	// 
	LightGroup::StaticInitialize(dxCommon->GetDevice());

	// 入力
	input = std::make_unique<Input>();
	input->Initialize(winApp.get());


	Texture::LoadTexture("white1x1.png");
	Texture::LoadTexture("colorGrid.png");
	Texture::LoadTexture("test_256x256.png");
	Texture::LoadTexture("particle.png");

	Texture::LoadTexture("buttonfront.png");
	Texture::LoadTexture("pushstart.png");
	Texture::LoadTexture("pushbutton.png");
	Texture::LoadTexture("title.png");
	Texture::LoadTexture("damagefilter.png");
	Texture::LoadTexture("playerUI_0.png");
	Texture::LoadTexture("playerUI_1.png");
	Texture::LoadTexture("playerUI_2.png");
	Texture::LoadTexture("playerUI_3.png");
	Texture::LoadTexture("scorebord.png");
	Texture::LoadTexture("stamp_S.png");
	Texture::LoadTexture("stamp_A.png");
	Texture::LoadTexture("stamp_B.png");
	Texture::LoadTexture("stamp_C.png");
	Texture::LoadTexture("stamp_D.png");
	Texture::LoadTexture("title_bg.png");
	Texture::LoadTexture("title_bg2.png");
	Texture::LoadTexture("number.png");

	// サウンドの読み込み
	Sound::LoadWav("Alarm01.wav");
	Sound::LoadWav("_title.wav");
	Sound::LoadWav("_stamp.wav");
	Sound::LoadWav("_UI_decision.wav");
	Sound::LoadWav("_UI_select.wav");
	Sound::LoadWav("_Boss_attack.wav");
	Sound::LoadWav("_Boss_damage.wav");
	Sound::LoadWav("_Boss_tentacles.wav");
	Sound::LoadWav("_Player_damage.wav");
	Sound::LoadWav("_Player_hit.wav");
	Sound::LoadWav("_Player_justmeet.wav");
	Sound::LoadWav("_Player_swing.wav");

	// モデルの読み込み
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("test_robo");

	FbxLoader::GetInstance().LoadModelFromFiletoBuff("octoLeg");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("octotorso");

	FbxLoader::GetInstance().LoadModelFromFiletoBuff("tex_test_20x1");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("joycon");

	FbxLoader::GetInstance().LoadModelFromFiletoBuff("DefaultPlane");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("DefaultBox");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("smooth_sphere");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("boss");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("boss_phon");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("wall");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("ground");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("AimMark");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("seat");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("ball");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("cube");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("octoleg");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("octotorso");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("bat");

	// シーンの設定
	sceneManager = std::make_unique<SceneManager>();
	sceneManager->Add("TestScene", new TestScene());
	sceneManager->Add("Title", new Title(winApp.get()));
	sceneManager->Add("Play", new Play());
	sceneManager->Add("End", new End(winApp.get()));

	sceneManager->Change("Play");

	// imgui
	imguiDev.Initialize(winApp.get(), dxCommon.get());
}

void JimaXengine::Application::Finalize()
{
	// 各種解放処理
	input->Finalize();

	FbxLoader::GetInstance().Finalize();

	// ゲームウィンドウの破棄
	winApp->DestroyGameWindow();
}

void JimaXengine::Application::Run()
{
	while (true)
	{
		// メッセージ処理
		if (winApp->ProcessMessage())
		{
			break;
		}

		fpsManager->Update();

		input->Update();

		sceneManager->Update();

		imguiDev.Update();

		// レンダーテクスチャへの描画
		postEffect->PreDrawScene();
		
		sceneManager->Draw();
		
		postEffect->PostDrawScene();

		// 描画前処理
		dxCommon->PreDraw();

		// ポストエフェクトの描画
		postEffect->Draw();

		imguiDev.Draw();

		// 描画後処理
		dxCommon->PostDraw();
	}
}