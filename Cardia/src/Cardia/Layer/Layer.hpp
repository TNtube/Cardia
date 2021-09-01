#pragma once

#include "Cardia/Core.hpp"
#include "Cardia/Event.hpp"

namespace Cardia
{
	class Layer
	{
	public:
		explicit Layer(std::string name = "Layer")
			: m_Name(std::move(name)) {}
		virtual ~Layer() = default;

		virtual void onPush() {}
		virtual void onPop() {}
		virtual void onUpdate() {}
		virtual void onImGuiDraw() {}
		virtual void onEvent(Event& event) {}

		const std::string& getName() const { return m_Name; }			// for debugging purpose

	protected:
		std::string m_Name;												// For debugging purpose
	};
}
