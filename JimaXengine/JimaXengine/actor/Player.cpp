#include "Player.h"
#include "../3d/FbxLoader.h"
#include "../general/Input.h"
#include <typeinfo>

#include "../general/_StringFormat.h"

#define JOYCON_L_PRODUCT_ID 8198
#define JOYCON_R_PRODUCT_ID 8199
// 送るデバイス、コマンド、サブコマンド
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

void JimaXengine::Player::JoyConInitialize()
{
    int globalCount = 0;
    device = hid_enumerate(0, 0);

    while (device)
    {
        if (device->product_id == JOYCON_L_PRODUCT_ID || device->product_id == JOYCON_R_PRODUCT_ID)
        {
            // プロダクトID等を指定して、HID deviceをopenする。そうすると、そのhidデバイスの情報が載ったhid_deviceが帰ってくる。
            dev = hid_open(device->vendor_id, device->product_id, device->serial_number);
            // 今開いているデバイスのプロダクト名の取得。
            printf("\nproduct_id: %ls", device->product_string);

            uint8_t data[0x01];

            data[0] = 0x01;
            // 0x30番のサブコマンドに、0x01を送信する。
            // ランプはビットフラグで、4桁。ランプの一番上から10進数で 1, 2, 4, 8 と対応している。
            SendSubcommand(dev, 0x30, data, 1, &globalCount);

            // read input report
            //uint8_t buff[0x40];
            memset(buff, 0x40, size_t(0x40));
            // 読み込むサイズを指定。
            size = 49;
            // buff に input report が入る。
            int ret = hid_read(dev, buff, size);
            printf("\ninput report id: %d\n", *buff);
            // ボタンの押し込みがビットフラグで表現されている。
            printf("input report id: %d\n", buff[5]);

            // ------ ボタンの入力を任意のタイミングで取得する。------
            // ノンブロッキングをONにする。
            hid_set_nonblocking(dev, 1);

            // imuを有効にする
            data[0] = 0x01;
            SendSubcommand(dev, 0x40, data, 1, &globalCount);

            Sleep(100);

            //data[0] = 0x3F;     // 入力したら送ってくる
            data[0] = 0x30;     // ずっと送ってくる
            SendSubcommand(dev, 0x03, data, 1, &globalCount);


            // ボタンの入力を受け付ける
            //while (true)
            //{
            //    // input report を受けとる。
            //    int ret = hid_read(dev, buff, size);

            //    // input report の id が 0x3F のものに絞る。
            //    if (*buff == 0x3F)
            //    {
            //        // input report の id　を表示。
            //        printf("\ninput report id: %d\n", *buff);
            //        // ボタンのビットビットフィールドを表示。
            //        printf("button byte 1: %d\n", buff[1]);
            //        printf("button byte 2: %d\n", buff[2]);
            //        // スティックの状態を表示。
            //        printf("stick  byte 3: %d\n", buff[3]);
            //    }

            //    //data[0] = 0x30;
            //    //SendSubcommand(dev, 0x03, data, 1, &globalCount);

            //    //// input report の id が 0x30 のものに絞る。
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

void JimaXengine::Player::JoyConUpdate()
{
    // input report を受けとる。
    if (dev)
    {
        int ret = hid_read(dev, buff, size);

        // ボタン
        //printf("button byte 1: %d\n", buff[1]);

        // accel
        accel.x = buff[13] | buff[14] << 8;
        accel.y = buff[15] | buff[16] << 8;
        accel.z = buff[17] | buff[18] << 8;

        //printf("%d\n", accel.z);

        Vector3 cal_acc_coeff = { 350,0,4081 };       // オフセット量
        Vector3 cal_acc_origin = { 18,75,4100 };      // コントローラー水平時のセンサーの値
        Vector3 acc_coeff;
        acc_coeff.x = (float)((1.0 / (cal_acc_coeff.x - cal_acc_origin.x)) * 4.0f);
        acc_coeff.y = (float)((1.0 / (cal_acc_coeff.y - cal_acc_origin.y)) * 4.0f);
        acc_coeff.z = (float)((1.0 / (cal_acc_coeff.z - cal_acc_origin.z)) * 4.0f);

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

        Vector3 cal_gyro_coeff = { 350,0,4081 };      // オフセット量
        Vector3 cal_gyro_offset = { 24,-19,-27 };     // コントローラー水平時のセンサーの値
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

void JimaXengine::Player::Move()
{
#pragma region Stick
    float defaultvel = 1.0f;
    float maxacc = 0.5f;    // 最大加速度
    float xLimit = 8.0f;    // 画面内に制限する用
    float yLimit = 4.0f;

    // 左
    leftRacket->vel = Vector3(0, 0, 0);

    if (Input::KeyPress(DIK_W) || Input::PadLeftStickUp()) leftRacket->vel.y = defaultvel;
    if (Input::KeyPress(DIK_S) || Input::PadLeftStickDown()) leftRacket->vel.y = -defaultvel;
    if (Input::KeyPress(DIK_A) || Input::PadLeftStickLeft()) leftRacket->vel.x = -defaultvel;
    if (Input::KeyPress(DIK_D) || Input::PadLeftStickRight()) leftRacket->vel.x = defaultvel;

    leftRacket->acc += leftRacket->vel / 10;

    // 操作されていたら
    if (Input::KeyPress(DIK_W) || Input::PadLeftStickUp()
        || Input::KeyPress(DIK_S) || Input::PadLeftStickDown())
    {
        // 最高速までに制限（現在の加速方向によって分岐）
        if (leftRacket->acc.y > maxacc)
        {
            leftRacket->acc.y = maxacc;
        }
        else if (leftRacket->acc.y < -maxacc)
        {
            leftRacket->acc.y = -maxacc;
        }
    }
    // 操作されていなかったら
    else
    {
        // 減速（現在の加速方向によって分岐）
        if (leftRacket->acc.y > 0)
        {
            leftRacket->acc.y -= defaultvel / 10;
            if (leftRacket->acc.y <= 0.1f)
            {
                leftRacket->acc.y = 0;
            }
        }
        else if (leftRacket->acc.y < 0)
        {
            leftRacket->acc.y += defaultvel / 10;
            if (leftRacket->acc.y >= -0.1)
            {
                leftRacket->acc.y = 0;
            }
        }
    }

    // 操作されていたら
    if(Input::KeyPress(DIK_A) || Input::PadLeftStickLeft()
        || Input::KeyPress(DIK_D) || Input::PadLeftStickRight())
    {
        // 最高速までに制限（現在の加速方向によって分岐）
        if (leftRacket->acc.x > maxacc)
        {
            leftRacket->acc.x = maxacc;
        }
        else if (leftRacket->acc.x < -maxacc)
        {
            leftRacket->acc.x = -maxacc;
        }
    }
    // 操作されていなかったら
    else
    {
        // 減速（現在の加速方向によって分岐）
        if (leftRacket->acc.x > 0)
        {
            leftRacket->acc.x -= defaultvel / 10;
            if (leftRacket->acc.x <= 0.1f)
            {
                leftRacket->acc.x = 0;
            }
        }
        else if (leftRacket->acc.x < 0)
        {
            leftRacket->acc.x += defaultvel / 10;
            if (leftRacket->acc.x >= -0.1)
            {
                leftRacket->acc.x = 0;
            }
        }
    }

    leftRacket->pos += leftRacket->acc;

    // 画面外に出ないように
    if (leftRacket->pos.x > xLimit)
    {
        leftRacket->pos.x = xLimit;
    }
    if (leftRacket->pos.x < -xLimit)
    {
        leftRacket->pos.x = -xLimit;
    }
    if (leftRacket->pos.y > yLimit)
    {
        leftRacket->pos.y = yLimit;
    }
    if (leftRacket->pos.y < -yLimit)
    {
        leftRacket->pos.y = -yLimit;
    }

    // 右
    rightRacket->vel = Vector3(0, 0, 0);
    if (Input::KeyPress(DIK_I) || Input::PadRightStickUp()) rightRacket->vel.y = defaultvel;
    if (Input::KeyPress(DIK_K) || Input::PadRightStickDown()) rightRacket->vel.y = -defaultvel;
    if (Input::KeyPress(DIK_J) || Input::PadRightStickLeft()) rightRacket->vel.x = -defaultvel;
    if (Input::KeyPress(DIK_L) || Input::PadRightStickRight()) rightRacket->vel.x = defaultvel;

    rightRacket->acc += rightRacket->vel / 10;

    // 操作されていたら
    if (Input::KeyPress(DIK_I) || Input::PadRightStickUp()
        || Input::KeyPress(DIK_K) || Input::PadRightStickDown())
    {
        // 最高速までに制限（現在の加速方向によって分岐）
        if (rightRacket->acc.y > maxacc)
        {
            rightRacket->acc.y = maxacc;
        }
        else if (rightRacket->acc.y < -maxacc)
        {
            rightRacket->acc.y = -maxacc;
        }
    }
    // 操作されていなかったら
    else
    {
        // 減速（現在の加速方向によって分岐）
        if (rightRacket->acc.y > 0)
        {
            rightRacket->acc.y -= defaultvel / 10;
            if (rightRacket->acc.y <= 0.1f)
            {
                rightRacket->acc.y = 0;
            }
        }
        else if (rightRacket->acc.y < 0)
        {
            rightRacket->acc.y += defaultvel / 10;
            if (rightRacket->acc.y >= -0.1)
            {
                rightRacket->acc.y = 0;
            }
        }
    }

    // 操作されていたら
    if (Input::KeyPress(DIK_J) || Input::PadRightStickLeft()
        || Input::KeyPress(DIK_L) || Input::PadRightStickRight())
    {
        // 最高速までに制限（現在の加速方向によって分岐）
        if (rightRacket->acc.x > maxacc)
        {
            rightRacket->acc.x = maxacc;
        }
        else if (rightRacket->acc.x < -maxacc)
        {
            rightRacket->acc.x = -maxacc;
        }
    }
    // 操作されていなかったら
    else
    {
        // 減速（現在の加速方向によって分岐）
        if (rightRacket->acc.x > 0)
        {
            rightRacket->acc.x -= defaultvel / 10;
            if (rightRacket->acc.x <= 0.1f)
            {
                rightRacket->acc.x = 0;
            }
        }
        else if (rightRacket->acc.x < 0)
        {
            rightRacket->acc.x += defaultvel / 10;
            if (rightRacket->acc.x >= -0.1)
            {
                rightRacket->acc.x = 0;
            }
        }
    }

    rightRacket->pos += rightRacket->acc;

    // 画面外に出ないように
    if (rightRacket->pos.x > xLimit)
    {
        rightRacket->pos.x = xLimit;
    }
    if (rightRacket->pos.x < -xLimit)
    {
        rightRacket->pos.x = -xLimit;
    }
    if (rightRacket->pos.y > yLimit)
    {
        rightRacket->pos.y = yLimit;
    }
    if (rightRacket->pos.y < -yLimit)
    {
        rightRacket->pos.y = -yLimit;
    }

#pragma endregion
}

JimaXengine::Player::Player(Camera* camera)
{
    pCamera = camera;
}

JimaXengine::Player::~Player()
{
	delete object;
	delete model;

    delete device;
    //delete dev;

}

void JimaXengine::Player::Initialize()
{
	model = FbxLoader::GetInstance().LoadModelFromFile("DefaultBox");
	//model = FbxLoader::GetInstance().LoadModelFromFile("Stand");
	object = new Object3d;
	object->Initialize();
	object->SetModel(model);
    
    pos = Vector3(5, 0, 0);
	object->SetPosition(pos);
    object->SetScale(Vector3(4, 0.5, 1));

    eye = { 0,0,-10 };
    target = { 0,0,100 };
    pCamera->SetViewMatrix(eye, target);

    renderType = RENDER_TYPE::RENDER_TYPE_ALPHA_TEST;
    //JoyConInitialize();

#pragma region Stick初期化

    leftRacket = std::make_unique<Racket>();

    leftRacket->pos = Vector3(-5, 0, 0);

    leftRacket->object = std::make_unique<Object3d>();
    leftRacket->object->Initialize();
    leftRacket->object->SetModel(model);
    leftRacket->object->SetColor(Vector4(0, 0, 1, 0.7f));

    leftRacket->col.minPos = Vector3(leftRacket->pos.x - 1, leftRacket->pos.y - 1, leftRacket->pos.z - 1);
    leftRacket->col.maxPos = Vector3(leftRacket->pos.x + 1, leftRacket->pos.y + 1, leftRacket->pos.z + 1);


    rightRacket = std::make_unique<Racket>();

    rightRacket->pos = Vector3(5, 0, 0);

    rightRacket->object = std::make_unique<Object3d>();
    rightRacket->object->Initialize();
    rightRacket->object->SetModel(model);
    rightRacket->object->SetColor(Vector4(1, 0, 0, 0.7f));

    rightRacket->col.minPos = Vector3(rightRacket->pos.x - 1, rightRacket->pos.y - 1, rightRacket->pos.z - 1);
    rightRacket->col.maxPos = Vector3(rightRacket->pos.x + 1, rightRacket->pos.y + 1, rightRacket->pos.z + 1);

#pragma endregion

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

void JimaXengine::Player::Update()
{
    {
        //{   // ポインターテスト
//    uint8_t* buf_ = new uint8_t[5]{ 0,1,2,3,4 };
//   
//    printf("buf   : %d\n", buf_);
//    printf("buf+1 : %d\n", (buf_ + 1));
//    printf("buf+2 : %d\n", (buf_ + 2));
//    printf("buf+3 : %d\n", (buf_ + 3));
//    printf("buf+4 : %d\n", (buf_ + 4));
//    printf("sizeof(buf) : %d\n", sizeof(buf_));

//}

//{   // 変数の型の比較
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

    }
    
    Move();

    object->SetPosition(pos);
    object->SetCamera(pCamera);
    object->Update();

    layObj->SetPosition(pos);
    layObj->SetRotation(Vector3(rotation.x, rotation.z, rotation.y));
    layObj->SetCamera(pCamera);
    layObj->Update();

    sphereCol.center = pos.ConvertXMVECTOR();

#pragma region Stick
    // left
    leftRacket->object->SetPosition(leftRacket->pos);
    leftRacket->object->SetCamera(pCamera);
    leftRacket->object->Update();

    leftRacket->col.minPos = Vector3(leftRacket->pos.x - 1, leftRacket->pos.y - 1, leftRacket->pos.z - 1);
    leftRacket->col.maxPos = Vector3(leftRacket->pos.x + 1, leftRacket->pos.y + 1, leftRacket->pos.z + 1);
    // right
    rightRacket->object->SetPosition(rightRacket->pos);
    rightRacket->object->SetCamera(pCamera);
    rightRacket->object->Update();

    rightRacket->col.minPos = Vector3(rightRacket->pos.x - 1, rightRacket->pos.y - 1, rightRacket->pos.z - 1);
    rightRacket->col.maxPos = Vector3(rightRacket->pos.x + 1, rightRacket->pos.y + 1, rightRacket->pos.z + 1);
#pragma endregion
}

void JimaXengine::Player::Draw()
{
	//object->Draw();
	//layObj->Draw();

#pragma region Stick
    leftRacket->object->Draw();
    rightRacket->object->Draw();
#pragma endregion

}

JimaXengine::GameObject::TYPE JimaXengine::Player::GetType()
{
    return GameObject::TYPE::PLAYER;
}

void JimaXengine::Player::DrawImGui()
{
    ImGui::SetNextWindowPos(ImVec2(20, 20), 1 << 1);
    ImGui::SetNextWindowSize(ImVec2(250, 300), 1 << 1);

    ImGui::Begin("PlayerInfomation");
    ImGui::Text("Lpos : %f,%f,%f", leftRacket->pos.x, leftRacket->pos.y, leftRacket->pos.z);
    ImGui::Text("Rpos : %f,%f,%f", rightRacket->pos.x, rightRacket->pos.y, rightRacket->pos.z);
    ImGui::End();
}

JimaXengine::Camera* JimaXengine::Player::GetCamera()
{
    return pCamera;
}

void JimaXengine::Player::DrawAlphaObj()
{
}