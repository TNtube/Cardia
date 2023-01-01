#pragma once

#include "Cardia/ECS/Entity.hpp"
#include "Cardia/ECS/Components.hpp"

namespace Cardia
{
	class Command
	{
	public:
		virtual ~Command() = default;
		virtual void Redo() = 0;
		virtual void Undo() = 0;
	};


	using namespace Component;

	class UpdateTransformPositionCommand : public Command
	{
	public:
		UpdateTransformPositionCommand(Component::Transform& transform, glm::vec3 oldPosition);
		void Redo() override;
		void Undo() override;
	private:
		Component::Transform& m_Transform;
		glm::vec3 m_OldPosition;
	};
}
