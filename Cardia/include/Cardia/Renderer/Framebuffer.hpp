#pragma once


namespace Cardia
{
	struct FramebufferSpec
	{
		int width;
		int height;

	};

	class Framebuffer
	{
	public:
		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void resize(int width, int height) = 0;
		virtual uint32_t getColorAttachmentRendererID() const = 0;

		static std::unique_ptr<Framebuffer> create(FramebufferSpec spec);

	};
}