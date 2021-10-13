#include "Joycon.h"

Joycon::Joycon(hid_device handle_, bool imu, bool localize, float alpha, bool left)
{
	handle = handle_;
	imu_enabled = imu;
	do_localize = localize;
	rumble_obj = { 160, 320, 0 };
	filterweight = alpha;
	isLeft = left;
}

Joycon::~Joycon()
{

}

void Joycon::DebugPrint(std::string s, DebugType d)
{
	if (debug_type == DebugType::NONE) return;
	if (d == DebugType::ALL || d == debug_type || debug_type == DebugType::ALL)
	{
		//Debug.Log(s);
	}
}

int Joycon::Attach(uint8_t leds_ = 0x0)
{
    state = state_::ATTACHED;
    uint8_t a[] = { 0x0 };
    // Input report mode
    Subcommand(0x3, new uint8_t[]{ 0x3f }, 1, false);
    a[0] = 0x1;
    dump_calibration_data();
    // Connect
    a[0] = 0x01;
    Subcommand(0x1, a, 1);
    a[0] = 0x02;
    Subcommand(0x1, a, 1);
    a[0] = 0x03;
    Subcommand(0x1, a, 1);
    a[0] = leds_;
    Subcommand(0x30, a, 1);
    Subcommand(0x40, new uint8_t[]{ (imu_enabled ? (uint8_t)0x1 : (uint8_t)0x0) }, 1, true);
    Subcommand(0x3, new uint8_t[]{ 0x30 }, 1, true);
    Subcommand(0x48, new uint8_t[]{ 0x1 }, 1, true);
    DebugPrint("Done with init.", DebugType::COMMS);
  
    return 0;
}

void Joycon::SetFilterCoeff(float a)
{
    filterweight = a;
}

void Joycon::Detach()
{
    stop_polling = true;
    PrintArray(max, format: "Max {0:S}", d : DebugType.IMU);
    PrintArray(sum, format: "Sum {0:S}", d : DebugType.IMU);
    if (state > state_::NO_JOYCONS)
    {
        Subcommand(0x30, new uint8_t[]{ 0x0 }, 1);
        Subcommand(0x40, new uint8_t[]{ 0x0 }, 1);
        Subcommand(0x48, new uint8_t[]{ 0x0 }, 1);
        Subcommand(0x3, new uint8_t[]{ 0x3f }, 1);
    }
    if (state > state_::DROPPED)
    {
        hid_close(handle);
    }
    state = state_::NOT_ATTACHED;
}

int Joycon::ReceiveRaw()
{
    if (handle == nullptr) return -2;
    hid_set_nonblocking(handle, 0);
    byte[] raw_buf = new byte[report_len];
    int ret = hid_read(handle, raw_buf, new UIntPtr(report_len));    // HIDの値を読み取る
    if (ret > 0)
    {
        lock(reports)           // lock:処理が終わるまで他のlockした処理を行わない
        {
            reports.Enqueue(new Report(raw_buf, System.DateTime.Now));
        }
        if (ts_en == raw_buf[1])
        {
            DebugPrint(string.Format("Duplicate timestamp enqueued. TS: {0:X2}", ts_en), DebugType.THREADING);
        }
        ts_en = raw_buf[1];
        DebugPrint(string.Format("Enqueue. Bytes read: {0:D}. Timestamp: {1:X2}", ret, raw_buf[1]), DebugType.THREADING);
    }
    return ret;

}

void Joycon::Poll()
{
    int attempts = 0;
    while (!stop_polling & state > state_::NO_JOYCONS)
    {
        SendRumble(rumble_obj.GetData());
        int a = ReceiveRaw();
        a = ReceiveRaw();
        if (a > 0)
        {
            state = state_::IMU_DATA_OK;
            attempts = 0;
        }
        else if (attempts > 1000)
        {
            state = state_::DROPPED;
            DebugPrint("Connection lost. Is the Joy-Con connected?", DebugType::ALL);
            break;
        }
        else
        {
            DebugPrint("Pause 5ms", DebugType::THREADING);
            Thread.Sleep((Int32)5);
        }
        ++attempts;
    }
    DebugPrint("End poll loop.", DebugType::THREADING);
}

