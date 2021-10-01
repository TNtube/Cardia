#pragma once
#include "Cardia/Renderer/Texture.hpp"

namespace Cardia
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		explicit OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		inline int getWidth() const override;
		inline int getHeight() const override;

		void bind(int sloth = 0) const override;
	};
}
