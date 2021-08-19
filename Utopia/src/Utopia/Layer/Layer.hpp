#pragma once

#include "Utopia/Core.hpp"
#include "Utopia/Event.hpp"

namespace Utopia
{
	class UTOPIA_API Layer
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
