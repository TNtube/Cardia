#pragma once


namespace Cardia
{
	class RendererContext
	{
	public:
		virtual ~RendererContext() = default;
		virtual void init() = 0;
		virtual void swapBuffers() = 0;
	};
}