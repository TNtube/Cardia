#pragma once

namespace Utopia
{
	class RendererContext
	{
	public:
		virtual void init() = 0;
		virtual void swapBuffers() = 0;
	};
}