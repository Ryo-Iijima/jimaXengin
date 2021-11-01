#include "Joycon.h"
#include <iterator>
#include <array>
#include "../general/_StringFormat.h"

void Joycon::Poll(Joycon* j)
{
	int attempts = 0;
	while (!j->stop_polling & j->state > state_::NO_JOYCONS)
	{
		j->SendRumble(j->rumble_obj->GetData());
		int a = j->ReceiveRaw();
		a = j->ReceiveRaw();
		if (a > 0)
		{
			j->state = state_::IMU_DATA_OK;
			attempts = 0;
		}
		else if (attempts > 1000)
		{
			j->state = state_::DROPPED;
			//DebugPrint("Connection lost. Is the Joy-Con connected?", DebugType::ALL);
			break;
		}
		else
		{
			//DebugPrint("Pause 5ms", DebugType::THREADING);
			//_sleep(5);
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
		++attempts;
	}
	//DebugPrint("End poll loop.", DebugType::THREADING);
}

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
		printf("%s\n", s.c_str());
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

int Joycon::Attach(uint8_t leds_)
{
	state = state_::ATTACHED;
	uint8_t a[] = { 0x0 };
	// Input report mode
	uint8_t arr[] = { 0x3f };
	Subcommand(0x3, arr, 1, false);
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

	arr[0] = { (imu_enabled ? (uint8_t)0x1 : (uint8_t)0x0) };
	Subcommand(0x40, arr, 1, true);
	//Subcommand(0x40, { (uint8_t*)(imu_enabled ? (uint8_t)0x1 : (uint8_t)0x0) }, 1, true);

	arr[0] = { 0x30 };
	Subcommand(0x3, arr, 1, true);
	//Subcommand(0x3, { (uint8_t*)0x30 }, 1, true);

	arr[0] = { 0x1 };
	Subcommand(0x48, arr, 1, true);
	//Subcommand(0x48, { (uint8_t*)0x1 }, 1, true);
	
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
	uint8_t raw_buf[report_len];
	int ret = hid_read(handle, raw_buf, sizeof(raw_buf));    // HIDの値を読み取る
	if (ret > 0)
	{
		{
			std::mutex _locktest;	// ロック用オブジェクト

			{
				// スコープの外に出ると、自動でlockが解放されるので、c#のコードと同じような書きき方で使えるらしい
				std::lock_guard<std::mutex> lock(_locktest);
				// ここから下に処理を書いていく
			}
			// スコープの外に出るとlock解放	
		}

		// c++ -> https://cpprefjp.github.io/reference/mutex/lock.html
		// c#  -> https://docs.microsoft.com/ja-jp/dotnet/csharp/language-reference/keywords/lock-statement
		// stackoverflow ->  https://stackoverflow.com/questions/56007404/cs-lock-in-c
		
		{
			//std::mutex a;
			std::lock_guard<std::mutex> lock(_lockReport);
			reports.push(new Report(raw_buf, std::time_t()));
		}
		if (ts_en == raw_buf[1])
		{
			//DebugPrint({ "Duplicate timestamp enqueued. TS: %d\n", ts_en }, DebugType::THREADING);
		}
		ts_en = raw_buf[1];
		//DebugPrint({"Enqueue. Bytes read: %d. Timestamp: %s\n", ret, (int)raw_buf[1]}, DebugType::THREADING);
	}
	return ret;
}

//void Joycon::Poll()
//{
//	int attempts = 0;
//	while (!stop_polling & state > state_::NO_JOYCONS)
//	{
//		SendRumble(rumble_obj->GetData());
//		int a = ReceiveRaw();
//		a = ReceiveRaw();
//		if (a > 0)
//		{
//			state = state_::IMU_DATA_OK;
//			attempts = 0;
//		}
//		else if (attempts > 1000)
//		{
//			state = state_::DROPPED;
//			DebugPrint("Connection lost. Is the Joy-Con connected?", DebugType::ALL);
//			break;
//		}
//		else
//		{
//			DebugPrint("Pause 5ms", DebugType::THREADING);
//			//_sleep(5);
//			std::this_thread::sleep_for(std::chrono::milliseconds(5));
//		}
//		++attempts;
//	}
//	DebugPrint("End poll loop.", DebugType::THREADING);
//}

void Joycon::Update()
{
	if (state > state_::NO_JOYCONS)      // ジョイコンがあるか
	{
		uint8_t report_buf[report_len];
		while (reports.size() > 0)       // レポートの数が1以上
		{
			Report* rep;

			{
				std::lock_guard<std::mutex> lock(_lockReport);

				rep = reports.front();
				rep->CopyBuffer(report_buf);

				delete rep;
				reports.pop();
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
				DebugPrint({ "Duplicate timestamp dequeued. TS: %d\n", ts_de }, DebugType::THREADING);
			}
			ts_de = report_buf[1];

			DebugPrint({"Dequeue. Queue length: %d./n", reports.size()}, DebugType::THREADING);
			DebugPrint({"Packet ID: %d./n", report_buf[0] }, DebugType::THREADING);
			DebugPrint({"Timestamp: %s.\n", report_buf[1] }, DebugType::THREADING);
			//DebugPrint({"Lag to dequeue: %d.\n", (std::time_t() - rep->GetTime()) }, DebugType::THREADING);
			//DebugPrint({"Lag between packets (expect 15ms): %d.\n", (rep->GetTime() - ts_prev) }, DebugType::THREADING);
			ts_prev = rep->GetTime();
		}
		ProcessButtonsAndStick(report_buf);
		if (rumble_obj->timed_rumble) {
			if (rumble_obj->t < 0) {
				rumble_obj->set_vals(160, 320, 0, 0);
			}
			else {
				rumble_obj->t -= std::time_t();
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
	{
		std::lock_guard<std::mutex> lock(_lockButton);
		{
			std::lock_guard<std::mutex> lock(_lockButtonDown);

			for (int i = 0; i < (sizeof(buttons) / sizeof(buttons[0])); ++i)
			{
				down_[i] = buttons[i];
			}
		}
		buttons[(int)Button::DPAD_DOWN] = (report_buf[3 + (isLeft ? 2 : 0)] & (isLeft ? 0x01 : 0x04)) != 0;
		buttons[(int)Button::DPAD_RIGHT] = (report_buf[3 + (isLeft ? 2 : 0)] & (isLeft ? 0x04 : 0x08)) != 0;
		buttons[(int)Button::DPAD_UP] = (report_buf[3 + (isLeft ? 2 : 0)] & (isLeft ? 0x02 : 0x02)) != 0;
		buttons[(int)Button::DPAD_LEFT] = (report_buf[3 + (isLeft ? 2 : 0)] & (isLeft ? 0x08 : 0x01)) != 0;
		buttons[(int)Button::HOME] = ((report_buf[4] & 0x10) != 0);
		buttons[(int)Button::MINUS] = ((report_buf[4] & 0x01) != 0);
		buttons[(int)Button::PLUS] = ((report_buf[4] & 0x02) != 0);
		buttons[(int)Button::STICK] = ((report_buf[4] & (isLeft ? 0x08 : 0x04)) != 0);
		buttons[(int)Button::SHOULDER_1] = (report_buf[3 + (isLeft ? 2 : 0)] & 0x40) != 0;
		buttons[(int)Button::SHOULDER_2] = (report_buf[3 + (isLeft ? 2 : 0)] & 0x80) != 0;
		buttons[(int)Button::SR] = (report_buf[3 + (isLeft ? 2 : 0)] & 0x10) != 0;
		buttons[(int)Button::SL] = (report_buf[3 + (isLeft ? 2 : 0)] & 0x20) != 0;

		{
			std::lock_guard<std::mutex> lock(_lockButtonUP);

			{
				std::lock_guard<std::mutex> lock(_lockButtonDown);

				for (int i = 0; i < (sizeof(buttons) / sizeof(buttons[0])); ++i)
				{
					buttons_up[i] = (down_[i] & !buttons[i]);
					buttons_down[i] = (!down_[i] & buttons[i]);
				}
			}
		}
	}
	return 0;

}

void Joycon::ExtractIMUValues(uint8_t* report_buf, int n)
{
	gyr_r[0] = (uint16_t)(report_buf[19 + n * 12] | ((report_buf[20 + n * 12] << 8) & 0xff00));    // n*12はLとRのバッファのズレ？12足すかそのままかで左右のコントローラを判別してる
	gyr_r[1] = (uint16_t)(report_buf[21 + n * 12] | ((report_buf[22 + n * 12] << 8) & 0xff00));
	gyr_r[2] = (uint16_t)(report_buf[23 + n * 12] | ((report_buf[24 + n * 12] << 8) & 0xff00));
	acc_r[0] = (uint16_t)(report_buf[13 + n * 12] | ((report_buf[14 + n * 12] << 8) & 0xff00));
	acc_r[1] = (uint16_t)(report_buf[15 + n * 12] | ((report_buf[16 + n * 12] << 8) & 0xff00));
	acc_r[2] = (uint16_t)(report_buf[17 + n * 12] | ((report_buf[18 + n * 12] << 8) & 0xff00));

	for (int i = 0; i < 3; ++i)
	{
		switch (i)
		{
		case 1:
		{
			acc_g.x = acc_r[i] * 0.00025f;                             // 加速度（G単位）の式？
			gyr_g.x = (gyr_r[i] - gyr_neutral[i]) * 0.00122187695f;    // 回転（度/秒-dps）の式？
			if (abs(acc_g.x) > abs(max.x))							   // 取った値の絶対値が最大値の絶対値より大きかったら
				max.x = acc_g.x;                                       // 最大値を設定しなおす
			break;
		}

		case 2:
		{
			acc_g.y = acc_r[i] * 0.00025f;
			gyr_g.y = (gyr_r[i] - gyr_neutral[i]) * 0.00122187695f;
			if (abs(acc_g.y) > abs(max.y))
				max.y = acc_g.y;
			break;
		}

		case 3:
		{
			acc_g.z = acc_r[i] * 0.00025f;
			gyr_g.z = (gyr_r[i] - gyr_neutral[i]) * 0.00122187695f;
			if (abs(acc_g.z) > abs(max.z))
				max.z = acc_g.z;
			break;
		}

		default:
			break;
		}
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
		sum.x += gyr_g.x * dt_sec;
		sum.y += gyr_g.y * dt_sec;
		sum.z += gyr_g.z * dt_sec;

		if (isLeft)
		{
			gyr_g.y *= -1;
			gyr_g.z *= -1;
			acc_g.y *= -1;
			acc_g.z *= -1;
		}

		if (first_imu_packet)
		{
			i_b = Vector3(1, 0, 0);
			j_b = Vector3(0, 1, 0);
			k_b = Vector3(0, 0, 1);
			first_imu_packet = false;
		}
		else
		{
			k_acc = -acc_g.Normalize();
			w_a = k_b.Cross(k_acc);
			w_g = -gyr_g * dt_sec;
			d_theta = (w_a * filterweight + w_g) / (1.0f + filterweight);
			k_b += d_theta.Cross(k_b);
			i_b += d_theta.Cross(i_b);
			j_b += d_theta.Cross(j_b);
			//Correction, ensure new axes are orthogonal
			err = i_b.Dot(j_b) * 0.5f;
			i_b_ = Vector3(i_b - j_b * err).Normalize();
			j_b = Vector3(j_b - i_b * err).Normalize();
			i_b = i_b_;
			k_b = i_b.Cross(j_b);
		}
		dt = 1;
	}
	timestamp = report_buf[1] + 2;
	return 0;
}

void Joycon::Begin()
{
	// NULLなら
	if (PollThreadObj == NULL)
	{
		// newしてスレッドが終了するまで待機
		PollThreadObj = new std::thread(Poll, this);
		PollThreadObj->join();
	}
}

void Joycon::Recenter()
{
	first_imu_packet = true;
}

Vector2 Joycon::CenterSticks(uint16_t* vals)
{
	float s[2] = { 0, 0 };
	for (unsigned int i = 0; i < 2; ++i)
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

	Vector2 ans;
	ans.x = s[0];
	ans.y = s[1];
	return ans;
}

void Joycon::SetRumble(float low_freq, float high_freq, float amp, int time)
{
	if (state <= state_::ATTACHED) return;
	if (rumble_obj->timed_rumble == false || rumble_obj->t < 0)
	{
		rumble_obj = new Rumble(low_freq, high_freq, amp, time);
	}
}

void Joycon::SendRumble(uint8_t* buf)
{
	uint8_t* buf_ = new uint8_t[report_len];
	buf_[0] = 0x10;
	buf_[1] = global_count;
	if (global_count == 0xf) global_count = 0;
	else ++global_count;
	// コピー元、コピー元のインデックス、コピー先、コピー先インデックス、要素数
	//Array.Copy(buf, 0, buf_, 2, 8);
	// コピー元先頭、コピー元終端、コピー先
	std::copy(buf, (buf+8), buf_);	// 要素数絶対8個だから8足してるけど後で直して

	PrintArray(buf_, DebugType::RUMBLE, 0, 0, "Rumble data sent: {0:S}");
	hid_write(handle, buf_, sizeof(report_len));
}

uint8_t* Joycon::Subcommand(uint8_t sc, uint8_t* buf, unsigned int len, bool print)
{
	uint8_t* buf_ = new uint8_t[report_len];
	uint8_t* response = new uint8_t[report_len];

	//Array.Copy(default_buf, 0, buf_, 2, 8);
	//Array.Copy(buf, 0, buf_, 11, len);

	memcpy(buf_ + 2, default_buf, 8);
	memcpy(buf_ + 11, buf, len);

	buf_[10] = sc;
	buf_[1] = global_count;
	buf_[0] = 0x1;
	if (global_count == 0xf) global_count = 0;
	else ++global_count;
	if (print) { PrintArray(buf_, DebugType::COMMS, len, 11, "Subcommand 0x" + _StringFormat::Format("{0:X2}", sc) + " sent. Data: 0x{0:S}"); };
	hid_write(handle, buf_, sizeof(len + 11));
	int res = hid_read_timeout(handle, response, sizeof(report_len), 50);
	if (res < 1) DebugPrint("No response.", DebugType::COMMS);
	else if (print) { PrintArray(response, DebugType::COMMS, report_len - 1, 1, "Response ID 0x" + _StringFormat::Format("{0:X2}", response[0]) + ". Data: 0x{0:S}"); }
	return response;
}

void Joycon::dump_calibration_data()
{
	uint8_t* buf_ = ReadSPI(0x80, (isLeft ? (uint8_t)0x12 : (uint8_t)0x1d), 9); // get user calibration data if possible
	bool found = false;
	for (int i = 0; i < 9; ++i)
	{
		if (buf_[i] != 0xff)
		{
			printf("Using user stick calibration data.");
			found = true;
			break;
		}
	}
	if (!found)
	{
		printf("Using factory stick calibration data.");
		buf_ = ReadSPI(0x60, (isLeft ? (uint8_t)0x3d : (uint8_t)0x46), 9); // get user calibration data if possible
	}
	stick_cal[isLeft ? 0 : 2] = (uint16_t)((buf_[1] << 8) & 0xF00 | buf_[0]); // X Axis Max above center
	stick_cal[isLeft ? 1 : 3] = (uint16_t)((buf_[2] << 4) | (buf_[1] >> 4));  // Y Axis Max above center
	stick_cal[isLeft ? 2 : 4] = (uint16_t)((buf_[4] << 8) & 0xF00 | buf_[3]); // X Axis Center
	stick_cal[isLeft ? 3 : 5] = (uint16_t)((buf_[5] << 4) | (buf_[4] >> 4));  // Y Axis Center
	stick_cal[isLeft ? 4 : 0] = (uint16_t)((buf_[7] << 8) & 0xF00 | buf_[6]); // X Axis Min below center
	stick_cal[isLeft ? 5 : 1] = (uint16_t)((buf_[8] << 4) | (buf_[7] >> 4));  // Y Axis Min below center

	//PrintArray(stick_cal, len: 6, start : 0, format : "Stick calibration data: {0:S}");

	buf_ = ReadSPI(0x60, (isLeft ? (uint8_t)0x86 : (uint8_t)0x98), 16);
	deadzone = (uint16_t)((buf_[4] << 8) & 0xF00 | buf_[3]);

	buf_ = ReadSPI(0x80, 0x34, 10);
	gyr_neutral[0] = (uint16_t)(buf_[0] | ((buf_[1] << 8) & 0xff00));
	gyr_neutral[1] = (uint16_t)(buf_[2] | ((buf_[3] << 8) & 0xff00));
	gyr_neutral[2] = (uint16_t)(buf_[4] | ((buf_[5] << 8) & 0xff00));
	//PrintArray(gyr_neutral, len: 3, d : DebugType.IMU, format : "User gyro neutral position: {0:S}");

	// This is an extremely messy way of checking to see whether there is user stick calibration data present, but I've seen conflicting user calibration data on blank Joy-Cons. Worth another look eventually.
	if (gyr_neutral[0] + gyr_neutral[1] + gyr_neutral[2] == -3 || abs(gyr_neutral[0]) > 100 || abs(gyr_neutral[1]) > 100 || abs(gyr_neutral[2]) > 100)
	{
		buf_ = ReadSPI(0x60, 0x29, 10);
		gyr_neutral[0] = (uint16_t)(buf_[3] | ((buf_[4] << 8) & 0xff00));
		gyr_neutral[1] = (uint16_t)(buf_[5] | ((buf_[6] << 8) & 0xff00));
		gyr_neutral[2] = (uint16_t)(buf_[7] | ((buf_[8] << 8) & 0xff00));
		//PrintArray(gyr_neutral, len: 3, d : DebugType.IMU, format : "Factory gyro neutral position: {0:S}");
	}
}

uint8_t* Joycon::ReadSPI(uint8_t addr1, uint8_t addr2, unsigned int len, bool print)
{
	uint8_t buf[5] = { addr2, addr1, 0x00, 0x00, (uint8_t)len };		// ←要素数マジックナンバー、直して
	uint8_t* read_buf = new uint8_t[len];
	uint8_t* buf_ = new uint8_t[len + 20];

	for (int i = 0; i < 100; ++i)
	{
		buf_ = Subcommand(0x10, buf, 5, false);
		if (buf_[15] == addr2 && buf_[16] == addr1)
		{
			break;
		}
	}
	//Array.Copy(buf_, 20, read_buf, 0, len);
	std::copy(buf_, (buf_ + len), read_buf);
	if (print) PrintArray(read_buf, DebugType::COMMS, len);
	return read_buf;
}

template<class T> void Joycon::PrintArray(T* arr, DebugType d, uint8_t len, uint8_t start, std::string format)
{
	if (d != debug_type && debug_type != DebugType::ALL) return;

	std::string tostr = "";
	//for (int i = 0; i < len; ++i)
	//{
	//						// 配列がbyte型なら{0:X2}、float型なら{0:F}、どちらでもないなら{0:D}
	//	//tostr += string.Format((arr[0] is byte) ? "{0:X2} " : ((arr[0] is float) ? "{0:F} " : "{0:D} "), arr[i + start]);
	//}
	////DebugPrint(string.Format(format, tostr), d);
	for (int i = 0; i < len; ++i)
	{
		//tostr += _StringFormat::Format((typeid(arr[0]) == typeid(int)) ? ((std::string)"%d") : ((typeid(arr[0]) == typeid(float)) ? ((std::string)"%f") : ((std::string)"%s")), arr[i]);
	}
	DebugPrint(_StringFormat::Format(format, tostr), debug_type);

}