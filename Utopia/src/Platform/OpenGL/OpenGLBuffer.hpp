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
		void setLayout(const BufferLayout& layout) override { m_Layout = layout; }
		const BufferLayout& getLayout() const override { return m_Layout; }

	private:
		unsigned m_VertexBufferID {};
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(unsigned* indices, unsigned count);
		~OpenGLIndexBuffer();
		void bind() const override;
		void unbind() const override;
		inline int getCount() const override { return m_Count; }

	private:
		unsigned m_IndexBufferID {};
		int m_Count {};
	};
}