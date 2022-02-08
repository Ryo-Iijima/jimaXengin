#include "TestScene.h"
#include "DebugCamera.h"
#include <fstream>

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

void JimaXengine::TestScene::JoyConUpdate()
{
    if (dev)
    {
        // �O�t���[���̏�Ԃ�ێ�
        prev_accel = accel;
        prev_gyro = gyro;

        {// �o�͗p�z��ɂ��L�^
            std::string acc = std::to_string(accel.x) + "," + std::to_string(accel.y) + "," + std::to_string(accel.z);
            std::string gyr = std::to_string(gyro.x) + "," + std::to_string(gyro.y) + "," + std::to_string(gyro.z);

            sensorData.push_back(acc + "," + gyr);
        }

        // input report ���󂯂Ƃ�B
        int ret = hid_read(dev, buff, size);

        // �{�^��
        //printf("button byte 1: %d\n", buff[1]);

#pragma region �����x

        /////////////////////////////////////////////////////
        //// accel 
        /////////////////////////////////////////////////////

        // �l�̎擾
        row_accel.x = (float)(buff[13] | buff[14] << 8);
        row_accel.y = (float)(buff[15] | buff[16] << 8);
        row_accel.z = (float)(buff[17] | buff[18] << 8);

        {// �����x�iG�j
            accel.x = row_accel.x * 0.00025f;
            accel.y = row_accel.y * 0.00025f;
            accel.z = row_accel.z * 0.00025f;
        }

        //{
        //    // �␳�p�̌v�Z�i�㔼���͖���v�Z����K�v�͂Ȃ��j
        //    Vector3 cal_acc_coeff = { 350,0,4081 };       // �I�t�Z�b�g��
        //    Vector3 cal_acc_origin = { 18,75,4100 };      // �R���g���[���[�������̃Z���T�[�̒l
        //    Vector3 acc_coeff;
        //    acc_coeff.x = (float)((1.0 / (cal_acc_coeff.x - cal_acc_origin.x)) * 4.0f);
        //    acc_coeff.y = (float)((1.0 / (cal_acc_coeff.y - cal_acc_origin.y)) * 4.0f);
        //    acc_coeff.z = (float)((1.0 / (cal_acc_coeff.z - cal_acc_origin.z)) * 4.0f);

        //    Vector3 acc_raw_component = { (float)row_accel.x ,(float)row_accel.y ,(float)row_accel.z };
        //    Vector3 acc_vector_component;
        //    acc_vector_component.x = acc_raw_component.x * acc_coeff.x;
        //    acc_vector_component.y = acc_raw_component.y * acc_coeff.y;
        //    acc_vector_component.z = acc_raw_component.z * acc_coeff.z;

        //    // ����
        //    accel.x = acc_vector_component.x / 50;
        //    accel.y = acc_vector_component.z / 10000;
        //    accel.z = acc_vector_component.y / -500;
        //}


        // �O�t���[���Ƃ̍���
        diff_accel = prev_accel - accel;

        //// �ő�l�A�ŏ��l�A���ݒn�̃��Z�b�g
        //if (Input::KeyTrigger(DIK_1))
        //{
        //    maxValue = Vector3().Zero;
        //    minValue = Vector3().Zero;
        //    position = Vector3().Zero;
        //}

        //// �ő�l�A�ŏ��l�̋L�^
        //if (maxValue.x < diff_accel.x)
        //{
        //    maxValue.x = diff_accel.x;
        //}
        //if (minValue.x > diff_accel.x)
        //{
        //    minValue.x = diff_accel.x;
        //}

        //// ����10�񕪂̒l�𕽋ς��ĉ��Z����
        //Vector3 average;
        //
        //// �������L�^
        //arr.push_back(diff_accel);
        //// �v�f����10��������10�ɂ���
        //if (arr.size() > 10)
        //{
        //    arr.erase(arr.begin());
        //}
        //// ���ς����
        //for (auto itr = arr.begin(); itr != arr.end(); itr++)
        //{
        //    average += (*itr);
        //}
        //average /= arr.size();

        //// �ω��������������疳��
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

#pragma region �W���C��

        /////////////////////////////////////////////////////
        //// gyro
        /////////////////////////////////////////////////////

        // �l�̎擾
        row_gyro.x = (float)(buff[19] | buff[20] << 8);
        row_gyro.y = (float)(buff[21] | buff[22] << 8);
        row_gyro.z = (float)(buff[23] | buff[24] << 8);

        //Vector3 gyr_neutral = { 330, 65495, 65520 };  // �����Q�l
        Vector3 gyr_neutral = { 7, 65495, 65520 };  // �����l

        {// �W���C���X�R�[�v�i�L�����u���[�V�����ς݁j - ��]�i�x / �b - dps�j
            //// �␳�p�̌v�Z�i�㔼���͖���v�Z����K�v�͂Ȃ��j
            //Vector3 cal_gyro_coeff = { 350,0,4081 };      // �I�t�Z�b�g��
            //Vector3 cal_gyro_offset = { 24,-19,-27 };     // �R���g���[���[�������̃Z���T�[�̒l
            //Vector3 gyro_cal_coeff;
            //gyro_cal_coeff.x = (936.0f / (cal_gyro_coeff.x - cal_gyro_offset.x));
            //gyro_cal_coeff.y = (936.0f / (cal_gyro_coeff.y - cal_gyro_offset.y));
            //gyro_cal_coeff.z = (936.0f / (cal_gyro_coeff.z - cal_gyro_offset.z));
            
            //Vector3 gyro_raw_component = { (float)row_gyro.x, (float)row_gyro.y, (float)row_gyro.z };
            //Vector3 gyro_vector_component;
            //gyro_vector_component.x = (gyro_raw_component.x - cal_gyro_offset.x) * gyro_cal_coeff.x;
            //gyro_vector_component.y = (gyro_raw_component.y - cal_gyro_offset.y) * gyro_cal_coeff.y;
            //gyro_vector_component.z = (gyro_raw_component.z - cal_gyro_offset.z) * gyro_cal_coeff.z;
            
            //// ����
            //gyro.x = (gyro_vector_component.x / 500);
            //gyro.y = (gyro_vector_component.y / 5000);
            //gyro.z = (gyro_vector_component.z / -100);      
        }

        {// �W���C���X�R�[�v-��]�i��]/�b�j360�ł���Ӗ��͂��܂�Ȃ�����
            gyro.x = ((row_gyro.x - gyr_neutral.x) / 0xffff * 360.0f);
            gyro.y = ((row_gyro.y - gyr_neutral.y) / 0xffff * 360.0f);
            gyro.z = ((row_gyro.z - gyr_neutral.z) / 0xffff * 360.0f);

            // 180�����Ă���-�����Ƀv���b�g���Ȃ���
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

    // ���ʂ̉��Z

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
    // �t�@�C���o�̓e�X�g
    std::ofstream outfile("test.txt");

    for (auto itr = sensorData.begin(); itr != sensorData.end(); itr++)
    {
        // �z��̓��e���t�@�C���ɏo��
        outfile << (*itr);
        outfile << "\n";
    }
    outfile << "�o�͏I��";
    outfile.close();

	delete light;
	delete oManager;
    delete object;

    delete device;
    delete dev;
}

void JimaXengine::TestScene::Initialize()
{
	// �J����
	Vector3 eye, target, up;
	eye = { 0, 50, -100 };
	target = { 0, 3, 10 };
	up = { 0, 1, 0 };
	camera = std::make_unique<DebugCamera>();
	camera->SetInitialViewMatrix(eye, target);
	camera->SetViewMatrix(eye, target, up);
    float viewAngle = 60.0f;	// ����p
    camera->SetProjectionMatrix(WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT, viewAngle);

	// �I�u�W�F�N�g�}�l�[�W���[
	oManager = new GameObjectManager();
	oManager->Initialize();
    //oManager->Add(new Player(camera.get()));

	//oManager->Add(new JoyconTest());

	// ���C�g
	light = LightGroup::Create();
	Object3d::SetLightGroup(light);

	// �V�[���J��
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
