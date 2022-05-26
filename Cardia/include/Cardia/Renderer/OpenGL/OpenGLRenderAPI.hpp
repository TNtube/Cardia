#pragma once

#include "Cardia/Renderer/RenderAPI.hpp"


namespace Cardia
{
	class OpenGLRenderAPI : public RenderAPI
	{
	public:
		void setClearColor(const glm::vec4& color) override;
		void clear() override;
		void setViewPort(int x, int y, int w, int h) override;
		void setWireFrame(bool state) override;
		void clearDepthBuffer() override;
		std::string getVendor() override;
		std::string getRenderer() override;
		std::string getVersion() override;

		void drawIndexed(const VertexArray* vertexArray, uint32_t indexCount) override;
	};
}

