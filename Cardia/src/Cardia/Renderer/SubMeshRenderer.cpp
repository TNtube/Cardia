#include <Cardia/Renderer/RenderAPI.hpp>
#include <Cardia/Renderer/Renderer2D.hpp>
#include "cdpch.hpp"

#include "Cardia/Renderer/SubMeshRenderer.hpp"

namespace Cardia
{

	void SubMeshRenderer::SubmitSubMesh(SubMesh &subMesh)
	{
		m_VertexArray = VertexArray::create();

		auto& vertices = subMesh.GetVertices();
		std::unique_ptr<VertexBuffer> vbo = VertexBuffer::create(vertices.data(), vertices.size() * sizeof(Vertex));

		vbo->setLayout({
			   {ShaderDataType::Float3, "a_Position"},
			   {ShaderDataType::Float3, "a_Normal"},
			   {ShaderDataType::Float4, "a_Color"},
			   {ShaderDataType::Float2, "a_TexPos"},
			   {ShaderDataType::Float, "a_TilingFactor"},
			   {ShaderDataType::Float, "a_EntityID"}
		});

		m_VertexArray->setVertexBuffer(std::move(vbo));

		auto& indices = subMesh.GetIndices();
		std::unique_ptr<IndexBuffer> ibo = IndexBuffer::create(indices.data(), indices.size());
		m_VertexArray->setIndexBuffer(std::move(ibo));
	}

	void SubMeshRenderer::Draw()
	{
		m_VertexArray->bind();
		m_VertexArray->getIndexBuffer().bind();
		Renderer2D::getStats().drawCalls++;
		Renderer2D::getStats().triangleCount += m_VertexArray->getIndexBuffer().getCount() / 3;

		RenderAPI::get().drawIndexed(m_VertexArray.get(), m_VertexArray->getIndexBuffer().getCount());
	}
}