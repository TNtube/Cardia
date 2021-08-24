#pragma once

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
		static RendererAPI getAPI();
	private:
		static RendererAPI s_RenderAPI;
	};
}