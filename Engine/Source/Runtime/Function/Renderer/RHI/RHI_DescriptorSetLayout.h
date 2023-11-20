
#pragma once

//= INCLUDES =================
#include <vector>
#include "RHI_Descriptor.h"
#include "Runtime/Core/Meta/Reflection/object.h"
//============================

// A descriptor set layout is created by individual descriptors.
// The descriptors come from shader reflection and contain no resource pointers.
// The descriptor set layout resource is part of the pipeline creation.
// 
// The descriptors that are a member RHI_DescriptorSetLayout also hold resource pointers.
// These descriptors are used to created a descriptor set.
// The descriptor set is what is actually bound before any draw/dispatch calls.

namespace LitchiRuntime
{
	class SP_CLASS RHI_DescriptorSetLayout : public Object
	{
	public:
		RHI_DescriptorSetLayout() = default;
		RHI_DescriptorSetLayout(const std::vector<RHI_Descriptor>& descriptors, const std::string& name);
		~RHI_DescriptorSetLayout();

		// Set
		void SetConstantBuffer(const uint32_t slot, RHI_ConstantBuffer* constant_buffer);
		void SetStructuredBuffer(const uint32_t slot, RHI_StructuredBuffer* structured_buffer);
		void SetSampler(const uint32_t slot, RHI_Sampler* sampler);
		void SetTexture(const uint32_t slot, RHI_Texture* texture, const uint32_t mip_index, const uint32_t mip_range);
		void SetMaterialGlobalBuffer(RHI_ConstantBuffer* constant_buffer);

		// Dynamic offsets
		void GetDynamicOffsets(std::vector<uint32_t>* offsets);

		// Misc
		void ClearDescriptorData();
		RHI_DescriptorSet* GetDescriptorSet();
		const std::vector<RHI_Descriptor>& GetDescriptors() const { return m_descriptors; }
		void NeedsToBind() { m_needs_to_bind = true; }
		uint64_t GetHash()                                  const { return m_hash; }
		void* GetRhiResource()                              const { return m_rhi_resource; }

	private:
		void CreateRhiResource(std::vector<RHI_Descriptor> descriptors);

		void* m_rhi_resource = nullptr;
		uint64_t m_hash = 0;
		bool m_needs_to_bind = false;
		std::vector<RHI_Descriptor> m_descriptors;
	};
}
