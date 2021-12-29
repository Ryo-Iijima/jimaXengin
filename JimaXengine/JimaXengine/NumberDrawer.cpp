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
    // ������ݒ�
    tex.resize(digit);
    eachNum.resize(digit);

    for (int i = 0; i < digit; i++)
    {
        // ������������
        tex[i] = std::make_unique<Object2d>();
        tex[i]->CreateSprite();
        // �S��0�Ŗ��߂Ă���
        eachNum[i] = 0;
    }
}

void JimaXengine::NumberDrawer::Update()
{
    // �\�����鐔���̃f�[�^���擾���Đ��񂷂�
    // �\�����������l�̌������擾
    int digitNum = General::GetDigit(number);
    // �����Ƃɐ��l���L�^
    int result = 0;
    int numStock = number;

    for (int i = 0; i < digitNum; i++)
    {
        // ���̂��܂肩�琔����ǂݎ��
        result = numStock % 10;
        // ����10�Ŋ��������̂��g��
        numStock /= 10;

        eachNum[i] = result;
    }
}

void JimaXengine::NumberDrawer::Draw()
{
    Update();

    for (int i = 0; i < digit; i++)
    {
        // ���������炷
        data.pos = referrencePos - Vector2(data.size.x * i, 0.0f);
        // �ۑ����������̃f�[�^����؂���͈͂�����
        data.uvPos = Vector2(data.length.x * eachNum[i], 0);

        tex[i]->DrawRect(data);
    }
}
