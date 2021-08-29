#pragma once
#include "Utopia/Renderer/RenderCommand.hpp"

namespace Utopia
{
	class OpenGLRenderCommand : public RenderCommand
	{
	public:
		void setViewPort(int x, int y, int w, int h) const override;
		void setWireFrame(bool state) const override;
	};
}

