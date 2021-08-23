#include "utpch.hpp"
#include "OpenGLVertexArray.hpp"

#include <glad/glad.h>

namespace Utopia
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
			utCoreAssert(false, "Unknown ShaderDataType.");
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

	void OpenGLVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer)
	{
		utCoreAssert(vertexBuffer->getLayout().getElement().size(), "Vertex Buffer should have a layout");

		glBindVertexArray(m_VertexArrayID);
		vertexBuffer->bind();

		unsigned index = 0;
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
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer)
	{
		glBindVertexArray(m_VertexArrayID);
		indexBuffer->bind();
		m_IndexBuffer = indexBuffer;
	}

	const std::vector<std::shared_ptr<VertexBuffer>> OpenGLVertexArray::getVertexBuffers() const
	{
		return m_VertexBuffers;
	}

	const std::shared_ptr<IndexBuffer> OpenGLVertexArray::getIndexBuffer() const
	{
		return m_IndexBuffer;
	}
}