#pragma once
#include "Utopia/Renderer/Buffer.hpp"

namespace Utopia
{
	class VertexArray
	{
	public:
		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual void addVertexBuffer (const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;
		virtual void setIndexBuffer (const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;
		virtual const std::vector<std::shared_ptr<VertexBuffer>> getVertexBuffers () const = 0;
		virtual const std::shared_ptr<IndexBuffer> getIndexBuffer () const = 0;


		static VertexArray* create();
	};
}