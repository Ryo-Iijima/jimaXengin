#include "Title.h"
#include "../3d/Object3d.h"
#include "../3d/FbxLoader.h"
#include "../2d/Texture.h"
#include "../general/Sound.h"

void JimaXengine::Title::BgScroll()
{
	// éËëOÇÃë—
	bg.uvPos += bg.vel;
	if (bg.uvPos.x >= Texture::GetMetadata("title_bg.png").width)
	{
		bg.uvPos = Vector2(0, 0);
	}

	// å„ÇÎÇÃésèºñÕól
	band.uvPos += band.vel;
	if (band.uvPos.x >= Texture::GetMetadata("title_bg2.png").width)
	{
		band.uvPos = Vector2(0, 0);
	}
}

void JimaXengine::Title::simpleStaging()
{
	fadeTexColor.w += 0.05f;
	if (fadeTexColor.w >= 1)
	{
		fadeTexColor.w = 1;
		stagingEnd = true;
	}
}

JimaXengine::Title::Title(WinApp* winapp)
{
}

JimaXengine::Title::~Title()
{
}

void JimaXengine::Title::Initialize()
{
	isEnd = false;
	nowScene = "Title";
	nextScene = "Play";

	// å„ÇÎÇÃésèºñÕól
	bg.obj2d.CreateSprite();
	bg.pos = Vector2(0, 0);
	bg.uvPos = Vector2(0, 0);
	bg.vel = Vector2(10.0f / 9, 10.0f / -16);
	bg.scale = Vector2(1.0f * 2.0f / 3.0f, 1.0f * 2.0f / 3.0f);

	// éËëOÇÃë—
	band.obj2d.CreateSprite();
	band.pos = Vector2(0, 0);
	band.uvPos = Vector2(0, 0);
	band.vel = Vector2(1, 0);
	band.scale = Vector2(1.0f * 2.0f / 3.0f, 1.0f * 2.0f / 3.0f);

	pushStartTex = std::make_unique<Object2d>();
	pushStartTex->CreateSprite();
	pushStartTexPos = Vector2(WinApp::WINDOW_WIDTH / 2, WinApp::WINDOW_HEIGHT / 6 * 5);
	pushStartTexScale = Vector2(1.0f / 3.0f, 1.0f / 3.0f);

	buttonFrontTex = std::make_unique<Object2d>();
	buttonFrontTex->CreateSprite();


	titleTex = std::make_unique<Object2d>();
	titleTex->CreateSprite();
	titleTexScale= Vector2(1.0f * 2.0f / 3.0f, 1.0f * 2.0f / 3.0f);
	titleTexPos = Vector2(0, 0);
	
	selected = false;

	Sound::PlayforBuff("_UI_select.wav", 1.0f);

	fadeTex = std::make_unique<Object2d>();
	fadeTex->CreateSprite();

	fadeTexColor = Vector4(1, 1, 1, 0);
	stagingStart = false;
	stagingEnd = false;
	//sound->PlayforBuff("_title.wav");
}

void JimaXengine::Title::Update()
{
	BgScroll();

	if (Input::KeyTrigger(DIK_1)||Input::KeyTrigger(DIK_SPACE))
	{
		selected = true;
		Sound::PlayforBuff("_UI_decision.wav", 1.0f);
	}

	if (selected)
	{
		damageCount += increase;

		if (damageCount > damageTime)
		{
			damageCount = 1;
			selected = false;
			stagingStart = true;
		}
	}

	if (stagingStart)
	{
		simpleStaging();
	}

	// ëJà⁄ââèoÇ™èIóπÇµÇΩÇÁ
	if (stagingEnd)
	{
		ShutDown();
	}
}

void JimaXengine::Title::Draw()
{
	// ésèºñÕól
	Vector2 a = Vector2((float)(Texture::GetMetadata("title_bg.png").width), (float)(Texture::GetMetadata("title_bg.png").height));
	bg.obj2d.DrawRect("title_bg.png", bg.pos, bg.uvPos, a, a * bg.scale, 0, "ALPHA");

	// ë—
	a = Vector2((float)(Texture::GetMetadata("title_bg2.png").width), (float)(Texture::GetMetadata("title_bg2.png").height));
	band.obj2d.DrawRect("title_bg2.png", band.pos, band.uvPos, a, a * band.scale, 0.0f, "ALPHA");

	pushStartTex->DrawOriginal("pushstart.png", pushStartTexPos, 0.0f, pushStartTexScale, "ALPHA", Vector2(0.5f, 0.5f));

	if (damageCount % 2 == 0)
	{
		buttonFrontTex->DrawOriginal("buttonfront.png", pushStartTexPos, 0.0f, pushStartTexScale, "ADD", Vector2(0.5f, 0.5f));
	}

	titleTex->DrawOriginal("title.png", titleTexPos, 0.0f, titleTexScale, "ALPHA");

	fadeTex->DrawOriginal("white1x1.png", Vector2(0, 0), 0.0f, Vector2(WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT), "ALPHA", Vector2(0, 0), fadeTexColor);
}
