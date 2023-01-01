#include "Command/Commands.hpp"

namespace Cardia
{

	UpdateTransformPositionCommand::UpdateTransformPositionCommand(Transform& transform, glm::vec3 oldPosition)
		: m_Transform(transform), m_OldPosition(oldPosition) {}

	void UpdateTransformPositionCommand::Redo()
	{
		auto temp = m_Transform.position;
		m_Transform.position = m_OldPosition;

		m_OldPosition = temp;
	}

	void UpdateTransformPositionCommand::Undo()
	{
		auto temp = m_Transform.position;
		m_Transform.position = m_OldPosition;

		m_OldPosition = temp;
	}
}
