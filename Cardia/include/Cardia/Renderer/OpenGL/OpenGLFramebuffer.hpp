#pragma once

#include "Cardia/Renderer/Framebuffer.hpp"


namespace Cardia
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		explicit OpenGLFramebuffer(FramebufferSpec spec);
		virtual ~OpenGLFramebuffer() override;

		void Bind() const override;
		void Unbind() const override;

		void Resize(int width, int height) override;
		int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		void ClearAttachment(uint32_t attachmentIndex, int value) override;

		inline uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { return m_ColorAttachments[index]; }

	private:
		void GenerateFramebuffer();

	private:
		uint32_t m_FramebufferID {};
		FramebufferSpec m_Spec;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
 	};
}