#pragma once
#include "Joycon.h"
#include <vector>

namespace JimaXengine
{
	class JoyconManager
	{
	private:
		bool EnableIMU = true;
		bool EnableLocalize = true;

		static const unsigned short vendor_id = 0x57e;
		static const unsigned short vendor_id_ = 0x057e;
		static const unsigned short product_l = 0x2006;
		static const unsigned short product_r = 0x2007;

		static JoyconManager* instance;

	public:
		std::vector<Joycon*> j = {}; // Array of all connected Joy-Cons

	public:
		static JoyconManager* Instance();
		// Joy-Con ê⁄ë±
		void Awake();

		void Start();
		void Update();
		void OnApplicationQuit();

	};
}