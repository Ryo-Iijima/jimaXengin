#pragma once
#include "../Joycon/Joycon.h"
#include "../Joycon/JoyconManager.h"
#include "../GameObject/GameObject.h"

namespace JimaXengine
{
	class JoyconTest :public GameObject
	{
	private:
		//const static Joycon::Button m_buttons[] = Enum.GetValues(typeof(Joycon.Button)) as Joycon.Button[]; // 配列をenumの並びで初期化使用としてる？
		const static Joycon::Button m_buttons[];

		std::vector<Joycon*> m_joycons;
		Joycon* m_joyconL;
		Joycon* m_joyconR;
		//Joycon::Button ? m_pressedButtonL;      // null許容型
		//Joycon::Button ? m_pressedButtonR;
		Joycon::Button* m_pressedButtonL;
		Joycon::Button* m_pressedButtonR;

	public:
		void Start();
		//void Update();
		void OnGUI();
		void SetControllers();

		// 純粋仮想関数
		JoyconTest();
		~JoyconTest();
		void Initialize();
		void Update();
		void Draw();
		// 仮想関数
		void Dead();
		void Delete();
		TYPE GetType();
		RENDER_TYPE GetRenderType();
		void DrawImGui();

	};
}