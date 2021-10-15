#include "Joycon.h"
#include <mutex>
#include <Windows.h>

Joycon::Joycon(hid_device* handle_, bool imu, bool localize, float alpha, bool left)
{
	handle = handle_;
	imu_enabled = imu;
	do_localize = localize;
	rumble_obj = new Rumble(160, 320, 0);
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
		printf("%c\n", s);
	}
}

Vector4 Joycon::GetVector()
{
	Vector3 v1 = Vector3(j_b.x, i_b.x, k_b.x);
	Vector3 v2 = -(Vector3(j_b.z, i_b.z, k_b.z));
	if (v2 != Vector3::Zero)
	{
		//return Vector4.LookRotation(v1, v2);
	}
	else
	{
		//return Vector4.identity;
	}
	return Vector4(1, 1, 1, 1);
}

int Joycon::Attach(uint8_t leds_ = 0x0)
{
	state = state_::ATTACHED;
	uint8_t a[2] = { 0x0 };
	// Input report mode
	Subcommand(0x3, (uint8_t*)0x3f, 1, false);
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
	Subcommand(0x40, { (uint8_t*)(imu_enabled ? (uint8_t)0x1 : (uint8_t)0x0) }, 1, true);
	Subcommand(0x3, { (uint8_t*)0x30 }, 1, true);
	Subcommand(0x48, { (uint8_t*)0x1 }, 1, true);
	DebugPrint("Done with init.", DebugType::COMMS);
	return 0;
}

void Joycon::Detach()
{
	stop_polling = true;
	PrintArray(&max, DebugType::IMU, 0, 0, "float");
	PrintArray(&sum, DebugType::IMU, 0, 0, "float");
	if (state > state_::NO_JOYCONS)
	{
		Subcommand(0x30, { (uint8_t*)0x00 }, 1);
		Subcommand(0x40, { (uint8_t*)0x00 }, 1);
		Subcommand(0x48, { (uint8_t*)0x00 }, 1);
		Subcommand(0x03, { (uint8_t*)0x3f }, 1);
	}
	if (state > state_::DROPPED)
	{
		hid_close(handle);
	}
	state = state_::NOT_ATTACHED;
}

int Joycon::ReceiveRaw()
{
	if (handle == 0) return -2;
	hid_set_nonblocking(handle, 0);
	//uint8_t raw_buf[report_len];
	uint8_t raw_buf[report_len];
	int ret = hid_read(handle, raw_buf, sizeof(raw_buf));    // HID‚Ì’l‚ð“Ç‚ÝŽæ‚é
	if (ret > 0)
	{
		lock(reports)           // lock:ˆ—‚ªI‚í‚é‚Ü‚Å‘¼‚Ìlock‚µ‚½ˆ—‚ðs‚í‚È‚¢[?]
		{
			reports.Enqueue(new Report(raw_buf, System.DateTime.Now));
		}
		if (ts_en == raw_buf[1])
		{
			DebugPrint(("Duplicate timestamp enqueued. TS: %s\n", ts_en), DebugType::THREADING);
		}
		ts_en = raw_buf[1];
		DebugPrint(("Enqueue. Bytes read: %d. Timestamp: %s\n", ret, raw_buf[1]), DebugType::THREADING);
	}
	return ret;
}

void Joycon::Poll()
{
	int attempts = 0;
	while (!stop_polling & state > state_::NO_JOYCONS)
	{
		SendRumble(rumble_obj->GetData());
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
			Sleep(5);
		}
		++attempts;
	}
	DebugPrint("End poll loop.", DebugType::THREADING);

}

template<class T> void Joycon::PrintArray(T* arr, DebugType d = DebugType::NONE, uint8_t len = 0, uint8_t start = 0, std::string format = "{0:S}")
{
	if (d != debug_type && debug_type != DebugType::ALL) return;
	if (len == 0) len = (uint)arr.Length;
	string tostr = "";
	for (int i = 0; i < len; ++i)
	{
		tostr += string.Format((arr[0] is byte) ? "{0:X2} " : ((arr[0] is float) ? "{0:F} " : "{0:D} "), arr[i + start]);
	}
	DebugPrint(string.Format(format, tostr), d);
}