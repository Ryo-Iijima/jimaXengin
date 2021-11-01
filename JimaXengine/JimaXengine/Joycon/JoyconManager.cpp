#include "JoyconManager.h"

JoyconManager* JoyconManager::instance = new JoyconManager();

JoyconManager* JoyconManager::Instance()
{
	//instance = new JoyconManager();
    return instance;
}

void JoyconManager::Awake()
{
	int i = 0;

	bool isLeft = false;
	hid_init();

	hid_device_info* pEnumrate = hid_enumerate(vendor_id, 0x0);
	hid_device_info* top_ptr = pEnumrate;

	if (pEnumrate == 0)
	{
		pEnumrate = hid_enumerate(vendor_id_, 0x0);
		if (pEnumrate == 0)
		{
			hid_free_enumeration(pEnumrate);
			printf("No Joy-Cons found!\n");
		}
	}

	while (pEnumrate != 0)
	{
		printf("productID:%d\n", pEnumrate->product_id);
		if (pEnumrate->product_id == product_l || pEnumrate->product_id == product_r) {
			if (pEnumrate->product_id == product_l) {
				isLeft = true;
				printf("Left Joy-Con connected.\n");
			}
			else if (pEnumrate->product_id == product_r) {
				isLeft = false;
				printf("Right Joy-Con connected.\n");
			}
			else {
				printf("Non Joy-Con input device skipped.\n");
			}
			hid_device* handle = hid_open_path(pEnumrate->path);
			hid_set_nonblocking(handle, 1);
			//j.push_back(nullptr);
			j.push_back(new Joycon(handle, EnableIMU, EnableLocalize & EnableIMU, 0.05f, isLeft));
			++i;
		}
		pEnumrate = pEnumrate->next;
	}
	hid_free_enumeration(top_ptr);
}

void JoyconManager::Start()
{
	for (int i = 0; i < j.size(); ++i)
	{
		printf("%d\n",i);
		Joycon* jc = j[i];
		uint8_t LEDs = 0x0;
		LEDs |= (uint8_t)(0x1 << i);
		jc->Attach(LEDs);
		jc->Begin();
	}
}

void JoyconManager::Update()
{
	for (int i = 0; i < j.size(); ++i)
	{
		j[i]->Update();
	}
}

void JoyconManager::OnApplicationQuit()
{
	for (int i = 0; i < j.size(); ++i)
	{
		j[i]->Detach();
	}
}