#include "TestScene.h"
#include "DebugCamera.h"
#include <fstream>

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
        (*globalCount)++;
    }

    buf[10] = command;
    memcpy(buf + 11, subcommand, len);

    hid_write(dev, buf, 0x40);
}

void JimaXengine::TestScene::JoyConInitialize()
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

void JimaXengine::TestScene::JoyConUpdate()
{
    if (dev)
    {
        // 前フレームの状態を保持
        prev_accel = accel;
        prev_gyro = gyro;

        {// 出力用配列にも記録
            std::string acc = std::to_string(accel.x) + "," + std::to_string(accel.y) + "," + std::to_string(accel.z);
            std::string gyr = std::to_string(gyro.x) + "," + std::to_string(gyro.y) + "," + std::to_string(gyro.z);

            sensorData.push_back(acc + "," + gyr);
        }

        // input report を受けとる。
        int ret = hid_read(dev, buff, size);

        // ボタン
        //printf("button byte 1: %d\n", buff[1]);

#pragma region 加速度

        /////////////////////////////////////////////////////
        //// accel 
        /////////////////////////////////////////////////////

        // 値の取得
        row_accel.x = (float)(buff[13] | buff[14] << 8);
        row_accel.y = (float)(buff[15] | buff[16] << 8);
        row_accel.z = (float)(buff[17] | buff[18] << 8);

        {// 加速度（G）
            accel.x = row_accel.x * 0.00025f;
            accel.y = row_accel.y * 0.00025f;
            accel.z = row_accel.z * 0.00025f;
        }

        //{
        //    // 補正用の計算（上半分は毎回計算する必要はない）
        //    Vector3 cal_acc_coeff = { 350,0,4081 };       // オフセット量
        //    Vector3 cal_acc_origin = { 18,75,4100 };      // コントローラー水平時のセンサーの値
        //    Vector3 acc_coeff;
        //    acc_coeff.x = (float)((1.0 / (cal_acc_coeff.x - cal_acc_origin.x)) * 4.0f);
        //    acc_coeff.y = (float)((1.0 / (cal_acc_coeff.y - cal_acc_origin.y)) * 4.0f);
        //    acc_coeff.z = (float)((1.0 / (cal_acc_coeff.z - cal_acc_origin.z)) * 4.0f);

        //    Vector3 acc_raw_component = { (float)row_accel.x ,(float)row_accel.y ,(float)row_accel.z };
        //    Vector3 acc_vector_component;
        //    acc_vector_component.x = acc_raw_component.x * acc_coeff.x;
        //    acc_vector_component.y = acc_raw_component.y * acc_coeff.y;
        //    acc_vector_component.z = acc_raw_component.z * acc_coeff.z;

        //    // 結果
        //    accel.x = acc_vector_component.x / 50;
        //    accel.y = acc_vector_component.z / 10000;
        //    accel.z = acc_vector_component.y / -500;
        //}


        // 前フレームとの差分
        diff_accel = prev_accel - accel;

        //// 最大値、最小値、現在地のリセット
        //if (Input::KeyTrigger(DIK_1))
        //{
        //    maxValue = Vector3().Zero;
        //    minValue = Vector3().Zero;
        //    position = Vector3().Zero;
        //}

        //// 最大値、最小値の記録
        //if (maxValue.x < diff_accel.x)
        //{
        //    maxValue.x = diff_accel.x;
        //}
        //if (minValue.x > diff_accel.x)
        //{
        //    minValue.x = diff_accel.x;
        //}

        //// 直近10回分の値を平均して加算する
        //Vector3 average;
        //
        //// 差分を記録
        //arr.push_back(diff_accel);
        //// 要素数が10個超えたら10個にする
        //if (arr.size() > 10)
        //{
        //    arr.erase(arr.begin());
        //}
        //// 平均を取る
        //for (auto itr = arr.begin(); itr != arr.end(); itr++)
        //{
        //    average += (*itr);
        //}
        //average /= arr.size();

        //// 変化が小さかったら無視
        //if (average.x < deadZone)
        //{
        //    average = Vector3().Zero;
        //}
        //if (fabs(diff_accel.x) < deadZone)
        //{
        //    diff_accel.x = 0;
        //}
        //if (fabs(diff_accel.y) < deadZone)
        //{
        //    diff_accel.y = 0;
        //}
        //if (fabs(diff_accel.z) < deadZone)
        //{
        //    diff_accel.z = 0;
        //}

        //velocity += diff_accel / 10.0f;
        velocity += accel;
        position.x += velocity.x;

#pragma endregion

#pragma region ジャイロ

        /////////////////////////////////////////////////////
        //// gyro
        /////////////////////////////////////////////////////

        // 値の取得
        row_gyro.x = (float)(buff[19] | buff[20] << 8);
        row_gyro.y = (float)(buff[21] | buff[22] << 8);
        row_gyro.z = (float)(buff[23] | buff[24] << 8);

        //Vector3 gyr_neutral = { 330, 65495, 65520 };  // 資料参考
        Vector3 gyr_neutral = { 7, 65495, 65520 };  // 実測値

        {// ジャイロスコープ（キャリブレーション済み） - 回転（度 / 秒 - dps）
            //// 補正用の計算（上半分は毎回計算する必要はない）
            //Vector3 cal_gyro_coeff = { 350,0,4081 };      // オフセット量
            //Vector3 cal_gyro_offset = { 24,-19,-27 };     // コントローラー水平時のセンサーの値
            //Vector3 gyro_cal_coeff;
            //gyro_cal_coeff.x = (936.0f / (cal_gyro_coeff.x - cal_gyro_offset.x));
            //gyro_cal_coeff.y = (936.0f / (cal_gyro_coeff.y - cal_gyro_offset.y));
            //gyro_cal_coeff.z = (936.0f / (cal_gyro_coeff.z - cal_gyro_offset.z));
            
            //Vector3 gyro_raw_component = { (float)row_gyro.x, (float)row_gyro.y, (float)row_gyro.z };
            //Vector3 gyro_vector_component;
            //gyro_vector_component.x = (gyro_raw_component.x - cal_gyro_offset.x) * gyro_cal_coeff.x;
            //gyro_vector_component.y = (gyro_raw_component.y - cal_gyro_offset.y) * gyro_cal_coeff.y;
            //gyro_vector_component.z = (gyro_raw_component.z - cal_gyro_offset.z) * gyro_cal_coeff.z;
            
            //// 結果
            //gyro.x = (gyro_vector_component.x / 500);
            //gyro.y = (gyro_vector_component.y / 5000);
            //gyro.z = (gyro_vector_component.z / -100);      
        }

        {// ジャイロスコープ-回転（回転/秒）360である意味はあまりないかも
            gyro.x = ((row_gyro.x - gyr_neutral.x) / 0xffff * 360.0f);
            gyro.y = ((row_gyro.y - gyr_neutral.y) / 0xffff * 360.0f);
            gyro.z = ((row_gyro.z - gyr_neutral.z) / 0xffff * 360.0f);

            // 180超えてたら-方向にプロットしなおす
            if (gyro.x > 180)
            {
                gyro.x -= 360;
            }
            if (gyro.y > 180)
            {
                gyro.y -= 360;
            }
            if (gyro.z > 180)
            {
                gyro.z -= 360;
            }
        }

    }

    // 結果の加算

    add_gyro = gyro * squeeze;

    //rotation.x += add_gyro.x;
    //rotation.y += -add_gyro.z;
    //rotation.z += add_gyro.y;

#pragma endregion

}

