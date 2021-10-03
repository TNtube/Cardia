#pragma once
#include "Cardia/Renderer/Texture.hpp"

namespace Cardia
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		explicit OpenGLTexture2D(std::string path);
		virtual ~OpenGLTexture2D();

		inline uint32_t getWidth() const override { return m_Width; }
		inline uint32_t getHeight() const override { return m_Height; }

		void bind(int sloth) const override;

	private:
		std::string m_Path;
		int32_t m_Width, m_Height;
		uint32_t m_TextureID;
	};
}
