#pragma once

namespace Utopia
{
	class VertexBuffer
	{
	public:
		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		static VertexBuffer* create(float* vertices, unsigned size);
	};

	class IndexBuffer
	{
	public:
		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual unsigned getCount() const = 0;
		static IndexBuffer* create(unsigned* indices, unsigned count);
	};

}