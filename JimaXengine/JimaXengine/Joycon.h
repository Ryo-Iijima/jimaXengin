#pragma once
#include "Joycon_support.h"
#include "hidapi/hidapi.h"
#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include <thread>
#include <queue>
#include <mutex>

class Joycon
{
private:
    // デバッグタイプ
    enum class DebugType
    {
        NONE,
        ALL,
        COMMS,
        THREADING,
        IMU,
        RUMBLE,
    };
    DebugType debug_type = DebugType::NONE;
    bool isLeft;    // JoyCon左右判別用フラグ

    // JoyCon状態
    enum class state_
    {
        NOT_ATTACHED,
        DROPPED,
        NO_JOYCONS,
        ATTACHED,
        INPUT_MODE_0x30,
        IMU_DATA_OK,
    };
    state_ state;   // JoyCon状態

    // ボタン
    enum class Button
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

    hid_device* handle;        // デバイスハンドル
    // サブコマンドのバッファ指定用
    uint8_t default_buf[8] = { 0x0, 0x1, 0x40, 0x40, 0x0, 0x1, 0x40, 0x40 };

    uint8_t stick_raw[3] = { 0, 0, 0 };             // スティックの生データ
    uint16_t stick_cal[6] = { 0, 0, 0, 0, 0, 0 };   // 現在の値
    uint16_t deadzone;                              // ゼロとする範囲
    uint16_t stick_precal[2] = { 0, 0 };            // スティックの位置?stic_calとの差は？

    bool stop_polling = false;                      // Detach()で使っているので通信をやめるフラグだと思う
    int timestamp;                                  // 
    bool first_imu_packet = true;                   // imuの入力が初めてのときフラグ（初期化用）
    bool imu_enabled = false;                       // imuが有効かのフラグ（Inertial Measurement Unitの略）

    uint16_t acc_r[3] = { 0, 0, 0 };                // センサーの値
    Vector3 acc_g;                                  // 加速度計算結果

    uint16_t gyr_r[3] = { 0, 0, 0 };                // センサーの値
    uint16_t gyr_neutral[3] = { 0, 0, 0 };          // センサーの値（静置した時の値？）
    Vector3 gyr_g;                                  // ジャイロ計算結果

    bool do_localize;
    float filterweight;                             // フィルター係数（なんのフィルターかはまだ不明、おそらくセンサーの値の調整に使用）
    static const unsigned int report_len = 49;      // レポートのデータ長？

    // 入力レポート構造体
    struct Report
    {
        uint8_t* r;               // レポート保持用
        time_t dataTime;            // データタイム

        // コンストラクタ
        Report(uint8_t* report, long time)
        {
            *r = *report;
            dataTime = time;
        }

        // getter
        time_t GetTime()
        {
            return dataTime;
        }

        // バッファのコピー
        void CopyBuffer(uint8_t b[])
        {
            for (int i = 0; i < report_len; ++i)
            {
                b[i] = r[i];
            }
        }
    };

    // 振動構造体（フォースフィードバッグの方がわかりやすくない？）
    struct Rumble
    {
    private:
        float h_f, amp, l_f;    // 高周波、振幅、低周波

    public:
        float t;                // 振動時間カウント用？
        bool timed_rumble;      // 実際に振動してる時にtrueになる？

    private:
        // クランプ
        float clamp(float x, float min, float max)
        {
            if (x < min) return min;
            if (x > max) return max;

            return x;
        }

    public:
        // 値の設定
        void set_vals(float low_freq, float high_freq, float amplitude, int time = 0)
        {
            h_f = high_freq;
            amp = amplitude;
            l_f = low_freq;
            timed_rumble = false;
            t = 0;
            if (time != 0)
            {
                t = time / 1000.0f;
                timed_rumble = true;
            }
        }

        // コンストラクタ
        Rumble() {}
        Rumble(float low_freq, float high_freq, float amplitude, int time = 0)
        {
            h_f = high_freq;
            amp = amplitude;
            l_f = low_freq;
            timed_rumble = false;
            t = 0;
            if (time != 0)
            {
                t = time / 1000.0f;
                timed_rumble = true;
            }
        }

