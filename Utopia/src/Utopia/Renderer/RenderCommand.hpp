#pragma once

namespace Utopia
{
	class RenderCommand
	{
	public:
		virtual void setViewPort(int x, int y, int w, int h) const {};
		virtual void setWireFrame(bool state) const {};

		static RenderCommand* create();
	};
}
