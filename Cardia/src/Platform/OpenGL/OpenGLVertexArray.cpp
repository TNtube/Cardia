#include "cdpch.hpp"
#include "OpenGLVertexArray.hpp"

#include <glad/glad.h>

namespace Cardia
{static GLenum ShaderDataTypeToOpenGLType(ShaderDataType type)
{
	switch (type)
	{
		case ShaderDataType::Float:
		case ShaderDataType::Float2:
		case ShaderDataType::Float3:
		case ShaderDataType::Float4:
		case ShaderDataType::Mat3:
		case ShaderDataType::Mat4:
			return GL_FLOAT;
		case ShaderDataType::Int:
		case ShaderDataType::Int2:
		case ShaderDataType::Int3:
		case ShaderDataType::Int4:
			return GL_INT;
		case ShaderDataType::Bool:
			return GL_BOOL;
		default:
			cdCoreAssert(false, "Unknown ShaderDataType.");
			return 0;
	}
}
	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_VertexArrayID);
	}

	void OpenGLVertexArray::bind() const
	{
		glBindVertexArray(m_VertexArrayID);
	}

	void OpenGLVertexArray::unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::addVertexBuffer(std::unique_ptr<VertexBuffer> vertexBuffer)
	{
		cdCoreAssert(vertexBuffer->getLayout().getElement().size(), "Vertex Buffer should have a layout");

		glBindVertexArray(m_VertexArrayID);
		vertexBuffer->bind();

		uint32_t index = 0;
		auto layout = vertexBuffer->getLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
					      element.getElementCount(),
					      ShaderDataTypeToOpenGLType(element.type),
					      element.normalized ? GL_TRUE : GL_FALSE,
					      layout.getStride(),
					      reinterpret_cast<const void*>(element.offset));
			index++;
		}
		m_VertexBuffers.push_back(std::move(vertexBuffer));
	}

	void OpenGLVertexArray::setIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer)
	{
		glBindVertexArray(m_VertexArrayID);
		indexBuffer->bind();
		m_IndexBuffer = std::move(indexBuffer);
	}

	std::vector<std::unique_ptr<VertexBuffer>>& OpenGLVertexArray::getVertexBuffers()
	{
		return m_VertexBuffers;
	}

	IndexBuffer& OpenGLVertexArray::getIndexBuffer() const
	{
		return *m_IndexBuffer;
	}
}