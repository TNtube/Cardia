#pragma once
#include "Utopia/Renderer/RenderAPI.hpp"

namespace Utopia
{
	class OpenGLRenderAPI : public RenderAPI
	{
	public:
		void setClearColor(const glm::vec4& color) override;
		void clear() override;
		virtual void setViewPort(int x, int y, int w, int h) override;
		virtual void setWireFrame(bool state) override;

		void drawIndexed(const std::unique_ptr<VertexArray>& vertexArray) override;
	};
}

