#pragma once
#include "../Joycon/Joycon.h"
#include "../Joycon/JoyconManager.h"
#include "../GameObject/GameObject.h"

namespace JimaXengine
{
	class JoyconTest :public GameObject
	{
	private:
		//const static Joycon::Button m_buttons[] = Enum.GetValues(typeof(Joycon.Button)) as Joycon.Button[]; // �z���enum�̕��тŏ������g�p�Ƃ��Ă�H
		const static Joycon::Button m_buttons[];

		std::vector<Joycon*> m_joycons;
		Joycon* m_joyconL;
		Joycon* m_joyconR;
		//Joycon::Button ? m_pressedButtonL;      // null���e�^
		//Joycon::Button ? m_pressedButtonR;
		Joycon::Button* m_pressedButtonL;
		Joycon::Button* m_pressedButtonR;

	public:
		void Start();
		//void Update();
		void OnGUI();
		void SetControllers();

		// �������z�֐�
		JoyconTest();
		~JoyconTest();
		void Initialize();
		void Update();
		void Draw();
		// ���z�֐�
		void Dead();
		void Delete();
		TYPE GetType();
		RENDER_TYPE GetRenderType();
		void DrawImGui();

	};
}