void Joycon::Update()
{
    if (state > state_::NO_JOYCONS)      // ジョイコンがあるか
    {
        uint8_t report_buf = new byte[report_len];
        while (reports.Count > 0)       // レポートの数が1以上
        {
            Report rep;
            lock(reports)
            {
                rep = reports.Dequeue();
                rep.CopyBuffer(report_buf);
            }
            if (imu_enabled)
            {
                if (do_localize)
                {
                    ProcessIMU(report_buf);
                }
                else
                {
                    ExtractIMUValues(report_buf, 0);
                }
            }
            if (ts_de == report_buf[1])
            {
                DebugPrint(string.Format("Duplicate timestamp dequeued. TS: {0:X2}", ts_de), DebugType.THREADING);
            }
            ts_de = report_buf[1];
            DebugPrint(string.Format("Dequeue. Queue length: {0:d}. Packet ID: {1:X2}. Timestamp: {2:X2}. Lag to dequeue: {3:s}. Lag between packets (expect 15ms): {4:s}",
                reports.Count, report_buf[0], report_buf[1], System.DateTime.Now.Subtract(rep.GetTime()), rep.GetTime().Subtract(ts_prev)), DebugType::THREADING);
            ts_prev = rep.GetTime();
        }
        ProcessButtonsAndStick(report_buf);
        if (rumble_obj.timed_rumble) {
            if (rumble_obj.t < 0) {
                rumble_obj.set_vals(160, 320, 0, 0);
            }
            else {
                rumble_obj.t -= Time.deltaTime;
            }
        }
    }
}

int Joycon::ProcessButtonsAndStick(uint8_t* report_buf)
{
    if (report_buf[0] == 0x00) return -1;

    stick_raw[0] = report_buf[6 + (isLeft ? 0 : 3)];
    stick_raw[1] = report_buf[7 + (isLeft ? 0 : 3)];
    stick_raw[2] = report_buf[8 + (isLeft ? 0 : 3)];

    stick_precal[0] = (uint16_t)(stick_raw[0] | ((stick_raw[1] & 0xf) << 8));
    stick_precal[1] = (uint16_t)((stick_raw[1] >> 4) | (stick_raw[2] << 4));
    stick = CenterSticks(stick_precal);
    lock(buttons)      // ボタンの受付
    {
        lock(down_)
        {
            for (int i = 0; i < buttons.Length; ++i)
            {
                down_[i] = buttons[i];
            }
        }
        buttons[(int)Button.DPAD_DOWN] = (report_buf[3 + (isLeft ? 2 : 0)] & (isLeft ? 0x01 : 0x04)) != 0;
        buttons[(int)Button.DPAD_RIGHT] = (report_buf[3 + (isLeft ? 2 : 0)] & (isLeft ? 0x04 : 0x08)) != 0;
        buttons[(int)Button.DPAD_UP] = (report_buf[3 + (isLeft ? 2 : 0)] & (isLeft ? 0x02 : 0x02)) != 0;
        buttons[(int)Button.DPAD_LEFT] = (report_buf[3 + (isLeft ? 2 : 0)] & (isLeft ? 0x08 : 0x01)) != 0;
        buttons[(int)Button.HOME] = ((report_buf[4] & 0x10) != 0);
        buttons[(int)Button.MINUS] = ((report_buf[4] & 0x01) != 0);
        buttons[(int)Button.PLUS] = ((report_buf[4] & 0x02) != 0);
        buttons[(int)Button.STICK] = ((report_buf[4] & (isLeft ? 0x08 : 0x04)) != 0);
        buttons[(int)Button.SHOULDER_1] = (report_buf[3 + (isLeft ? 2 : 0)] & 0x40) != 0;
        buttons[(int)Button.SHOULDER_2] = (report_buf[3 + (isLeft ? 2 : 0)] & 0x80) != 0;
        buttons[(int)Button.SR] = (report_buf[3 + (isLeft ? 2 : 0)] & 0x10) != 0;
        buttons[(int)Button.SL] = (report_buf[3 + (isLeft ? 2 : 0)] & 0x20) != 0;
        lock(buttons_up)
        {
            lock(buttons_down)
            {
                for (int i = 0; i < buttons.Length; ++i)
                {
                    buttons_up[i] = (down_[i] & !buttons[i]);
                    buttons_down[i] = (!down_[i] & buttons[i]);
                }
            }
        }
    }
    return 0;
}

