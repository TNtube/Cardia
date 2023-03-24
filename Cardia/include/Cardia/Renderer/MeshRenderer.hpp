#pragma once

#include <vector>
#include "Cardia/DataStructure/Mesh.hpp"
#include "SubMeshRenderer.hpp"


namespace Cardia
{
	class MeshRenderer
	{
	public:
		MeshRenderer() = default;

		void SubmitMesh(std::shared_ptr<Mesh> mesh);
		std::shared_ptr<Mesh> GetMesh() { return m_Mesh; }
		void Draw();
	private:
		std::vector<SubMeshRenderer> m_SubMeshRenderers {};
		std::shared_ptr<Mesh> m_Mesh;
	};
}