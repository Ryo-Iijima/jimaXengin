#pragma once
#include "Joycon_support.h"
#include "hidapi/hidapi.h"
#include <cstddef>
#include <cstdint>
#include "math/Vector3.h"
#include <vector>
#include <string>
#include "math/Vector2.h"
#include "math/Vector4.h"

class Joycon
{
private:
    enum DebugType
    {
        NONE,
        ALL,
        COMMS,
        THREADING,
        IMU,
        RUMBLE,
    };
    DebugType debug_type = DebugType::NONE;
    bool isLeft;

    // Joycon���
    enum state_
    {
        NOT_ATTACHED,
        DROPPED,
        NO_JOYCONS,
        ATTACHED,
        INPUT_MODE_0x30,
        IMU_DATA_OK,
    };
    state_ state;

    // �{�^��
    enum Button
    {
        DPAD_DOWN,
        DPAD_RIGHT,
        DPAD_LEFT,
        DPAD_UP,
        SL,
        SR,
        MINUS,
        HOME,
        PLUS,
        CAPTURE,
        STICK,
        SHOULDER_1,
        SHOULDER_2
    };
    bool buttons_down[13];
    bool buttons_up[13];
    bool buttons[13];
    bool down_[13];

    Vector2 stick = { 0, 0 };

    hid_device* handle;

    uint8_t default_buf[8] = { 0x0, 0x1, 0x40, 0x40, 0x0, 0x1, 0x40, 0x40 };

    uint8_t stick_raw[3] = { 0, 0, 0 };             // �X�e�B�b�N���f�[�^
    uint16_t stick_cal[6] = { 0, 0, 0, 0, 0, 0 };   // 
    uint16_t deadzone;                              // 
    uint16_t stick_precal[2] = { 0, 0 };            // 

    bool stop_polling = false;
    int timestamp;
    bool first_imu_packet = true;
    bool imu_enabled = false;
    uint16_t acc_r[3] = { 0, 0, 0 };    // �Z���T�[�̒l
    Vector3 acc_g;                      // �����x�v�Z����

    uint16_t gyr_r[3] = { 0, 0, 0 };        // �Z���T�[�̐��̒l
    uint16_t gyr_neutral[3] = { 0, 0, 0 };  // �Z���T�[�̐��̒l�i�Òu�������̒l�H�j
    Vector3 gyr_g;                          // �W���C���v�Z����

    bool do_localize;
    float filterweight;
    const unsigned int report_len = 49;              // input���|�[�g�̃T�C�Y

    std::vector<Report> reports;
    Rumble rumble_obj;

    uint8_t global_count = 0;
    std::string debug_str;

    uint8_t ts_en;
    uint8_t ts_de;
    long ts_prev;

    Thread PollThreadObj;

    Vector3 max = { 0, 0, 0 };
    Vector3 sum = { 0, 0, 0 };

    float err;
    Vector3 i_b, j_b, k_b, k_acc;
    Vector3 d_theta;
    Vector3 i_b_;
    Vector3 w_a, w_g;
    Vector4 vec;

public:
    Joycon(hid_device handle_, bool imu, bool localize, float alpha, bool left);
    ~Joycon();

    void DebugPrint(std::string s, DebugType d);

    // getter
    bool GetButtonDown(Button b) { return buttons_down[(int)b]; }
    bool GetButton(Button b) { return buttons[(int)b]; }
    bool GetButtonUp(Button b) { return buttons_up[(int)b]; }
    Vector2 GetStick() { return stick; }
    Vector3 GetGyro() { return gyr_g; }
    Vector3 GetAccel() { return acc_g; }
    Vector4 GetVector();

    int Attach(uint8_t leds_ = 0x0);        // ����->�Y�t�E�R���g���[���[���̐ݒ�
    void SetFilterCoeff(float a);
    void Detach();                          // �㏈��
    int ReceiveRaw();                       // ����->���A�󂯎��
    void Poll();                            // ����->���[�ABegin�ŌĂ΂�Ă�

    void Update();

    int ProcessButtonsAndStick(uint8_t* report_buf);        // �{�^���ƃX�e�B�b�N�̏�Ԃ����
    void ExtractIMUValues(uint8_t* report_buf, int n = 0);  // IMU�̒l����������
    int ProcessIMU(uint8_t* report_buf);                    // IMU�̏�Ԃ����
    void Begin();
    void Recenter();
    Vector2 CenterSticks(uint16_t vals);                    // ����->�X�e�B�b�N���S
    void SetRumble(float low_freq, float high_freq, float amp, int time = 0);     // �U��
    void SendRumble(uint8_t* buf);                                                // Set�������̂𑗂�H2�͕������킹�H

    uint8_t* Subcommand(uint8_t sc, uint8_t* buf, unsigned int len, bool print = true);  // �T�u�R�}���h�𑗂�
    void dump_calibration_data();   // ����->�S�~�L�����u���[�V�����f�[�^

    uint8_t* ReadSPI(uint8_t addr1, uint8_t addr2, unsigned int len, bool print = false);

    //void PrintArray<T>(T[] arr, DebugType d = DebugType::NONE, uint len = 0, uint start = 0, string format = "{0:S}");     // �\�����������A�e���v���[�g�I�Ȃ��ƁH
    template<class T> void PrintArray(T* arr, DebugType d = DebugType::NONE, uint8_t len = 0, uint8_t start = 0, std::string format = "{0:S}");     // ����ł����̂��H
};