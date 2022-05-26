#pragma once

#include "Cardia/Renderer/Framebuffer.hpp"


namespace Cardia
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		explicit OpenGLFramebuffer(FramebufferSpec spec);
		virtual ~OpenGLFramebuffer() override;

		void bind() const override;
		void unbind() const override;

		void resize(int width, int height) override;

		inline uint32_t getColorAttachmentRendererID() const override { return m_ColorAttachment; }

	private:
		void createFramebuffer();

	private:
		uint32_t m_FramebufferID {};
		uint32_t m_ColorAttachment {};
		uint32_t m_RenderAttachment {};
		FramebufferSpec m_Spec;
 	};
}