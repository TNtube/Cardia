#pragma once

#include "Cardia/Layer/Layer.hpp"
#include "Cardia/Core/Time.hpp"

namespace Cardia
{
	class ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer() override = default;

		void onPush() override;
		void onPop() override;
		void onImGuiDraw(DeltaTime deltaTime) override;

		void Begin();
		void End();


		float m_Time {};
	};
}
