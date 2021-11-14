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
		inline static void drawIndexed(const VertexArray* vertexArray, uint32_t indexCount = 0)
		{
			s_RenderAPI->drawIndexed(vertexArray, indexCount);
		}
		inline static std::string getVendor() {
			return s_RenderAPI->getVendor();
		}
		inline static std::string getRenderer() {
			return s_RenderAPI->getRenderer();
		}
		inline static std::string getVersion() {
			return s_RenderAPI->getVersion();
		}
	private:
		static std::unique_ptr<RenderAPI> s_RenderAPI;

	};
}
