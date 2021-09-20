#pragma once
#include "RenderCommand.hpp"

namespace Cardia
{

	class Renderer
	{
	public:
		static void beginScene();
		static void endScene();
		static void submit(const std::unique_ptr<VertexArray>& vertexArray);
		inline static RenderAPI::API getAPI() { return RenderAPI::getAPI(); };

	};
}