#pragma once

#include "Utopia/Layer/Layer.hpp"

namespace Utopia
{
	class UTOPIA_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() override = default;

		void onPush() override;
		void onPop() override;
		void onUpdate() override;
		void onEvent(Event& event) override;
	private:
		bool onMouseButtonDownEvent(const MouseButtonDownEvent& e);
		bool onMouseButtonUpEvent(const MouseButtonUpEvent& e);
		bool onMouseMotionEvent(const MouseMotionEvent& e);
		bool onMouseScrolledEvent(const MouseScrolledEvent& e);
		bool onKeyDownEvent(const KeyDownEvent& e);
		bool onKeyUpEvent(const KeyUpEvent& e);
		bool onKeyTypedEvent(const KeyTypedEvent& e);
		bool onWinResizeEvent(const WinResizeEvent& e);


		float m_Time {};
	};
}