void Joycon::ExtractIMUValues(uint8_t* report_buf, int n = 0)
{
    gyr_r[0] = (Int16)(report_buf[19 + n * 12] | ((report_buf[20 + n * 12] << 8) & 0xff00));    // n*12はLとRのバッファのズレ？12足すかそのままかのみ
    gyr_r[1] = (Int16)(report_buf[21 + n * 12] | ((report_buf[22 + n * 12] << 8) & 0xff00));
    gyr_r[2] = (Int16)(report_buf[23 + n * 12] | ((report_buf[24 + n * 12] << 8) & 0xff00));
    acc_r[0] = (Int16)(report_buf[13 + n * 12] | ((report_buf[14 + n * 12] << 8) & 0xff00));
    acc_r[1] = (Int16)(report_buf[15 + n * 12] | ((report_buf[16 + n * 12] << 8) & 0xff00));
    acc_r[2] = (Int16)(report_buf[17 + n * 12] | ((report_buf[18 + n * 12] << 8) & 0xff00));
    for (int i = 0; i < 3; ++i)
    {
        acc_g[i] = acc_r[i] * 0.00025f;                             // 加速度（G単位）の式？
        gyr_g[i] = (gyr_r[i] - gyr_neutral[i]) * 0.00122187695f;    // 回転（度/秒-dps）の式？
        if (Math.Abs(acc_g[i]) > Math.Abs(max[i]))                  // 取った値の絶対値が最大値の絶対値より大きかったら
            max[i] = acc_g[i];                                      // 最大値を設定しなおす
    }

}

int Joycon::ProcessIMU(uint8_t* report_buf)
{
    // Direction Cosine Matrix method
// http://www.starlino.com/dcm_tutorial.html

    if (!imu_enabled | state < state_::IMU_DATA_OK)
        return -1;

    if (report_buf[0] != 0x30) return -1; // no gyro data

    // read raw IMU values
    int dt = (report_buf[1] - timestamp);
    if (report_buf[1] < timestamp) dt += 0x100;

    for (int n = 0; n < 3; ++n)
    {
        ExtractIMUValues(report_buf, n);

        float dt_sec = 0.005f * dt;
        sum[0] += gyr_g.x * dt_sec;
        sum[1] += gyr_g.y * dt_sec;
        sum[2] += gyr_g.z * dt_sec;

        if (isLeft)
        {
            gyr_g.y *= -1;
            gyr_g.z *= -1;
            acc_g.y *= -1;
            acc_g.z *= -1;
        }

        if (first_imu_packet)
        {
            i_b = { 1, 0, 0 };
            j_b = { 0, 1, 0 };
            k_b = { 0, 0, 1 };
            first_imu_packet = false;
        }
        else
        {
            k_acc = -Vector3.Normalize(acc_g);
            w_a = Vector3.Cross(k_b, k_acc);
            w_g = -gyr_g * dt_sec;
            d_theta = (filterweight * w_a + w_g) / (1f + filterweight);
            k_b += Vector3.Cross(d_theta, k_b);
            i_b += Vector3.Cross(d_theta, i_b);
            j_b += Vector3.Cross(d_theta, j_b);
            //Correction, ensure new axes are orthogonal
            err = Vector3.Dot(i_b, j_b) * 0.5f;
            i_b_ = Vector3.Normalize(i_b - err * j_b);
            j_b = Vector3.Normalize(j_b - err * i_b);
            i_b = i_b_;
            k_b = Vector3.Cross(i_b, j_b);
        }
        dt = 1;
    }
    timestamp = report_buf[1] + 2;
    return 0;
}

