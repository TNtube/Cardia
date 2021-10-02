#pragma once

#include <glm/glm.hpp>
#include "VertexArray.hpp"

namespace Cardia
{
	class RenderAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1 //, Vulkan = 2, Direct3D = 3
		};

		virtual void setClearColor(const glm::vec4& color) = 0;
		virtual void clear() = 0;
		virtual void setViewPort(int x, int y, int w, int h) = 0;
		virtual void setWireFrame(bool state) = 0;

		virtual void drawIndexed(const VertexArray* vertexArray) = 0;

		inline static API getAPI() { return s_API; }

	private:
		static API s_API;
	};
}