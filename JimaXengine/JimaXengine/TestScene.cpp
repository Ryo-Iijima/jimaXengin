#include "TestScene.h"
#include "DebugCamera.h"

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
    // input report ���󂯂Ƃ�B
    if (dev)
    {
        int ret = hid_read(dev, buff, size);

        // �{�^��
        //printf("button byte 1: %d\n", buff[1]);

        // accel
        row_accel.x = buff[13] | buff[14] << 8;
        row_accel.y = buff[15] | buff[16] << 8;
        row_accel.z = buff[17] | buff[18] << 8;

        //printf("%d\n", accel.z);

        Vector3 cal_acc_coeff = { 350,0,4081 };       // �I�t�Z�b�g��
        Vector3 cal_acc_origin = { 18,75,4100 };      // �R���g���[���[�������̃Z���T�[�̒l
        Vector3 acc_coeff;
        acc_coeff.x = (float)((1.0 / (cal_acc_coeff.x - cal_acc_origin.x)) * 4.0f);
        acc_coeff.y = (float)((1.0 / (cal_acc_coeff.y - cal_acc_origin.y)) * 4.0f);
        acc_coeff.z = (float)((1.0 / (cal_acc_coeff.z - cal_acc_origin.z)) * 4.0f);

        Vector3 acc_raw_component = { (float)row_accel.x ,(float)row_accel.y ,(float)row_accel.z };
        Vector3 acc_vector_component;
        acc_vector_component.x = acc_raw_component.x * acc_coeff.x;
        acc_vector_component.y = acc_raw_component.y * acc_coeff.y;
        acc_vector_component.z = acc_raw_component.z * acc_coeff.z;

        accel.x = acc_vector_component.x / 50;
        accel.y = acc_vector_component.z / 10000;
        accel.z = acc_vector_component.y / -500;


        // gyro
        row_gyro.x = buff[19] | buff[20] << 8;
        row_gyro.y = buff[21] | buff[22] << 8;
        row_gyro.z = buff[23] | buff[24] << 8;

        Vector3 cal_gyro_coeff = { 350,0,4081 };      // �I�t�Z�b�g��
        Vector3 cal_gyro_offset = { 24,-19,-27 };     // �R���g���[���[�������̃Z���T�[�̒l
        Vector3 gyro_cal_coeff;
        gyro_cal_coeff.x = (936.0f / (cal_gyro_coeff.x - cal_gyro_offset.x));
        gyro_cal_coeff.y = (936.0f / (cal_gyro_coeff.y - cal_gyro_offset.y));
        gyro_cal_coeff.z = (936.0f / (cal_gyro_coeff.z - cal_gyro_offset.z));

        Vector3 gyro_raw_component = { (float)row_gyro.x, (float)row_gyro.y, (float)row_gyro.z };
        Vector3 gyro_vector_component;
        gyro_vector_component.x = (gyro_raw_component.x - cal_gyro_offset.x) * gyro_cal_coeff.x;
        gyro_vector_component.y = (gyro_raw_component.y - cal_gyro_offset.y) * gyro_cal_coeff.y;
        gyro_vector_component.z = (gyro_raw_component.z - cal_gyro_offset.z) * gyro_cal_coeff.z;

        rotation.x += (gyro_vector_component.x / 500);
        rotation.y += (gyro_vector_component.y / 5000);
        rotation.z += (gyro_vector_component.z / -100);

        //Sleep(500);
    }

    //velocity += accel;
    //position += velocity;
    //object->SetRotation(Vector3(rotation.x, rotation.z, rotation.y));
    //object->SetPosition(pos);

}

JimaXengine::TestScene::TestScene()
{
}

JimaXengine::TestScene::~TestScene()
{
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
	eye = { 0, 3, -23 };
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
	light = Light::Create();
	light->SetColor({ 1,1,1 });
	Object3d::SetLight(light);

	// �V�[���J��
	isEnd = false;
	nowScene = "TestScene";
	nextScene = "TestScene";

    object = new Object3d(position, scale, rotation, color);
    object->Initialize();
    object->SetModelforBuff("DefaultBox");

    position = Vector3(0, 0, 0);
    scale = Vector3(1, 1, 1);
    rotation = Vector3(0, 0, 0);
    color = Vector4(1, 1, 1, 1);

    JoyConInitialize();
}

void JimaXengine::TestScene::Update()
{
	camera->Move();

	{
		static XMVECTOR lightDir = { 0,1,5,0 };
		if (Input::KeyPress(DIK_UP)) { lightDir.m128_f32[1] += 1.0f; }
		if (Input::KeyPress(DIK_DOWN)) { lightDir.m128_f32[1] -= 1.0f; }
		if (Input::KeyPress(DIK_RIGHT)) { lightDir.m128_f32[0] += 1.0f; }
		if (Input::KeyPress(DIK_LEFT)) { lightDir.m128_f32[0] -= 1.0f; }

		light->SetDir(lightDir);
	}
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
    ImGui::Text("row_acc : %f,%f,%f", row_accel.x, row_accel.y, row_accel.z);
    ImGui::Text("row_gyro : %f,%f,%f", row_gyro.x, row_gyro.y, row_gyro.z);

    ImGui::Text("accel : %f,%f,%f", accel.x, accel.y, accel.z);
    ImGui::Text("rotation : %f,%f,%f", rotation.x, rotation.y, rotation.z);

    ImGui::End();

}
