
#pragma once

//= INCLUDES ======================
#include "RHI_PhysicalDevice.h"
#include <memory>
#include "RHI_Descriptor.h"
//=================================

namespace LitchiRuntime
{
	enum class RHI_Device_Resource
	{
		sampler_comparison,
		sampler_regular
	};

	class SP_CLASS RHI_Device
	{
	public:
		static void Initialize();
		static void Tick(const uint64_t frame_count);
		static void Destroy();

		// Queues
		static void QueuePresent(void* swapchain_view, uint32_t* image_index, std::vector<RHI_Semaphore*>& wait_semaphores);
		static void QueueSubmit(const RHI_Queue_Type type, const uint32_t wait_flags, void* cmd_buffer, RHI_Semaphore* wait_semaphore = nullptr, RHI_Semaphore* signal_semaphore = nullptr, RHI_Fence* signal_fence = nullptr);
		static void QueueWait(const RHI_Queue_Type type);
		static void QueueWaitAll();
		static void* QueueGet(const RHI_Queue_Type type);
		static uint32_t QueueGetIndex(const RHI_Queue_Type type);
		static void QueueSetIndex(const RHI_Queue_Type type, const uint32_t index);

		// Descriptors
		static void SetDescriptorSetCapacity(uint32_t descriptor_set_capacity);
		static void AllocateDescriptorSet(void*& resource, RHI_DescriptorSetLayout* descriptor_set_layout, const std::vector<RHI_Descriptor>& descriptors);
		static std::unordered_map<uint64_t, RHI_DescriptorSet>& GetDescriptorSets();
		static void* GetDescriptorSet(const RHI_Device_Resource resource_type);
		static void* GetDescriptorSetLayout(const RHI_Device_Resource resource_type);
		static void SetBindlessSamplers(const std::array<std::shared_ptr<RHI_Sampler>, 7>& samplers);

		// Pipelines
		static void GetOrCreatePipeline(RHI_PipelineState& pso, RHI_Pipeline*& pipeline, RHI_DescriptorSetLayout*& descriptor_set_layout);
		static uint32_t GetPipelineCount();

		// Command pools
		static RHI_CommandPool* CommandPoolAllocate(const char* name, const uint64_t swap_chain_id, const RHI_Queue_Type queue_type);
		static void CommandPoolDestroy(RHI_CommandPool* cmd_pool);
		static const std::vector<std::shared_ptr<RHI_CommandPool>>& GetCommandPools();

		// Deletion queue
		static void DeletionQueueAdd(const RHI_Resource_Type resource_type, void* resource);
		static void DeletionQueueParse();
		static bool DeletionQueueNeedsToParse();

		static RHI_RenderPass* RenderPassCreate();

		static RHI_Framebuffer* FrameBufferCreate(RHI_RenderPass renderPass,uint32_t attachmentCount,const void* attachmentImageViews,uint32_t width,uint32_t height,uint32_t layers);



#if 1 Memory

		/**
		 * \brief 
		 * \param resource Handle
		 * \return return mappedData
		 */
		static void* MemoryGetMappedDataFromBuffer(void* resource);
		/**
		 * \brief
		 * \param resource buffer Handle
		 * \param size buffer size
		 * \param usage buffer create usages ps: index storage uniform buffer
		 * \param memory_property_flags memory flags ps: hostVisible deviceLocal hostEnhance
		 * \param data_initial data buffer
		 * \param name buffer name mean resource Name
		 */
		static void MemoryBufferCreate(void*& resource, const uint64_t size, uint32_t usage, uint32_t memory_property_flags, const void* data_initial, const char* name);
		static void MemoryBufferDestroy(void*& resource);
		static void MemoryTextureCreate(void* vk_image_creat_info, void*& resource, const char* name);
		static void MemoryTextureDestroy(void*& resource);
		static void MemoryMap(void* resource, void*& mapped_data);
		static void MemoryUnmap(void* resource, void*& mapped_data);
		static uint32_t MemoryGetUsageMb();
		static uint32_t MemoryGetBudgetMb();
#endif 
		// Immediate execution command list
		static RHI_CommandList* CmdImmediateBegin(const RHI_Queue_Type queue_type);
		static void CmdImmediateSubmit(RHI_CommandList* cmd_list);

		// Properties (actual silicon properties)
		static float PropertyGetTimestampPeriod() { return m_timestamp_period; }
		static uint64_t PropertyGetMinUniformBufferOffsetAllignment() { return m_min_uniform_buffer_offset_alignment; }
		static uint64_t PropertyGetMinStorageBufferOffsetAllignment() { return m_min_storage_buffer_offset_alignment; }
		static uint32_t PropertyGetMaxTexture1dDimension() { return m_max_texture_1d_dimension; }
		static uint32_t PropertyGetMaxTexture2dDimension() { return m_max_texture_2d_dimension; }
		static uint32_t PropertyGetMaxTexture3dDimension() { return m_max_texture_3d_dimension; }
		static uint32_t PropertyGetMaxTextureCubeDimension() { return m_max_texture_cube_dimension; }
		static uint32_t PropertyGetMaxTextureArrayLayers() { return m_max_texture_array_layers; }
		static uint32_t PropertyGetMaxPushConstantSize() { return m_max_push_constant_size; }

		// Markers
		static void MarkerBegin(RHI_CommandList* cmd_list, const char* name, const Vector4& color);
		static void MarkerEnd(RHI_CommandList* cmd_list);

		// Misc
		static void SetResourceName(void* resource, const RHI_Resource_Type resource_type, const std::string name);
		static bool IsValidResolution(const uint32_t width, const uint32_t height);
		static uint32_t GetEnabledGraphicsStages() { return m_enabled_graphics_shader_stages; }
		static uint32_t GetDescriptorType(const RHI_Descriptor& descriptor);
		static PhysicalDevice* GetPrimaryPhysicalDevice();

	private:
		// Physical device
		static bool PhysicalDeviceDetect();
		static void PhysicalDeviceRegister(const PhysicalDevice& physical_device);
		static void PhysicalDeviceSelectPrimary();
		static void PhysicalDeviceSetPrimary(const uint32_t index);
		static std::vector<PhysicalDevice>& PhysicalDeviceGet();

		// Properties
		static float m_timestamp_period;
		static uint64_t m_min_uniform_buffer_offset_alignment;
		static uint64_t m_min_storage_buffer_offset_alignment;
		static uint32_t m_max_texture_1d_dimension;
		static uint32_t m_max_texture_2d_dimension;
		static uint32_t m_max_texture_3d_dimension;
		static uint32_t m_max_texture_cube_dimension;
		static uint32_t m_max_texture_array_layers;
		static uint32_t m_max_push_constant_size;

		// Misc
		static bool m_wide_lines;
		static uint32_t m_physical_device_index;
		static uint32_t m_enabled_graphics_shader_stages;
	};
}
