#include "cdpch.hpp"
#include "Cardia/Renderer/OpenGL/OpenGLFramebuffer.hpp"
#include "Cardia/Core/Core.hpp"

#include <glad/glad.h>

#include <utility>


namespace Cardia {

	static GLenum TextureTarget(bool multisampled)
	{
		return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}

	static void AttachColorTexture(uint32_t id, uint32_t samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, uint32_t index)
	{
		bool multisampled = samples > 1;
		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
	}

	static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
	{
		bool multisampled = samples > 1;
		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
		}
		else
		{
			glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
	}

	OpenGLFramebuffer::OpenGLFramebuffer(FramebufferSpec spec) : m_Spec(std::move(spec))
	{
		for (auto attachment : m_Spec.attachments.Attachments)
		{
			if (attachment.TextureFormat != FramebufferTextureFormat::DEPTH24STENCIL8)
				m_ColorAttachmentSpecifications.emplace_back(attachment);
			else
				m_DepthAttachmentSpecification = attachment;
		}
		GenerateFramebuffer();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_FramebufferID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFramebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
		glViewport(0, 0, m_Spec.width, m_Spec.height);
	}

	void OpenGLFramebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(int width, int height)
	{
		if (width <= 0 || height <= 0) // temporary values, but I don't think that any screen could override those values
		{
			Log::coreWarn("Attempting to Resize Framebuffer with invalid value : w {0}, h {1}", width,
				      height);
			return;
		}

		m_Spec.width = width;
		m_Spec.height = height;
		GenerateFramebuffer();
	}

	void OpenGLFramebuffer::GenerateFramebuffer()
	{
		if (m_FramebufferID)
		{
			glDeleteFramebuffers(1, &m_FramebufferID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glGenFramebuffers(1, &m_FramebufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
		bool multisample = m_Spec.samples > 1;


		if (m_ColorAttachmentSpecifications.size()) {
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			glCreateTextures(TextureTarget(multisample), m_ColorAttachments.size(), m_ColorAttachments.data());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				glBindTexture(TextureTarget(multisample), m_ColorAttachments[i]);
				switch (m_ColorAttachmentSpecifications[i].TextureFormat)
				{
					case FramebufferTextureFormat::RGBA8:
						AttachColorTexture(m_ColorAttachments[i], m_Spec.samples, GL_RGBA8, GL_RGBA, m_Spec.width, m_Spec.height, i);
						break;
					case FramebufferTextureFormat::RED_INTEGER:
						AttachColorTexture(m_ColorAttachments[i], m_Spec.samples, GL_R32I, GL_RED_INTEGER, m_Spec.width, m_Spec.height, i);
						break;
					default:break;
				}
			}
		}

		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
		{
			glCreateTextures(TextureTarget(multisample), 1, &m_DepthAttachment);
			glBindTexture(TextureTarget(multisample), m_DepthAttachment);
			switch (m_DepthAttachmentSpecification.TextureFormat)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:
					AttachDepthTexture(m_DepthAttachment,  m_Spec.samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Spec.width, m_Spec.height);
					break;
				default:break;
			}
		}

		if (m_ColorAttachments.size() > 1)
		{
			cdCoreAssert(m_ColorAttachments.size() <= 4, "Color attachment size cannot be less than 4");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			// Only depth-pass
			glDrawBuffer(GL_NONE);
		}

		cdCoreAssert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		cdCoreAssert(attachmentIndex < m_ColorAttachments.size(), "No attachment found");

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;

	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		cdCoreAssert(attachmentIndex < m_ColorAttachments.size(), "No attachment found");

		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];

		auto textFormat = GL_NONE;
		switch (spec.TextureFormat)
		{
			case FramebufferTextureFormat::RGBA8:       textFormat = GL_RGBA8;
			case FramebufferTextureFormat::RED_INTEGER: textFormat = GL_RED_INTEGER;
			default:break;
		}

		glClearTexImage(m_ColorAttachments[attachmentIndex], 0, textFormat, GL_INT, &value);
	}
}