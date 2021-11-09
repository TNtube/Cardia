#include "cdpch.hpp"
#include "Cardia/Renderer/OpenGL/OpenGLTexture.hpp"
#include "Cardia/Core/Core.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include <glad/glad.h>


namespace Cardia
{
	OpenGLTexture2D::OpenGLTexture2D(std::string path)
		: m_Path(std::move(path)), m_Width(), m_Height(), m_TextureID()
	{
		stbi_set_flip_vertically_on_load(true);

		int width, height, nbChannels;
		unsigned char *data = stbi_load(m_Path.c_str(), &width, &height, &nbChannels, 3);
		cdCoreAssert(data, "Failed to load image !");

		m_Width = width;
		m_Height = height;

		GLenum channelType = GL_RGB, channelSize = GL_RGB8;
		if (nbChannels == 3) {
			channelType = GL_RGB;
			channelSize = GL_RGB8;
		}
		else if (nbChannels == 4) {
			Log::coreTrace("FOOBAR");
			channelType = GL_RGBA;
			channelSize = GL_RGBA8;
		}
		else
			cdCoreAssert(false, "Invalid number of channel in image");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
		glTextureStorage2D(m_TextureID, 1, channelSize, m_Width, m_Height);

		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, channelType, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::OpenGLTexture2D(int width, int height, void* data)
		: m_Width(width), m_Height(height), m_TextureID()
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
		glTextureStorage2D(m_TextureID, 1, GL_RGBA8, m_Width, m_Height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, data);

	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_TextureID);
	}

	void OpenGLTexture2D::bind(int slot) const
	{
		glBindTextureUnit(slot, m_TextureID);
	}

	uint32_t OpenGLTexture2D::getRendererID()
	{
		return m_TextureID;
	}
}