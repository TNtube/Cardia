#include "cdpch.hpp"
#include "Cardia/Renderer/Batch.hpp"

#include <numeric>

#include "Cardia/Renderer/RenderAPI.hpp"


namespace Cardia
{
        Batch::Batch(VertexArray* va, const glm::vec3& cameraPosition, const BatchSpecification& specification) :
                specification(specification), camPos(cameraPosition)
        {
                vertexArray = va;

                vertexBuffer = &va->getVertexBuffer();
                indexBuffer = &va->getIndexBuffer();
                indexOffset = 0;

                std::array<int, maxTextureSlots> samplers {};
                for (int32_t i = 0; i < maxTextureSlots; ++i)
                {
                        samplers[i] = i;
                }

                m_Shader = ShaderManager::get(specification.shader);

                if (!m_Shader)
                {
                        // TODO: Temporary, should change when Materials get implemented
                        const auto shaderPath = "resources/shaders/" + specification.shader;
                        m_Shader = ShaderManager::load(specification.shader, {shaderPath + ".vert", shaderPath + ".frag"});
                }

                m_Shader->bind();
                m_Shader->setIntArray("u_Textures", samplers.data(), maxTextureSlots);

                uint32_t whiteColor = 0xffffffff;
                whiteTexture = Texture2D::create(1, 1, &whiteColor);

                // Always white tex at pos 0
                textureSlots[0] = whiteTexture.get();
                startBash();
        }

        void Batch::startBash()
        {
                vertexCount = 0;
                indexOffset = 0;
                vertexBufferData.clear();
                indexBufferData.clear();
                textureSlotIndex = 1;
        }

        void Batch::render(bool alpha)
        {
                if (alpha)
                {
                        std::ranges::sort(indexBufferData, [this](const std::vector<uint32_t>& a, const std::vector<uint32_t>& b)
                        {
                               const auto lambda = [this](const glm::vec3 va, const uint32_t ib)
                               {
                                      return va + vertexBufferData[ib].position;
                               };
                               const auto vertexA = std::accumulate(a.begin(), a.end(), glm::vec3(0), lambda) / 3.0f;
                               const auto vertexB = std::accumulate(b.begin(), b.end(), glm::vec3(0), lambda) / 3.0f;
                               return glm::distance(vertexA, camPos) >= glm::distance(vertexB, camPos);
                        });   
                }
                
                std::vector<uint32_t> iboData;

                for (const auto& object: indexBufferData)
                {
                        for (const auto index: object)
                        {
                                iboData.push_back(index);
                        }
                }

                vertexBuffer->setData(vertexBufferData.data(), static_cast<int>(vertexBufferData.size()) * sizeof(Vertex));
                indexBuffer->setData(iboData.data(), static_cast<int>(iboData.size()) * sizeof(uint32_t));

                m_Shader->bind();
                for (int i = 0; i < textureSlotIndex; ++i)
                {
                        textureSlots[i]->bind(i);
                }

                RenderAPI::get().drawIndexed(vertexArray, vertexCount);
        }

        bool Batch::addMesh(Mesh& mesh, const Texture2D* texture)
        {
                if (vertexCount >= maxIndices)
                        return false;

                float textureIndex = 0;
                for(int i = 1; i < textureSlotIndex; ++i) {
                        if (texture && *textureSlots[i] == *texture) {
                                textureIndex = static_cast<float>(i);
                                break;
                        }
                }

                if (textureIndex == 0 && texture) {
                        if (textureSlotIndex >= maxTextureSlots)
                                return false;

                        textureSlots[textureSlotIndex] = texture;
                        textureIndex = static_cast<float>(textureSlotIndex);
                        textureSlotIndex++;
                }
                for (auto& vertex : mesh.vertices)
                {
                        vertex.textureIndex = textureIndex;
                }

                vertexBufferData.reserve( vertexBufferData.size() + mesh.vertices.size() );
                vertexBufferData.insert(vertexBufferData.end(), mesh.vertices.begin(), mesh.vertices.end());

                indexBufferData.reserve(indexBufferData.size() + mesh.indices.size());
                for (auto& index: mesh.indices)
                {
                        index += indexOffset;
                }

                indexBufferData.push_back(mesh.indices);

                indexOffset += mesh.vertices.size();
                vertexCount += mesh.indices.size();

                return true;
        }
}
