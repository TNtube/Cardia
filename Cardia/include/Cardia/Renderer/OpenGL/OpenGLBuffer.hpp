#pragma once

#include "Cardia/Renderer/Buffer.hpp"


namespace Cardia
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		OpenGLVertexBuffer(uint32_t size);
		~OpenGLVertexBuffer() override;
		void bind() const override;
		void unbind() const override;
		void setData(const void* data, uint32_t size) override;
		void setLayout(const BufferLayout& layout) override { m_Layout = layout; }
		const BufferLayout& getLayout() const override { return m_Layout; }

	private:
		uint32_t m_VertexBufferID {};
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		explicit OpenGLIndexBuffer(uint32_t count);
		~OpenGLIndexBuffer() override;
		void bind() const override;
		void unbind() const override;
		void setData(const void* data, uint32_t size) override;
		inline int getCount() const override { return m_Count; }

	private:
		uint32_t m_IndexBufferID {};
		int m_Count {};
	};
}