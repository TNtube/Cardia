#pragma once

#include "Cardia/Core/Event.hpp"
#include "Cardia/Core/Time.hpp"


namespace Cardia
{
	class ImGuiLayer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void onEvent(Event &event);

		void Begin();
		void End();


		float m_Time {};
	};
}
