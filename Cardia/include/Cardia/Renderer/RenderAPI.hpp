#pragma once

#include "VertexArray.hpp"

#include <glm/glm.hpp>


namespace Cardia
{
	class RenderAPI
	{
	public:
		virtual ~RenderAPI() = default;

		enum class API
		{
			None = 0, OpenGL = 1 //, Vulkan = 2, Direct3D = 3
		};

		static void init();

		virtual void setClearColor(const glm::vec4& color) = 0;
		virtual void clear() = 0;
		virtual void setViewPort(int x, int y, int w, int h) = 0;
		virtual void setWireFrame(bool state) = 0;
		virtual std::string getVendor() = 0;
		virtual std::string getRenderer() = 0;
		virtual std::string getVersion() = 0;

		virtual void drawIndexed(const VertexArray* vertexArray, uint32_t indexCount = 0) = 0;

		static API getAPI() { return s_API; }
		static RenderAPI& get() { cdCoreAssert(s_Instance.get(), "RenderAPI not initialized."); return *s_Instance; }

	private:
		static inline std::unique_ptr<RenderAPI> s_Instance = nullptr;
		static API s_API;
	};
}