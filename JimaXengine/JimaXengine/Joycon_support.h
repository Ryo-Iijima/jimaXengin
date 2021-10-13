#pragma once
#include <cstddef>

struct Report
{
    uint8_t r[1];
    long dataTime;
    const int report_len = 49;

    Report(uint8_t report[], long time)
    {
        *r = *report;
        dataTime = time;
    }
    long GetTime()
    {
        return dataTime;
    }
    void CopyBuffer(uint8_t b[])
    {
        for (int i = 0; i < report_len; ++i)
        {
            b[i] = r[i];
        }
    }
};

struct Rumble
{
    float h_f, amp, l_f;
    float t;
    bool timed_rumble;

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

    Rumble();
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

    float clamp(float x, float min, float max)
    {
        if (x < min) return min;
        if (x > max) return max;

        return x;
    }

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