void Joycon::Begin()
{
    if (PollThreadObj == null)
    {
        PollThreadObj = new Thread(new ThreadStart(Poll));
        PollThreadObj.Start();
    }
}

void Joycon::Recenter()
{
    first_imu_packet = true;
}

Vector2 Joycon::CenterSticks(uint16_t vals)
{
    Vector2 s = { 0, 0 };
    for (int i = 0; i < 2; ++i)
    {
        float diff = vals[i] - stick_cal[2 + i];
        if (abs(diff) < deadzone) vals[i] = 0;
        else if (diff > 0) // if axis is above center
        {
            s[i] = diff / stick_cal[i];
        }
        else
        {
            s[i] = diff / stick_cal[4 + i];
        }
    }
    return s;

}

void Joycon::SetRumble(float low_freq, float high_freq, float amp, int time = 0)
{
    if (state <= state_::ATTACHED) return;
    if (rumble_obj.timed_rumble == false || rumble_obj.t < 0)
    {
        rumble_obj = Rumble(low_freq, high_freq, amp, time);
    }
}

void Joycon::SendRumble(uint8_t* buf)
{
    byte[] buf_ = new byte[report_len];
    buf_[0] = 0x10;
    buf_[1] = global_count;
    if (global_count == 0xf) global_count = 0;
    else ++global_count;
    Array.Copy(buf, 0, buf_, 2, 8);
    PrintArray(buf_, DebugType.RUMBLE, format: "Rumble data sent: {0:S}");
    hid_write(handle, buf_, new UIntPtr(report_len));

}

uint8_t* Joycon::Subcommand(uint8_t sc, uint8_t* buf, unsigned int len, bool print = true)
{
    uint8_t buf_[] = new uint8_t[report_len];
    uint8_t response[] = new byte[report_len];
    Array.Copy(default_buf, 0, buf_, 2, 8);
    Array.Copy(buf, 0, buf_, 11, len);
    buf_[10] = sc;
    buf_[1] = global_count;
    buf_[0] = 0x1;
    if (global_count == 0xf) global_count = 0;
    else ++global_count;
    if (print) { PrintArray(buf_, DebugType.COMMS, len, 11, "Subcommand 0x" + string.Format("{0:X2}", sc) + " sent. Data: 0x{0:S}"); };
    hid_write(handle, buf_, new UIntPtr(len + 11));
    int res = hid_read_timeout(handle, response, new UIntPtr(report_len), 50);
    if (res < 1) DebugPrint("No response.", DebugType::COMMS);
    else if (print) { PrintArray(response, DebugType::COMMS, report_len - 1, 1, "Response ID 0x" + string.Format("{0:X2}", response[0]) + ". Data: 0x{0:S}"); }
    return response;

}