JimaXengine::TestScene::TestScene()
{
    size = 0;
}

JimaXengine::TestScene::~TestScene()
{
    // ファイル出力テスト
    std::ofstream outfile("test.txt");

    for (auto itr = sensorData.begin(); itr != sensorData.end(); itr++)
    {
        // 配列の内容をファイルに出力
        outfile << (*itr);
        outfile << "\n";
    }
    outfile << "出力終了";
    outfile.close();

	delete light;
	delete oManager;
    delete object;

    delete device;
    delete dev;
}

void JimaXengine::TestScene::Initialize()
{
	// カメラ
	Vector3 eye, target, up;
	eye = { 0, 50, -100 };
	target = { 0, 3, 10 };
	up = { 0, 1, 0 };
	camera = std::make_unique<DebugCamera>();
	camera->SetInitialViewMatrix(eye, target);
	camera->SetViewMatrix(eye, target, up);
    float viewAngle = 60.0f;	// 視野角
    camera->SetProjectionMatrix(WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT, viewAngle);

	// オブジェクトマネージャー
	oManager = new GameObjectManager();
	oManager->Initialize();
    //oManager->Add(new Player(camera.get()));

	//oManager->Add(new JoyconTest());

	// ライト
	light = LightGroup::Create();
	Object3d::SetLightGroup(light);

	// シーン遷移
	isEnd = false;
	nowScene = "TestScene";
	nextScene = "TestScene";

    object = new Object3d(position, scale, rotation, color);
    object->Initialize();
    object->SetModelforBuff("joycon");

    position = Vector3(0, 0, 0);
    scale = Vector3(10, 10, 10);
    rotation = Vector3(0, 0, 0);
    color = Vector4(1, 1, 1, 1);

    velocity = Vector3(0, 0, 0);

    JoyConInitialize();
}

