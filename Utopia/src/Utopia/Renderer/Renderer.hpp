#pragma once
#include "RenderCommand.hpp"

namespace Utopia
{
	enum class RendererAPI
	{
		None = 0,
		OpenGL = 1 //, Vulkan = 2, Direct3D = 3
	};

	class Renderer
	{
	public:
		Renderer();
		static RendererAPI getAPI();
		RenderCommand* renderCommand;
	private:
		static RendererAPI s_RenderAPI;

	};
}