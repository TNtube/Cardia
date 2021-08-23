#pragma once
#include "Utopia/Renderer/VertexArray.hpp"


namespace Utopia
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray() = default;
		void bind() const override;
		void unbind() const override;
		void addVertexBuffer (const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		void setIndexBuffer (const std::shared_ptr<IndexBuffer>& indexBuffer) override;
		virtual const std::vector<std::shared_ptr<VertexBuffer>> getVertexBuffers () const override;
		virtual const std::shared_ptr<IndexBuffer> getIndexBuffer () const override;

	private:
		unsigned m_VertexArrayID{};
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};
}