void Joycon::dump_calibration_data()
{
    byte[] buf_ = ReadSPI(0x80, (isLeft ? (byte)0x12 : (byte)0x1d), 9); // get user calibration data if possible
    bool found = false;
    for (int i = 0; i < 9; ++i)
    {
        if (buf_[i] != 0xff)
        {
            Debug.Log("Using user stick calibration data.");
            found = true;
            break;
        }
    }
    if (!found)
    {
        Debug.Log("Using factory stick calibration data.");
        buf_ = ReadSPI(0x60, (isLeft ? (uint8_t)0x3d : (uint8_t)0x46), 9); // get user calibration data if possible
    }
    stick_cal[isLeft ? 0 : 2] = (uint16_t)((buf_[1] << 8) & 0xF00 | buf_[0]); // X Axis Max above center
    stick_cal[isLeft ? 1 : 3] = (uint16_t)((buf_[2] << 4) | (buf_[1] >> 4));  // Y Axis Max above center
    stick_cal[isLeft ? 2 : 4] = (uint16_t)((buf_[4] << 8) & 0xF00 | buf_[3]); // X Axis Center
    stick_cal[isLeft ? 3 : 5] = (uint16_t)((buf_[5] << 4) | (buf_[4] >> 4));  // Y Axis Center
    stick_cal[isLeft ? 4 : 0] = (uint16_t)((buf_[7] << 8) & 0xF00 | buf_[6]); // X Axis Min below center
    stick_cal[isLeft ? 5 : 1] = (uint16_t)((buf_[8] << 4) | (buf_[7] >> 4));  // Y Axis Min below center

    PrintArray(stick_cal, len: 6, start : 0, format : "Stick calibration data: {0:S}");

    buf_ = ReadSPI(0x60, (isLeft ? (uint8_t)0x86 : (uint8_t)0x98), 16);
    deadzone = (uint16_t)((buf_[4] << 8) & 0xF00 | buf_[3]);

    buf_ = ReadSPI(0x80, 0x34, 10);
    gyr_neutral[0] = (uint16_t)(buf_[0] | ((buf_[1] << 8) & 0xff00));
    gyr_neutral[1] = (uint16_t)(buf_[2] | ((buf_[3] << 8) & 0xff00));
    gyr_neutral[2] = (uint16_t)(buf_[4] | ((buf_[5] << 8) & 0xff00));
    PrintArray(gyr_neutral, len: 3, d : DebugType.IMU, format : "User gyro neutral position: {0:S}");

    // This is an extremely messy way of checking to see whether there is user stick calibration data present, but I've seen conflicting user calibration data on blank Joy-Cons. Worth another look eventually.
    if (gyr_neutral[0] + gyr_neutral[1] + gyr_neutral[2] == -3 || abs(gyr_neutral[0]) > 100 || abs(gyr_neutral[1]) > 100 || abs(gyr_neutral[2]) > 100)
    {
        buf_ = ReadSPI(0x60, 0x29, 10);
        gyr_neutral[0] = (uint16_t)(buf_[3] | ((buf_[4] << 8) & 0xff00));
        gyr_neutral[1] = (uint16_t)(buf_[5] | ((buf_[6] << 8) & 0xff00));
        gyr_neutral[2] = (uint16_t)(buf_[7] | ((buf_[8] << 8) & 0xff00));
        PrintArray(gyr_neutral, len: 3, d : DebugType.IMU, format : "Factory gyro neutral position: {0:S}");
    }

}

uint8_t* Joycon::ReadSPI(uint8_t addr1, uint8_t addr2, unsigned int len, bool print)
{
    byte[] buf = { addr2, addr1, 0x00, 0x00, (byte)len };
    byte[] read_buf = new byte[len];
    byte[] buf_ = new byte[len + 20];

    for (int i = 0; i < 100; ++i)
    {
        buf_ = Subcommand(0x10, buf, 5, false);
        if (buf_[15] == addr2 && buf_[16] == addr1)
        {
            break;
        }
    }
    Array.Copy(buf_, 20, read_buf, 0, len);
    if (print) PrintArray(read_buf, DebugType::COMMS, len);
    return read_buf;

}

//void PrintArray<T>(T[] arr, DebugType d = DebugType.NONE, uint len = 0, uint start = 0, string format = "{0:S}")
//{
//    if (d != debug_type && debug_type != DebugType.ALL) return;
//    if (len == 0) len = (uint)arr.Length;
//    string tostr = "";
//    for (int i = 0; i < len; ++i)
//    {
//        tostr += string.Format((arr[0] is byte) ? "{0:X2} " : ((arr[0] is float) ? "{0:F} " : "{0:D} "), arr[i + start]);
//    }
//    DebugPrint(string.Format(format, tostr), d);
//}

template<class T> void Joycon::PrintArray(T* arr, DebugType d, uint8_t len, uint8_t star, std::string format)
{

}
