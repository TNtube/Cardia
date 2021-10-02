#pragma once
#include "RenderAPI.hpp"


namespace Cardia
{
	class RenderCommand
	{
	public:
		inline static void setViewPort(int x, int y, int w, int h) {
			s_RenderAPI->setViewPort(x, y, w, h);
		}
		inline static void setWireFrame(bool state) {
			s_RenderAPI->setWireFrame(state);
		}
		inline static void setClearColor(const glm::vec4& color) {
			s_RenderAPI->setClearColor(color);
		}
		inline static void clear() {
			s_RenderAPI->clear();
		}
		inline static void drawIndexed(const VertexArray* vertexArray)
		{
			s_RenderAPI->drawIndexed(vertexArray);
		}
	private:
		static std::unique_ptr<RenderAPI> s_RenderAPI;

	};
}
