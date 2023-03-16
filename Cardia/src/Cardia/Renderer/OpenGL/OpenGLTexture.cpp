#include "cdpch.hpp"
#include "Cardia/Renderer/OpenGL/OpenGLTexture.hpp"
#include "Cardia/Core/Core.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#include <glad/glad.h>


namespace Cardia
{
	OpenGLTexture2D::OpenGLTexture2D(std::string path)
		:m_Width(), m_Height(), m_TextureID()
	{
		m_Path = std::move(path);
		stbi_set_flip_vertically_on_load(true);

		int width, height, nbChannels;
		Log::coreInfo("Loading {0}...", m_Path);
		unsigned char *data = stbi_load(m_Path.c_str(), &width, &height, &nbChannels, 0);
		m_Loaded = true;
		if (!data)
		{
			Log::coreWarn("Invalid image");
			m_Loaded = false;
			return;
		}

		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (nbChannels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
			m_IsTransparent = true;
		}
		else if (nbChannels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		else
		{
			cdCoreAssert(false, "Unsupported image format");
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
		glTextureStorage2D(m_TextureID, 1, internalFormat, m_Width, m_Height);

		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

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
		if (m_Loaded) glDeleteTextures(1, &m_TextureID);
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