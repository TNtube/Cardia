#pragma once
#include "Cardia/Renderer/Buffer.hpp"

namespace Cardia
{
	class VertexArray
	{
	public:
		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual void addVertexBuffer (std::unique_ptr<VertexBuffer> vertexBuffer) = 0;
		virtual void setIndexBuffer (std::unique_ptr<IndexBuffer> indexBuffer) = 0;
		virtual std::vector<std::unique_ptr<VertexBuffer>>& getVertexBuffers () = 0;
		virtual IndexBuffer& getIndexBuffer () const = 0;


		static VertexArray* create();
	};
}