#pragma once

#include "Cardia/Renderer/VertexArray.hpp"


namespace Cardia
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray() = default;
		void bind() const override;
		void unbind() const override;
		void addVertexBuffer (std::unique_ptr<VertexBuffer> vertexBuffer) override;
		void setIndexBuffer (std::unique_ptr<IndexBuffer> indexBuffer) override;
		virtual std::vector<std::unique_ptr<VertexBuffer>>& getVertexBuffers() override;
		virtual IndexBuffer& getIndexBuffer () const override;

	private:
		uint32_t m_VertexArrayID{};
		std::vector<std::unique_ptr<VertexBuffer>> m_VertexBuffers;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
	};
}