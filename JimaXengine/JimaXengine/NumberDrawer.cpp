#include "NumberDrawer.h"
#include "general/General.h"


JimaXengine::NumberDrawer::NumberDrawer(int& number, int digit, Object2d::DrawTextureData data)
    :number(number)
{
    this->digit = digit;
    this->data = data;
    referrencePos = data.pos;
}

void JimaXengine::NumberDrawer::Initialize()
{
    // 長さを設定
    tex.resize(digit);
    eachNum.resize(digit);

    for (int i = 0; i < digit; i++)
    {
        // 桁数分初期化
        tex[i] = std::make_unique<Object2d>();
        tex[i]->CreateSprite();
        // 全桁0で埋めておく
        eachNum[i] = 0;
    }
}

void JimaXengine::NumberDrawer::Update()
{
    // 表示する数字のデータを取得して整列する
    // 表示したい数値の桁数を取得
    int digitNum = General::GetDigit(number);
    // 桁ごとに数値を記録
    int result = 0;
    int numStock = number;

    for (int i = 0; i < digitNum; i++)
    {
        // 商のあまりから数字を読み取る
        result = numStock % 10;
        // 次は10で割ったものを使う
        numStock /= 10;

        eachNum[i] = result;
    }
}

void JimaXengine::NumberDrawer::Draw()
{
    Update();

    for (int i = 0; i < digit; i++)
    {
        // 桁数分ずらす
        data.pos = referrencePos - Vector2(data.size.x * i, 0.0f);
        // 保存した桁毎のデータから切り取り範囲を決定
        data.uvPos = Vector2(data.length.x * eachNum[i], 0);

        tex[i]->DrawRect(data);
    }
}
