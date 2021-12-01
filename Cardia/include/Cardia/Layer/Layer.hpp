#pragma once

#include "Cardia/Core/Event.hpp"
#include "Cardia/Core/Time.hpp"


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
		virtual void onUpdate(DeltaTime deltaTime) {}
		virtual void onImGuiDraw(DeltaTime deltaTime) {}
		virtual void onEvent(Event& event) {}

		const std::string& getName() const { return m_Name; }			// for debugging purpose

	protected:
		std::string m_Name;												// For debugging purpose
	};
}
