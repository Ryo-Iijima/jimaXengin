#include "Player.h"
#include "../3d/FbxLoader.h"
#include "../general/Input.h"
#include <typeinfo>

#include "../general/_StringFormat.h"

#define JOYCON_L_PRODUCT_ID 8198
#define JOYCON_R_PRODUCT_ID 8199
// ����f�o�C�X�A�R�}���h�A�T�u�R�}���h
void SendSubcommand(hid_device* dev, uint8_t command, uint8_t subcommand[], int len, int* globalCount)
{
    uint8_t buf[0x40];
    memset(buf, 0x0, size_t(0x40));
    
    buf[0] = 1; // 0x10 for rumble only
    buf[1] = *globalCount; // Increment by 1 for each packet sent. It loops in 0x0 - 0xF range.

    if (*globalCount == 0xf0) {
        *globalCount = 0x00;
    }
    else {
        *globalCount++;
    }

    buf[10] = command;
    memcpy(buf + 11, subcommand, len);

    hid_write(dev, buf, 0x40);
}

void Player::JoyConInitialize()
{
    int globalCount = 0;
    device = hid_enumerate(0, 0);

    while (device)
    {
        if (device->product_id == JOYCON_L_PRODUCT_ID || device->product_id == JOYCON_R_PRODUCT_ID)
        {
            // �v���_�N�gID�����w�肵�āAHID device��open����B��������ƁA����hid�f�o�C�X�̏�񂪍ڂ���hid_device���A���Ă���B
            dev = hid_open(device->vendor_id, device->product_id, device->serial_number);
            // ���J���Ă���f�o�C�X�̃v���_�N�g���̎擾�B
            printf("\nproduct_id: %ls", device->product_string);

            uint8_t data[0x01];

            data[0] = 0x01;
            // 0x30�Ԃ̃T�u�R�}���h�ɁA0x01�𑗐M����B
            // �����v�̓r�b�g�t���O�ŁA4���B�����v�̈�ԏォ��10�i���� 1, 2, 4, 8 �ƑΉ����Ă���B
            SendSubcommand(dev, 0x30, data, 1, &globalCount);

            // read input report
            //uint8_t buff[0x40];
            memset(buff, 0x40, size_t(0x40));
            // �ǂݍ��ރT�C�Y���w��B
            size = 49;
            // buff �� input report ������B
            int ret = hid_read(dev, buff, size);
            printf("\ninput report id: %d\n", *buff);
            // �{�^���̉������݂��r�b�g�t���O�ŕ\������Ă���B
            printf("input report id: %d\n", buff[5]);

            // ------ �{�^���̓��͂�C�ӂ̃^�C�~���O�Ŏ擾����B------
            // �m���u���b�L���O��ON�ɂ���B
            hid_set_nonblocking(dev, 1);

            // imu��L���ɂ���
            data[0] = 0x01;
            SendSubcommand(dev, 0x40, data, 1, &globalCount);

            Sleep(100);

            //data[0] = 0x3F;     // ���͂����瑗���Ă���
            data[0] = 0x30;     // �����Ƒ����Ă���
            SendSubcommand(dev, 0x03, data, 1, &globalCount);


            // �{�^���̓��͂��󂯕t����
            //while (true)
            //{
            //    // input report ���󂯂Ƃ�B
            //    int ret = hid_read(dev, buff, size);

            //    // input report �� id �� 0x3F �̂��̂ɍi��B
            //    if (*buff == 0x3F)
            //    {
            //        // input report �� id�@��\���B
            //        printf("\ninput report id: %d\n", *buff);
            //        // �{�^���̃r�b�g�r�b�g�t�B�[���h��\���B
            //        printf("button byte 1: %d\n", buff[1]);
            //        printf("button byte 2: %d\n", buff[2]);
            //        // �X�e�B�b�N�̏�Ԃ�\���B
            //        printf("stick  byte 3: %d\n", buff[3]);
            //    }

            //    //data[0] = 0x30;
            //    //SendSubcommand(dev, 0x03, data, 1, &globalCount);

            //    //// input report �� id �� 0x30 �̂��̂ɍi��B
            //    //if (*buff == 0x30)
            //    //{
            //    //    printf("gyro.x: %d\n", buff[19] | buff[20] << 8);
            //    //}


            //    Sleep(1000);

            //}

            //break;
        }
        device = device->next;
    }
    hid_free_enumeration(device);

}

