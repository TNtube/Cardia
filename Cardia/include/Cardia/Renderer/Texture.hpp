#pragma once

#include <memory>
#include <string>


namespace Cardia
{
	class Texture {
	public:
		virtual uint32_t getHeight() const = 0;
		virtual uint32_t getWidth() const = 0;

		virtual void bind(int slot = 0) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static std::unique_ptr<Texture2D> create(const std::string& path);
		static std::unique_ptr<Texture2D> create(int width, int height, void* data);
	};
}