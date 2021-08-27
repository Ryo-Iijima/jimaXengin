#include "Player.h"
#include "3d/Object3d.h"
#include "3d/FbxLoader.h"
#include "general/Input.h"

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

void Player::JoyConInitialize()
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
            // 0x30番のサブコマンドに、0x01を送信します。
            // ランプはビットフラグで、4桁。ランプの一番上から10進数で 1, 2, 4, 8 と対応しています。
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
            //    //if (*buff != 0x3F)
            //    //{
            //    //    continue;
            //    //}

            //    // input report の id が 0x30 のものに絞る。
            //    if (*buff != 0x30)
            //    {
            //        continue;
            //    }

            //    // input report の id　を表示。
            //    printf("\ninput report id: %d\n", *buff);
            //    //// ボタンのビットビットフィールドを表示。
            //    //printf("button byte 1: %d\n", buff[1]);
            //    //printf("button byte 2: %d\n", buff[2]);
            //    //// スティックの状態を表示。
            //    //printf("stick  byte 3: %d\n", buff[3]);
            //    //for (int i = 1; i <= 15; i++)
            //    //{
            //    //    printf("data byte %d: %d\n", i, buff[i]);

            //    //}

            //    accel.x = buff[13];
            //    accel.y = buff[15];
            //    accel.z = buff[17];

            //    gyro.x = buff[19];
            //    gyro.y = buff[21];
            //    gyro.z = buff[23];

            //    //printf("accel: %f, %f, %f\n", accel.x, accel.y, accel.z);
            //    //printf("gyro: %f, %f, %f\n", gyro.x, gyro.y, gyro.z);

            //    printf("%f\n", accel.x * 0.000244f);

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
    // input report を受けとる。
    if (dev)
    {
        int ret = hid_read(dev, buff, size);

        // input report の id が 0x3F のものに絞る。
        //if (*buff != 0x3F)
        //{
        //    continue;
        //}

        // input report の id が 0x30 のものに絞る。
        //if (*buff != 0x30)
        //{
        //    continue;
        //}

        // input report の id　を表示。
        printf("\ninput report id: %d\n", *buff);
        //// ボタンのビットビットフィールドを表示。
        //printf("button byte 1: %d\n", buff[1]);
        //printf("button byte 2: %d\n", buff[2]);
        //// スティックの状態を表示。
        //printf("stick  byte 3: %d\n", buff[3]);
        //for (int i = 1; i <= 15; i++)
        //{
        //    printf("data byte %d: %d\n", i, buff[i]);

        //}

        accel.x = buff[13];
        accel.y = buff[15];
        accel.z = buff[17];
        //accel.x = buff[14];
        //accel.y = buff[16];
        //accel.z = buff[18];

        gyro.x = buff[19];
        gyro.y = buff[21];
        gyro.z = buff[23];
        //gyro.x = buff[20];
        //gyro.y = buff[22];
        //gyro.z = buff[24];

        //printf("accel: %f, %f, %f\n", accel.x, accel.y, accel.z);
        //printf("gyro: %f, %f, %f\n", gyro.x, gyro.y, gyro.z);

        printf("%f\n", gyro.x);

        //Sleep(500);

    }


    cur_gyro_x = gyro.x;

    //gyro_x += (pre_gyro_x - cur_gyro_x);
    gyro_x = cur_gyro_x;

    object->SetRotation(Vector3(gyro_x, 0, 0));

    pre_gyro_x = cur_gyro_x;

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

Player::Player()
{

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
	model = FbxLoader::GetInstance().LoadModelFromFile("cube");
	object = new Object3d;
	object->Initialize();
	object->SetModel(model);

	object->SetPosition(Vector3(10, 0, 0));


    //JoyConInitialize();

}

void Player::Update()
{
    //JoyConUpdate();
    
    Move();

    object->SetPosition(pos);
    object->Update();

}

void Player::Draw()
{
	object->Draw();
}

void Player::DrawAlphaObj()
{
}