void Player::JoyConUpdate()
{
    // input report ���󂯂Ƃ�B
    if (dev)
    {
        int ret = hid_read(dev, buff, size);

        // �{�^��
        //printf("button byte 1: %d\n", buff[1]);

        // accel
        accel.x = buff[13] | buff[14] << 8;
        accel.y = buff[15] | buff[16] << 8;
        accel.z = buff[17] | buff[18] << 8;

        //printf("%d\n", accel.z);

        Vector3 cal_acc_coeff = { 350,0,4081 };       // �I�t�Z�b�g��
        Vector3 cal_acc_origin = { 18,75,4100 };      // �R���g���[���[�������̃Z���T�[�̒l
        Vector3 acc_coeff;
        acc_coeff.x = (1.0 / (cal_acc_coeff.x - cal_acc_origin.x)) * 4.0f;
        acc_coeff.y = (1.0 / (cal_acc_coeff.y - cal_acc_origin.y)) * 4.0f;
        acc_coeff.z = (1.0 / (cal_acc_coeff.z - cal_acc_origin.z)) * 4.0f;

        Vector3 acc_raw_component = { (float)accel.x ,(float)accel.y ,(float)accel.z };
        Vector3 acc_vector_component;
        acc_vector_component.x = acc_raw_component.x * acc_coeff.x;
        acc_vector_component.y = acc_raw_component.y * acc_coeff.y;
        acc_vector_component.z = acc_raw_component.z * acc_coeff.z;

        acc.x = acc_vector_component.x/50;
        acc.y = acc_vector_component.z / 10000;
        acc.z = acc_vector_component.y/-500;


        // gyro
        gyro.x = buff[19] | buff[20] << 8;
        gyro.y = buff[21] | buff[22] << 8;
        gyro.z = buff[23] | buff[24] << 8;

        Vector3 cal_gyro_coeff = { 350,0,4081 };      // �I�t�Z�b�g��
        Vector3 cal_gyro_offset = { 24,-19,-27 };     // �R���g���[���[�������̃Z���T�[�̒l
        Vector3 gyro_cal_coeff;
        gyro_cal_coeff.x = (936.0f / (cal_gyro_coeff.x - cal_gyro_offset.x));
        gyro_cal_coeff.y = (936.0f / (cal_gyro_coeff.y - cal_gyro_offset.y));
        gyro_cal_coeff.z = (936.0f / (cal_gyro_coeff.z - cal_gyro_offset.z));

        Vector3 gyro_raw_component = { (float)gyro.x, (float)gyro.y, (float)gyro.z };
        Vector3 gyro_vector_component;
        gyro_vector_component.x = (gyro_raw_component.x - cal_gyro_offset.x) * gyro_cal_coeff.x;
        gyro_vector_component.y = (gyro_raw_component.y - cal_gyro_offset.y) * gyro_cal_coeff.y;
        gyro_vector_component.z = (gyro_raw_component.z - cal_gyro_offset.z) * gyro_cal_coeff.z;

        rotation.x += (gyro_vector_component.x / 500);
        rotation.y += (gyro_vector_component.y / 5000);
        rotation.z += (gyro_vector_component.z / -100);

        //Sleep(500);
    }

    vel += acc;
    pos += vel;
    object->SetRotation(Vector3(rotation.x, rotation.z, rotation.y));
    object->SetPosition(pos);

}