        // getter
        uint8_t* GetData()
        {
            uint8_t rumble_data[8];
            if (amp == 0.0f) {
                rumble_data[0] = 0x0;
                rumble_data[1] = 0x1;
                rumble_data[2] = 0x40;
                rumble_data[3] = 0x40;
            }
            else {
                l_f = clamp(l_f, 40.875885f, 626.286133f);
                amp = clamp(amp, 0.0f, 1.0f);
                h_f = clamp(h_f, 81.75177f, 1252.572266f);
                uint16_t hf = (uint16_t)((round(32.0f * log(h_f * 0.1f)) - 0x60) * 4);
                uint8_t lf = (uint8_t)(round(32.0f * log(l_f * 0.1f)) - 0x40);
                uint8_t hf_amp;
                if (amp == 0) hf_amp = 0;
                else if (amp < 0.117) hf_amp = (uint8_t)(((log(amp * 1000) * 32) - 0x60) / (5 - pow(amp, 2)) - 1);
                else if (amp < 0.23) hf_amp = (uint8_t)(((log(amp * 1000) * 32) - 0x60) - 0x5c);
                else hf_amp = (uint8_t)((((log(amp * 1000) * 32) - 0x60) * 2) - 0xf6);

                uint16_t lf_amp = (uint16_t)(round(hf_amp) * .5);
                uint8_t parity = (uint8_t)(lf_amp % 2);
                if (parity > 0)
                {
                    --lf_amp;
                }

                lf_amp = (uint16_t)(lf_amp >> 1);
                lf_amp += 0x40;
                if (parity > 0) lf_amp |= 0x8000;
                //rumble_data = new byte[8];
                rumble_data[0] = (uint8_t)(hf & 0xff);
                rumble_data[1] = (uint8_t)((hf >> 8) & 0xff);
                rumble_data[2] = lf;
                rumble_data[1] += hf_amp;
                rumble_data[2] += (uint8_t)((lf_amp >> 8) & 0xff);
                rumble_data[3] += (uint8_t)(lf_amp & 0xff);
            }

            for (int i = 0; i < 4; ++i)
            {
                rumble_data[4 + i] = rumble_data[i];
            }

            return rumble_data;
        }
    };

    std::queue<Report*> reports;        // レポート配列
    Rumble* rumble_obj;                 // 振動データオブジェクト

    uint8_t global_count = 0;           // 
    std::string debug_str;              // 

    uint8_t ts_en;
    uint8_t ts_de;
    long ts_prev;

    std::mutex _lockReport;            // lock用オブジェクト
    std::mutex _lockButton,  _lockButtonDown,  _lockButtonUP;
    std::thread* PollThreadObj;

    Vector3 max = { 0, 0, 0 };
    Vector3 sum = { 0, 0, 0 };

    float err;
    Vector3 i_b, j_b, k_b, k_acc;
    Vector3 d_theta;
    Vector3 i_b_;
    Vector3 w_a, w_g;
    Vector4 vec;

public:
    // コンストラクタ
    Joycon(hid_device* handle_, bool imu, bool localize, float alpha, bool left);
    // デストラクタ
    ~Joycon();
    // デバッグプリント
    void DebugPrint(std::string s, DebugType d);

    // getter
    bool GetButtonDown(Button b) { return buttons_down[(int)b]; }
    bool GetButton(Button b) { return buttons[(int)b]; }
    bool GetButtonUp(Button b) { return buttons_up[(int)b]; }
    Vector2 GetStick() { return stick; }
    Vector3 GetGyro() { return gyr_g; }
    Vector3 GetAccel() { return acc_g; }
    Vector4 GetVector();

    int Attach(uint8_t leds_ = 0x0);        // 結びつけ、コントローラへの命令
    // setter
    void SetFilterCoeff(float a) { filterweight = a; }
    void Detach();                          // 後処理
    int ReceiveRaw();                       // 直訳->生、受け取る
    void Poll();                            // 直訳->投票、Beginで呼ばれてる

    void Update();

    int ProcessButtonsAndStick(uint8_t* report_buf);        // ボタンとスティックの状態を取る
    void ExtractIMUValues(uint8_t* report_buf, int n = 0);  // IMUの値を引き抜く
    int ProcessIMU(uint8_t* report_buf);                    // IMUの状態を取る
    void Begin();                                           // 前処理
    void Recenter();                                        // 
    Vector2 CenterSticks(uint16_t* vals);                   // 直訳->スティック中心
    void SetRumble(float low_freq, float high_freq, float amp, int time = 0);     // 振動
    void SendRumble(uint8_t* buf);                                                // Setしたものを送る？2つは抱き合わせ？

    uint8_t* Subcommand(uint8_t sc, uint8_t* buf, unsigned int len, bool print = true);  // サブコマンドを送る
    void dump_calibration_data();   // 直訳->ゴミキャリブレーションデータ

    uint8_t* ReadSPI(uint8_t addr1, uint8_t addr2, unsigned int len, bool print = false);

    /// <summary>
    /// n型の配列を受け取ってコンソール出力する
    /// </summary>
    /// <typeparam name="T">  </typeparam>
    /// <param name="arr"> 配列 </param>
    /// <param name="d"> デバッグタイプ </param>
    /// <param name="len"> 配列のデータ長 </param>
    /// <param name="start"> 開始位置 </param>
    /// <param name="format"> 出力書式 </param>
    //void PrintArray<T>(T[] arr, DebugType d = DebugType::NONE, uint len = 0, uint start = 0, string format = "{0:S}");     // 構文が分からん、テンプレート的なこと？
    template<class T> void PrintArray(T* arr, DebugType d = DebugType::NONE, uint8_t len = 0, uint8_t start = 0, std::string format = "{0:S}");     // これでいいのか？
};