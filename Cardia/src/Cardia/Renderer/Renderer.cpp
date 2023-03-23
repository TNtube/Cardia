#include "cdpch.hpp"
#include "Cardia/Renderer/OpenGL/OpenGLShader.hpp"
#include "Cardia/Renderer/Renderer.hpp"

namespace Cardia
{
	std::unique_ptr<Renderer::SceneData> Renderer::s_SceneData = std::make_unique<Renderer::SceneData>();

	void Renderer::beginScene(Camera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
	}

	void Renderer::endScene()
	{
	}

	void Renderer::submit(const VertexArray* vertexArray, const Shader* shader, const glm::mat4& transform)
	{
		shader->bind();
		const auto& glShader = dynamic_cast<const Cardia::OpenGLShader&>(*shader);
		glShader.setUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		glShader.setUniformMat4("u_Model", transform);

		vertexArray->bind();
		RenderAPI::get().drawIndexed(vertexArray);
	}
}