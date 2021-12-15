#include "End.h"
#include "../2d/Texture.h"
#include "../imgui/imgui.h"

JimaXengine::End::End(WinApp* winapp)
{
}

JimaXengine::End::~End()
{
}

void JimaXengine::End::Initialize()
{
	Vector2 scale = Vector2(2.0f / 3.0f, 2.0f / 3.0f);

	bordTex = std::make_unique<Object2d>();
	bordTex->CreateSprite();
	bordTexData.name = "scorebord.png";
	bordTexData.pos = Vector2(0, 0);
	bordTexData.angle = 0;
	bordTexData.size = scale;
	bordTexData.type = "ALPHA";

	scale = Vector2(2.0f / (3.0f * 2.5f), 2.0f / (3.0f * 2.5f));
	timeTex = std::make_unique<Object2d>();
	timeTex->CreateSprite();
	timeTexData.name = "number.png";
	timeTexData.pos = Vector2(600, 155);
	timeTexData.uvPos = Vector2((float)(Texture::GetMetadata(timeTexData.name).width) * 0.0f, 0);
	timeTexData.length = Vector2((float)(Texture::GetMetadata(timeTexData.name).width) * 0.1f, (float)(Texture::GetMetadata(timeTexData.name).height));
	timeTexData.size = timeTexData.length * scale;
	timeTexData.angle = 0;
	timeTexData.type = "ALPHA";

	scale = Vector2(2.0f / 3.0f, 2.0f / 3.0f);
	stampTex = std::make_unique<Object2d>();
	stampTex->CreateSprite();
	stampTexData.name = "stamp_S.png";
	stampTexData.pos = Vector2(WinApp::WINDOW_WIDTH / 4 * 3, WinApp::WINDOW_HEIGHT / 4 * 3);
	stampTexData.angle = 0;
	stampTexData.size = Vector2(0.3f, 0.3f) * scale;
	stampTexData.type = "ALPHA";
	stampTexData.anchor = Vector2(0.5f, 0.5f);
	stampTexData.color = Vector4(0.7f, 0, 0, 1);
}

void JimaXengine::End::Update()
{
	{
		if (Input::KeyPress(DIK_UP)) timeTexData.pos.y--;
		if (Input::KeyPress(DIK_DOWN)) timeTexData.pos.y++;
		if (Input::KeyPress(DIK_RIGHT)) timeTexData.pos.x++;
		if (Input::KeyPress(DIK_LEFT)) timeTexData.pos.x--;
		if (Input::KeyPress(DIK_A))
		{
			timeTexData.size.x += 192 / 100;
			timeTexData.size.y += 192 / 100;
		}
		if (Input::KeyPress(DIK_Z))
		{
			timeTexData.size.x -= 192 / 100;
			timeTexData.size.y -= 270 / 100;
		}
	}
}

void JimaXengine::End::Draw()
{
	bordTex->DrawOriginal(bordTexData);

	timeTex->DrawRect(timeTexData);

	stampTex->DrawOriginal(stampTexData);

	ImGui::SetNextWindowPos(ImVec2(920, 20), 1 << 1);
	ImGui::SetNextWindowSize(ImVec2(250, 300), 1 << 1);

	ImGui::Begin("BossInformation");
	ImGui::Text("pos : %f,%f", timeTexData.pos.x, timeTexData.pos.y);
	ImGui::Text("size : %f,%f", timeTexData.size.x, timeTexData.size.y);

	ImGui::End();

}