void Player::Move()
{
    vel = { 0,0,0 };

    if (Input::KeyPress(DIK_W)) vel.z = 1;
    if (Input::KeyPress(DIK_S)) vel.z = -1;
    if (Input::KeyPress(DIK_A)) vel.x = -1;
    if (Input::KeyPress(DIK_D)) vel.x = 1;

    pos += vel;
}

Player::Player(Camera* camera)
{
    pCamera = camera;
}

Player::~Player()
{
	delete object;
	delete model;

    delete device;
    delete dev;

}

void Player::Initialize()
{
	model = FbxLoader::GetInstance().LoadModelFromFile("DefaultBox");
	//model = FbxLoader::GetInstance().LoadModelFromFile("Stand");
	object = new Object3d;
	object->Initialize();
	object->SetModel(model);

    pos = Vector3(5, 0, 0);
	object->SetPosition(pos);
    object->SetScale(Vector3(4, 0.5, 1));

    eye = { 0,20,-50 };
    target = { 0,0,0 };
    pCamera->SetViewMatrix(eye, target);

    //JoyConInitialize();

    model = FbxLoader::GetInstance().LoadModelFromFile("box");
    layObj = new Object3d;
    layObj->Initialize();
    layObj->SetModel(model);

    layObj->SetPosition(pos);
    layObj->SetScale(Vector3(1, 1, 1));
    layObj->SetColor(Vector4(1, 0, 0, 1));

}

enum class DebugType
{
    NONE,
    ALL,
    COMMS,
    THREADING,
    IMU,
    RUMBLE,
};

void DebugPrint(std::string s, DebugType d)
{
    printf("%s\n", s.c_str());
}

void Player::Update()
{
    //{   // �|�C���^�[�e�X�g
    //    uint8_t* buf_ = new uint8_t[5]{ 0,1,2,3,4 };
    //   
    //    printf("buf   : %d\n", buf_);
    //    printf("buf+1 : %d\n", (buf_ + 1));
    //    printf("buf+2 : %d\n", (buf_ + 2));
    //    printf("buf+3 : %d\n", (buf_ + 3));
    //    printf("buf+4 : %d\n", (buf_ + 4));
    //    printf("sizeof(buf) : %d\n", sizeof(buf_));

    //}

    //{   // �ϐ��̌^�̔�r
    //    int a = 0;
    //    if (typeid(a)==typeid(int))
    //    {
    //        printf("int is : %s\n", typeid(int).name());
    //    }
    //}

    //{
    //    DebugType debug_type = DebugType::NONE;
    //    const unsigned int len = 5;
    //    int arr[len]{ 0,1,2,3,4 };
    //    std::string format = "%s";

    //    std::string tostr = "";
    //    for (int i = 0; i < len; ++i)
    //    {
    //        tostr += _StringFormat::Format((typeid(arr[0]) == typeid(int)) ? ((std::string)"%d") : ((typeid(arr[0]) == typeid(float)) ? ((std::string)"%f") : ((std::string)"%s")), arr[i]);
    //    }
    //    DebugPrint(_StringFormat::Format(format, tostr), debug_type);

    //}

    //JoyConUpdate();
    
    Move();

    float out = 0;
    if (Collision::LineToAABB3D(&layCol,&aabb3dCol,&pCamera->GetMatView(),out))
    {
        layObj->SetColor(Vector4(0, 1, 0, 1));
    }

    //object->SetPosition(pos);
    object->SetCamera(pCamera);
    object->Update();

    layObj->SetPosition(pos);
    layObj->SetRotation(Vector3(rotation.x, rotation.z, rotation.y));
    layObj->SetCamera(pCamera);
    layObj->Update();

    sphereCol.center = pos.ConvertXMVECTOR();

}

void Player::Draw()
{
	object->Draw();
	layObj->Draw();
}

GameObject::TYPE Player::GetType()
{
    return GameObject::TYPE::PLAYER;
}

void Player::DrawImGui()
{
    //ImGui::Begin("PlayerInfomation");
    //ImGui::End();
}

Camera* Player::GetCamera()
{
    return pCamera;
}

void Player::DrawAlphaObj()
{
}