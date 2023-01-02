#pragma once

#include "Cardia/ECS/Entity.hpp"
#include "Cardia/ECS/Components.hpp"

namespace Cardia
{
	class Command
	{
	public:
		virtual ~Command() = default;
		virtual void Redo(Application* ctx) = 0;
		virtual void Undo(Application* ctx) = 0;
	};

	class UpdateTransformPositionCommand : public Command
	{
	public:
		UpdateTransformPositionCommand(const UUID& uuid, glm::vec3 oldPosition);
		void Redo(Application* ctx) override;
		void Undo(Application* ctx) override;
	private:
		UUID m_UUID;
		glm::vec3 m_OldPosition;
	};
}