void JimaXengine::TestScene::Update()
{
	camera->Move();

	light->Update();

	oManager->Update();

    object->SetCamera(camera.get());
    object->Update();

    JoyConUpdate();
}

void JimaXengine::TestScene::Draw()
{
    object->Draw();

	oManager->Draw();

    ImGui::SetNextWindowPos(ImVec2(20, 20), 1 << 1);
    ImGui::SetNextWindowSize(ImVec2(400, 300), 1 << 1);

    ImGui::Begin("joyconparam");
    ImGui::Text(" row_accel : [%+010.3f], [%+010.3f], [%+010.3f]", row_accel.x, row_accel.y, row_accel.z);
    //ImGui::Text("  row_gyro : [%+010.3f], [%+010.3f], [%+010.3f]", row_gyro.x, row_gyro.y, row_gyro.z);

    ImGui::Text("     accel : [%+010.3f], [%+010.3f], [%+010.3f]", accel.x, accel.y, accel.z);
    //ImGui::Text("     gyro  : [%+010.3f], [%+010.3f], [%+010.3f]", gyro.x, gyro.y, gyro.z);

    //ImGui::Text("diff_accel : [%+010.3f], [%+010.3f], [%+010.3f]", diff_accel.x, diff_accel.y, diff_accel.z);
    //ImGui::Text(" add_gyro  : [%+010.3f], [%+010.3f], [%+010.3f]", add_gyro.x, add_gyro.y, add_gyro.z);
   
    //ImGui::Text(" maxValue  : [%+010.3f], [%+010.3f], [%+010.3f]", maxValue.x, maxValue.y, maxValue.z);
    //ImGui::Text(" minValue  : [%+010.3f], [%+010.3f], [%+010.3f]", minValue.x, minValue.y, minValue.z);

    //ImGui::Text(" Length  : [%+010.3f]", diff_accel.Length());

    //static float slider1 = 0.0f;
    //ImGui::SliderFloat("deadZone", &deadZone, 0.0f, 20.0f);
    //ImGui::SliderFloat("speed", &speed, 0.0f, 1.0f);
    ImGui::SliderFloat("squeeze", &squeeze, 0.0f, 1.0f);

    ImGui::End();

}
