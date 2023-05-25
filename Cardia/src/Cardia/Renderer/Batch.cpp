#include "cdpch.hpp"
#include "Cardia/Renderer/Batch.hpp"

#include <numeric>
#include <Cardia/Project/AssetsManager.hpp>


namespace Cardia
{
	Batch::Batch(const glm::vec3& cameraPosition, const Texture2D* texture, const BatchSpecification& specification) :
		specification(specification), camPos(cameraPosition), m_CurrentTexture(texture)
	{

		// vertexBuffer = &va->getVertexBuffer();
		// indexBuffer = &va->getIndexBuffer();
		indexOffset = 0;

		const auto shaderPath = "resources/shaders/" + specification.shader;
		m_Shader = AssetsManager::Load<Shader>(shaderPath);

		uint32_t whiteColor = 0xffffffff;
		whiteTexture = Texture2D::create(1, 1, &whiteColor);
		startBash();
	}

	void Batch::startBash()
	{
		indexCount = 0;
		indexOffset = 0;
		// vertexBufferData.clear();
		indexBufferData.clear();
	}

	void Batch::render(bool alpha)
	{
		if (alpha)
		{
			std::ranges::sort(indexBufferData, [this](const std::vector<uint32_t>& a, const std::vector<uint32_t>& b)
			{
				const auto lambda = [this](const glm::vec3 va, const uint32_t ib)
				{
					return va /*+ vertexBufferData[ib].position*/;
				};
				const auto vertexA = std::accumulate(a.begin(), a.end(), glm::vec3(0), lambda) / 3.0f;
				const auto vertexB = std::accumulate(b.begin(), b.end(), glm::vec3(0), lambda) / 3.0f;
				return glm::distance(vertexA, camPos) >= glm::distance(vertexB, camPos);
			});
		}

		std::vector<uint32_t> iboData;

		for (const auto& object: indexBufferData)
		{
			iboData.reserve(object.size());
			iboData.insert(iboData.end(), object.begin(), object.end());
		}
		// vertexArray->bind();

		// vertexBuffer->setData(vertexBufferData.data(), static_cast<int>(vertexBufferData.size()) * sizeof(Vertex));
		// indexBuffer->setData(iboData.data(), static_cast<int>(iboData.size()) * sizeof(uint32_t));

		m_Shader->bind();
		m_Shader->setInt("u_Texture", 0);
		m_Shader->setMat4("u_Model", glm::mat4(1));
		m_Shader->setMat3("u_TransposedInvertedModel", glm::transpose(glm::mat4(1)));
		// if (m_CurrentTexture)
		// 	m_CurrentTexture->Bind(0);
		// else
		// 	whiteTexture->Bind(0);

		// RenderAPI::get().drawIndexed(vertexArray, indexCount);
	}

	bool Batch::addMesh(SubMesh* mesh)
	{
		if (indexCount >= maxIndices)
			return false;

		// vertexBufferData.reserve( vertexBufferData.size() + mesh->GetVertices().size() );
		// vertexBufferData.insert(vertexBufferData.end(), mesh->GetVertices().begin(), mesh->GetVertices().end());

		std::vector<uint32_t>& indices = indexBufferData.emplace_back(mesh->GetIndices().begin(), mesh->GetIndices().end());
		for (auto& index: indices)
		{
			index += indexOffset;
		}

		indexOffset += mesh->GetVertices().size();
		indexCount += indices.size();

		return true;
	}
}
