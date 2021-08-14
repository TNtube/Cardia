#pragma once

#include "Utopia/Layer/Layer.hpp"

namespace Utopia
{
	class UTOPIA_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void onPush();
		void onPop();
		void onUpdate();
		void onEvent(Event& event);
	private:
		float m_Time {};
	};
}
