#pragma once

#include "Cardia/Renderer/Texture.hpp"


namespace Cardia
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		explicit OpenGLTexture2D(std::string path);
		explicit OpenGLTexture2D(int width, int height, void* data);
		virtual ~OpenGLTexture2D() override;

		inline uint32_t getWidth() const override { return m_Width; }
		inline uint32_t getHeight() const override { return m_Height; }

		inline bool operator==(const Texture& other) const override {
			return m_TextureID == ((OpenGLTexture2D&)other).m_TextureID;
		}

		void bind(int slot) const override;
		uint32_t getRendererID() override;

	private:
		std::string m_Path;
		int32_t m_Width, m_Height;
		uint32_t m_TextureID;
	};
}
