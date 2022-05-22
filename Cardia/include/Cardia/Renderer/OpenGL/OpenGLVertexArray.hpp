#pragma once

#include "Cardia/Renderer/VertexArray.hpp"


namespace Cardia
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray() override = default;
		void bind() const override;
		void unbind() const override;
		void setVertexBuffer (std::unique_ptr<VertexBuffer> vertexBuffer) override;
		void setIndexBuffer (std::unique_ptr<IndexBuffer> indexBuffer) override;
		virtual VertexBuffer& getVertexBuffer() override;
		virtual IndexBuffer& getIndexBuffer () const override;

	private:
		uint32_t m_VertexArrayID{};
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
	};
}