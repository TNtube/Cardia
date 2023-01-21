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

		void SubmitMesh(Mesh& mesh);
		void Draw();
	private:
		std::vector<SubMeshRenderer> m_SubMeshRenderers {};
	};
}