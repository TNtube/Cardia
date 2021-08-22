#pragma once
#include "Utopia/Renderer/Buffer.hpp"

namespace Utopia
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, unsigned size);
		~OpenGLVertexBuffer();
		void bind() const override;
		void unbind() const override;

	private:
		unsigned m_VertexBuffer {};
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(unsigned* indices, unsigned count);
		~OpenGLIndexBuffer();
		void bind() const override;
		void unbind() const override;
		inline unsigned getCount() const override { return m_Count; }

	private:
		unsigned m_IndexBuffer {};
		unsigned m_Count {};
	};
}