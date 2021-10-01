#pragma once

#include <memory>
#include <string>


namespace Cardia
{
	class Texture {
	public:
		virtual int getHeight() const = 0;
		virtual int getWidth() const = 0;

		virtual void bind(int sloth = 0) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static std::unique_ptr<Texture2D> create(const std::string& path);
	};
}