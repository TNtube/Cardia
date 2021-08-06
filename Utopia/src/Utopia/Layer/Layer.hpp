#pragma once

#include "Utopia/Core.hpp"
#include "Utopia/Event.hpp"

namespace Utopia
{
	class UTOPIA_API Layer
	{
	public:
		Layer(std::string name = "Layer")
			: m_Name(std::move(name)) {}
		virtual ~Layer() {}

		virtual void onPush() {}
		virtual void onPop() {}
		virtual void onUpdate() {}
		virtual void onEvent(Event& event) {}

		inline const std::string& getName() const { return m_Name; }			// for debugging purpose

	protected:
		std::string m_Name;														// For debugging purpose
	};
}
