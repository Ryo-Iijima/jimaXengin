#include "Title.h"
#include "../3d/Object3d.h"
#include "../3d/FbxLoader.h"

void JimaXengine::Title::BgScroll()
{
	// éËëOÇÃë—
	for (int i = 0; i < sizeof(bgBand) / sizeof(bgBand[0]); i++)
	{
		if (bgBand[i].pos.x <= -WinApp::WINDOW_WIDTH)
		{
			bgBand[i].pos = Vector2(WinApp::WINDOW_WIDTH, 0);
		}
		bgBand[i].pos += bgBand[i].vel;
	}

	// å„ÇÎÇÃésèºñÕól
	for (int i = 0; i < sizeof(bg) / sizeof(bg[0]); i++)
	{
		if (bg[i].pos.y >= WinApp::WINDOW_HEIGHT)
		{
			bg[0].pos = Vector2(0, 0 );
			bg[1].pos = Vector2(WinApp::WINDOW_WIDTH, 0 );
			bg[2].pos = Vector2( 0, -WinApp::WINDOW_HEIGHT );
			bg[3].pos = Vector2( WinApp::WINDOW_WIDTH, -WinApp::WINDOW_HEIGHT );
		}
		bg[i].pos += bg[i].vel;
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
	for (int i = 0; i < sizeof(bg) / sizeof(bg[0]); i++)
	{
		bg[i].obj2d.CreateSprite();
		bg[i].vel = Vector2(10.0f / -9, 10.0f / 16);
		bg[i].scale = Vector2(1.0f * 2.0f / 3.0f, 1.0f * 2.0f / 3.0f);
	}
	bg[0].pos = Vector2( 0, 0 );
	bg[1].pos = Vector2( WinApp::WINDOW_WIDTH, 0 );
	bg[2].pos = Vector2( 0, -WinApp::WINDOW_HEIGHT );
	bg[3].pos = Vector2( WinApp::WINDOW_WIDTH, -WinApp::WINDOW_HEIGHT );

	// éËëOÇÃë—
	for (int i = 0; i < sizeof(bgBand) / sizeof(bgBand[0]); i++)
	{
		bgBand[i].obj2d.CreateSprite();
		bgBand[i].vel = Vector2(-1, 0);
		bgBand[i].scale = Vector2(1.0f * 2.0f / 3.0f, 1.0f * 2.0f / 3.0f);

	}
	bgBand[0].pos = Vector2(0, 0);
	bgBand[1].pos = Vector2(WinApp::WINDOW_WIDTH, 0);

	pushStartTex = std::make_unique<Object2d>();
	pushStartTex->CreateSprite();
	pushStartTexPos = Vector2(WinApp::WINDOW_WIDTH / 2, WinApp::WINDOW_HEIGHT / 6 * 5);
	pushStartTexScale = Vector2(1.0f / 3.0f, 1.0f / 3.0f);

	buttonFrontTex = std::make_unique<Object2d>();
	buttonFrontTex->CreateSprite();


	titleTex = std::make_unique<Object2d>();
	titleTex->CreateSprite();
	titleTexPos = Vector2(WinApp::WINDOW_WIDTH / 2,WinApp::WINDOW_HEIGHT / 4 );
	
	selected = false;

	sound = new Sound;
	//sound->PlayforBuff("Resources/sound/Alarm01.wav");
	sound->PlayforBuff("_title.wav");

}

void JimaXengine::Title::Update()
{
	sound->PlayforBuff("_hit.wav");

	BgScroll();

	if (Input::KeyTrigger(DIK_SPACE))
	{
		selected = true;
	}

	if (selected)
	{
		damageCount += increase;

		if (damageCount > damageTime)
		{
			damageCount = 1;
			selected = false;
		}
	}

	if (Input::KeyTrigger(DIK_1))
	{
		ShutDown();
	}
}

void JimaXengine::Title::Draw()
{
	// ésèºñÕól
	for (int i = 0; i < sizeof(bg) / sizeof(bg[0]); i++)
	{
		bg[i].obj2d.DrawOriginal("title_bg.png", bg[i].pos, 0.0f, bg[i].scale, "ALPHA");
	}
	// ë—
	for (int i = 0; i < sizeof(bgBand) / sizeof(bgBand[0]); i++)
	{
		bgBand[i].obj2d.DrawOriginal("title_bg2.png", bgBand[i].pos, 0.0f, bgBand[i].scale, "ALPHA");
	}

	pushStartTex->DrawOriginal("pushstart.png", pushStartTexPos, 0.0f, pushStartTexScale, "ALPHA", Vector2(0.5f, 0.5f));

	if (damageCount % 2 == 0)
	{
		buttonFrontTex->DrawOriginal("buttonfront.png", pushStartTexPos, 0.0f, pushStartTexScale, "ADD", Vector2(0.5f, 0.5f));
	}

	titleTex->DrawOriginal("title.png", titleTexPos, 0.0f, Vector2(1.0f, 1.0f), "ALPHA", Vector2(0.5f, 0.5f));

}
