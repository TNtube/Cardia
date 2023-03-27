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

		inline bool operator==(const Texture& other) const override {
			return m_TextureID == ((OpenGLTexture2D&)other).m_TextureID;
		}

		void Bind(int slot) const override;
		uint32_t GetRendererID() override;

	private:
		int32_t m_Width{}, m_Height{};
		uint32_t m_TextureID;
	};
}
