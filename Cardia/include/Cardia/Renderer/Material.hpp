#pragma once
#include "Pipeline.hpp"


namespace Cardia
{
	class Material
	{
	public:
		Material();

	private:
		std::shared_ptr<DescriptorSetLayout> m_DescriptorSetLayout;
		std::shared_ptr<DescriptorSet> m_DescriptorSets;
		std::unique_ptr<PipelineLayout> m_PipelineLayout;
		std::unique_ptr<Pipeline> m_Pipeline;
	};
}
