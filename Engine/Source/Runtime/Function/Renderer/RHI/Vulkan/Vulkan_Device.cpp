
//= INCLUDES ==========================
#include "Runtime/Core/pch.h"
#include "../RHI_Device.h"
#include "../RHI_Implementation.h"
#include "../RHI_Semaphore.h"
#include "../RHI_CommandPool.h"
#include "../RHI_DescriptorSet.h"
#include "../RHI_Sampler.h"
#include "../RHI_Fence.h"
#include "../RHI_Shader.h"
#include "../RHI_DescriptorSetLayout.h"
#include "../RHI_Pipeline.h"
#include "../RHI_Texture.h"
#include "GLFW/glfw3.h"
#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Renderer/Rendering/Renderer.h"
//#include "../../Profiling/Profiler.h"
LC_WARNINGS_OFF
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
LC_WARNINGS_ON
//=====================================

//= NAMESPACES ===============
using namespace std;
using namespace LitchiRuntime::Math;
//============================

namespace LitchiRuntime
{
	namespace version
	{
		uint32_t used = 0;

		string to_string()
		{
			return std::to_string(VK_VERSION_MAJOR(used)) + "." + std::to_string(VK_VERSION_MINOR(used)) + "." + std::to_string(VK_VERSION_PATCH(used));
		}
	}

	namespace
	{
		static mutex mutex_allocation;
		static mutex mutex_deletion_queue;
		static unordered_map<RHI_Resource_Type, vector<void*>> deletion_queue;

		static bool is_present_instance_layer(const char* layer_name)
		{
			uint32_t layer_count;
			vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

			vector<VkLayerProperties> layers(layer_count);
			vkEnumerateInstanceLayerProperties(&layer_count, layers.data());

			for (const auto& layer : layers)
			{
				if (strcmp(layer_name, layer.layerName) == 0)
					return true;
			}

			return false;
		}

		static bool is_present_device_extension(const char* extension_name, VkPhysicalDevice device_physical)
		{
			uint32_t extension_count = 0;
			vkEnumerateDeviceExtensionProperties(device_physical, nullptr, &extension_count, nullptr);

			vector<VkExtensionProperties> extensions(extension_count);
			vkEnumerateDeviceExtensionProperties(device_physical, nullptr, &extension_count, extensions.data());

			for (const auto& extension : extensions)
			{
				if (strcmp(extension_name, extension.extensionName) == 0)
					return true;
			}

			return false;
		}

		static bool is_present_instance(const char* extension_name)
		{
			uint32_t extension_count = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

			vector<VkExtensionProperties> extensions(extension_count);
			vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

			for (const auto& extension : extensions)
			{
				if (strcmp(extension_name, extension.extensionName) == 0)
					return true;
			}

			return false;
		}

		static vector<const char*> get_physical_device_supported_extensions(const vector<const char*>& extensions, VkPhysicalDevice device_physical)
		{
			vector<const char*> extensions_supported;

			for (const auto& extension : extensions)
			{
				if (is_present_device_extension(extension, device_physical))
				{
					extensions_supported.emplace_back(extension);
				}
				else
				{
					DEBUG_LOG_ERROR("Device extension {} is not supported", extension);
				}
			}

			return extensions_supported;
		}

		static vector<const char*> get_supported_extensions(const vector<const char*>& extensions)
		{
			vector<const char*> extensions_supported;

			for (const auto& extension : extensions)
			{
				if (is_present_instance(extension))
				{
					extensions_supported.emplace_back(extension);
				}
				else
				{
					DEBUG_LOG_ERROR("Instance extension {} is not supported", extension);
				}
			}

			return extensions_supported;
		}

		VkImageTiling get_format_tiling(const RHI_Texture* texture)
		{
			VkFormatProperties2 format_properties2 = {};
			format_properties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

			VkFormatProperties3 format_properties3 = {};
			format_properties3.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3;
			format_properties2.pNext = &format_properties3;

			VkFormatFeatureFlagBits2 format_flags = 0;
			format_flags |= texture->IsSrv() ? VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT : 0;
			format_flags |= texture->IsUav() ? VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT : 0;
			format_flags |= texture->IsRtv() ? VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT : 0;
			format_flags |= texture->IsDsv() ? VK_FORMAT_FEATURE_2_DEPTH_STENCIL_ATTACHMENT_BIT : 0;
			format_flags |= texture->IsVrs() ? VK_FORMAT_FEATURE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR : 0;

			vkGetPhysicalDeviceFormatProperties2(RHI_Context::device_physical, vulkan_format[rhi_format_to_index(texture->GetFormat())], &format_properties2);

			// check for optimal support
			VkImageTiling tiling = VK_IMAGE_TILING_MAX_ENUM;
			if (format_properties3.optimalTilingFeatures & format_flags)
			{
				tiling = VK_IMAGE_TILING_OPTIMAL;
			}
			// check for linear support
			else if (format_properties3.linearTilingFeatures & format_flags)
			{
				tiling = VK_IMAGE_TILING_LINEAR;
			}

			LC_ASSERT_MSG(tiling != VK_IMAGE_TILING_MAX_ENUM, "The GPU doesn't support this format");
			LC_ASSERT_MSG(tiling == VK_IMAGE_TILING_OPTIMAL, "This format doesn't support optimal tiling, switch to a more efficient format");

			return tiling;
		}

		VkImageUsageFlags get_image_usage_flags(const RHI_Texture* texture)
		{
			VkImageUsageFlags flags = 0;

			flags |= texture->IsSrv() ? VK_IMAGE_USAGE_SAMPLED_BIT : 0;
			flags |= texture->IsUav() ? VK_IMAGE_USAGE_STORAGE_BIT : 0;
			flags |= texture->IsVrs() ? VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV : 0;
			flags |= texture->IsDsv() ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : 0;
			flags |= texture->IsRtv() ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : 0;

			// If the texture has data, it will be staged, so it needs transfer bits.
			// If the texture participates in clear or blit operations, it needs transfer bits.
			if (texture->HasData() || (texture->GetFlags() & RHI_Texture_ClearBlit) != 0)
			{
				flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT; // source of a transfer command.
				flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT; // destination of a transfer command
			}

			return flags;
		}

		VkApplicationInfo create_application_info()
		{
			VkApplicationInfo app_info = {};
			app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			app_info.pApplicationName = Litchi_info::name;             // for gpu vendors to do game specific driver optimizations
			app_info.pEngineName = app_info.pApplicationName; // for gpu vendors to do engine specific driver optimizations
			app_info.engineVersion = VK_MAKE_VERSION(Litchi_info::version_major, Litchi_info::version_minor, Litchi_info::version_revision);
			app_info.applicationVersion = app_info.engineVersion;

			// deduce api version to use based on the SDK and what the driver supports
			{
				uint32_t driver_version = 0;
				{
					// per LunarG, if vkEnumerateInstanceVersion is not present, we are running on Vulkan 1.0
					// https://www.lunarg.com/wp-content/uploads/2019/02/Vulkan-1.1-Compatibility-Statement_01_19.pdf
					auto eiv = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"));

					if (eiv)
					{
						eiv(&driver_version);
					}
					else
					{
						driver_version = VK_API_VERSION_1_0;
					}
				}

				// choose the version which is supported by both the sdk and the driver
				uint32_t sdk_version = VK_HEADER_VERSION_COMPLETE;
				app_info.apiVersion = Helper::Min(sdk_version, driver_version);

				// 1.3 the minimum required version as we are using extensions from 1.3
				LC_ASSERT_MSG(app_info.apiVersion >= VK_API_VERSION_1_3, "Vulkan 1.3 is not supported");

				// in case the SDK is not supported by the driver, prompt the user to update
				if (sdk_version > driver_version)
				{
					// detect and log version
					string driver_version_str = to_string(VK_API_VERSION_MAJOR(driver_version)) + "." + to_string(VK_API_VERSION_MINOR(driver_version)) + "." + to_string(VK_API_VERSION_PATCH(driver_version));
					string sdk_version_str = to_string(VK_API_VERSION_MAJOR(sdk_version)) + "." + to_string(VK_API_VERSION_MINOR(sdk_version)) + "." + to_string(VK_API_VERSION_PATCH(sdk_version));
					DEBUG_LOG_WARN("Using Vulkan {}, update drivers or wait for GPU vendor to support Vulkan {}, engine may still work", driver_version_str.c_str(), sdk_version_str.c_str());
				}

				// save the api version we ended up using
				version::used = app_info.apiVersion;
				RHI_Context::api_version_str = version::to_string();
			}

			return app_info;
		}
	}

	namespace command_pools
	{
		vector<shared_ptr<RHI_CommandPool>> regular;
		array<shared_ptr<RHI_CommandPool>, 3> immediate;
		mutex mutex_immediate_execution;
		condition_variable condition_variable_immediate_execution;
		bool is_immediate_executing = false;
	}

	namespace queues
	{
		mutex mutex_queue;
		void* graphics = nullptr;
		void* compute = nullptr;
		void* copy = nullptr;

		static uint32_t index_graphics = 0;
		static uint32_t index_compute = 0;
		static uint32_t index_copy = 0;
	}

	namespace functions
	{
		PFN_vkCreateDebugUtilsMessengerEXT  create_messenger = nullptr;
		PFN_vkDestroyDebugUtilsMessengerEXT destroy_messenger = nullptr;
		PFN_vkSetDebugUtilsObjectTagEXT     set_object_tag = nullptr;
		PFN_vkSetDebugUtilsObjectNameEXT    set_object_name = nullptr;
		PFN_vkCmdBeginDebugUtilsLabelEXT    marker_begin = nullptr;
		PFN_vkCmdEndDebugUtilsLabelEXT      marker_end = nullptr;
		PFN_vkCmdSetFragmentShadingRateKHR  set_fragment_shading_rate = nullptr;

		static void initialize(bool validation_enabled, bool gpu_markers_enabled)
		{
#define get_func(var, def)\
            var = reinterpret_cast<PFN_##def>(vkGetInstanceProcAddr(static_cast<VkInstance>(RHI_Context::instance), #def));\
            if (!var) DEBUG_LOG_ERROR("Failed to get function pointer for %s", #def);\
 /* VK_EXT_debug_utils */
			{
				if (validation_enabled)
				{
					get_func(create_messenger, vkCreateDebugUtilsMessengerEXT);
					get_func(destroy_messenger, vkDestroyDebugUtilsMessengerEXT);

					LC_ASSERT(create_messenger && destroy_messenger);
				}

				if (gpu_markers_enabled)
				{
					get_func(marker_begin, vkCmdBeginDebugUtilsLabelEXT);
					get_func(marker_end, vkCmdEndDebugUtilsLabelEXT);

					LC_ASSERT(marker_begin && marker_end);
				}
			}

			/* VK_EXT_debug_marker */
			if (validation_enabled)
			{
				get_func(set_object_tag, vkSetDebugUtilsObjectTagEXT);
				get_func(set_object_name, vkSetDebugUtilsObjectNameEXT);

				LC_ASSERT(set_object_tag && set_object_name);
			}

			get_func(set_fragment_shading_rate, vkCmdSetFragmentShadingRateKHR);
			LC_ASSERT(set_fragment_shading_rate);
		}
	}

	namespace validation_layer_logging
	{
		static VkDebugUtilsMessengerEXT messenger;

		static VKAPI_ATTR VkBool32 VKAPI_CALL callback
		(
			VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
			VkDebugUtilsMessageTypeFlagsEXT msg_type,
			const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
			void* p_user_data
		)
		{
			// filter out certain things
			{
				// fidelityfx sdk
				if (p_callback_data->messageIdNumber == 0xdc18ad6b)
				{
					// [ UNASSIGNED-BestPractices-vkAllocateMemory-small-allocation ] | MessageID = 0xdc18ad6b | vkAllocateMemory():
					// Allocating a VkDeviceMemory of size 256. This is a very small allocation (current threshold is 262144 bytes).
					// You should make large allocations and sub-allocate from one large VkDeviceMemory.
					return VK_FALSE;
				}

				// occlusion queries
				{
					if (p_callback_data->messageIdNumber == 0xd39be754)
					{
						// Validation Warning :
						// [BestPractices - QueryPool - Unavailable] Object 0 :
						// handle = 0x980b0000000002e, name = query_pool_occlusion, type = VK_OBJECT_TYPE_QUERY_POOL; | MessageID = 0xd39be754 | vkGetQueryPoolResults() :
						// QueryPool VkQueryPool 0x980b0000000002e[query_pool_occlusion] and query 0 : vkCmdBeginQuery() was never called.
						return VK_FALSE;
					}
				}

				if (p_callback_data->messageIdNumber == 0xe17ab4ae) // validation error - SYNC-HAZARD-PRESENT-AFTER-WRITE
					return VK_FALSE;

				if (p_callback_data->messageIdNumber == 0x42f2f4ed) // validation error - depth_ouput
					return VK_FALSE;

				if (p_callback_data->messageIdNumber == 0xe4d96472) // validation error - depth_ouput
					return VK_FALSE;

				if (p_callback_data->messageIdNumber == 0x5c0ec5d6) // validation error - depth_light
					return VK_FALSE;
			}

			string msg = "Vulkan: " + string(p_callback_data->pMessage);

			if (/*(msg_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) ||*/ (msg_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT))
			{
				DEBUG_LOG_INFO(msg);
			}
			else if (msg_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			{
				DEBUG_LOG_WARN(msg);
			}
			else if (msg_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			{
				DEBUG_LOG_ERROR(msg);
			}

			return VK_FALSE;
		}

		static void initialize(VkInstance instance)
		{
			if (functions::create_messenger)
			{
				VkDebugUtilsMessengerCreateInfoEXT create_info = {};
				create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				create_info.pfnUserCallback = callback;

				functions::create_messenger(instance, &create_info, nullptr, &messenger);
			}
		}

		static void shutdown(VkInstance instance)
		{
			if (!functions::destroy_messenger)
				return;

			functions::destroy_messenger(instance, messenger, nullptr);
		}
	}

	namespace vulkan_memory_allocator
	{
		static mutex mutex_allocator;
		static VmaAllocator allocator;
		static unordered_map<uint64_t, VmaAllocation> allocations;

		static void initialize(const uint32_t api_version)
		{
			// https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/staying_within_budget.html
			// It is recommended to use VK_EXT_memory_budget device extension to obtain information about the budget from Vulkan device.
			// VMA is able to use this extension automatically. When not enabled, the allocator behaves same way, but then it estimates
			// current usage and available budget based on its internal information and Vulkan memory heap sizes, which may be less precise.

			VmaAllocatorCreateInfo allocator_info = {};
			allocator_info.physicalDevice = RHI_Context::device_physical;
			allocator_info.device = RHI_Context::device;
			allocator_info.instance = RHI_Context::instance;
			allocator_info.vulkanApiVersion = api_version;
			allocator_info.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;

			LC_ASSERT_MSG(vmaCreateAllocator(&allocator_info, &vulkan_memory_allocator::allocator) == VK_SUCCESS, "Failed to create memory allocator");
		}

		static void destroy()
		{
			LC_ASSERT(vulkan_memory_allocator::allocator != nullptr);
			LC_ASSERT_MSG(vulkan_memory_allocator::allocations.empty(), "There are still allocations");
			vmaDestroyAllocator(static_cast<VmaAllocator>(vulkan_memory_allocator::allocator));
			vulkan_memory_allocator::allocator = nullptr;
		}

		static uint64_t resource_to_id(void* resource)
		{
			return reinterpret_cast<uint64_t>(resource);
		}

		static void save_allocation(void*& resource, const char* name, VmaAllocation allocation)
		{
			LC_ASSERT_MSG(resource != nullptr, "Resource can't be null");
			LC_ASSERT_MSG(name != nullptr, "Name can't be empty");

			// Set allocation data
			vmaSetAllocationUserData(allocator, allocation, resource);
			vmaSetAllocationName(allocator, allocation, name);

			// Name the allocation's underlying VkDeviceMemory
			RHI_Device::SetResourceName(allocation->GetMemory(), RHI_Resource_Type::DeviceMemory, name);

			lock_guard<mutex> lock(mutex_allocation);
			allocations[resource_to_id(resource)] = allocation;
		}

		static void destroy_allocation(void*& resource)
		{
			lock_guard<mutex> lock(mutex_allocation);

			auto it = allocations.find(resource_to_id(resource));
			if (it != allocations.end())
			{
				allocations.erase(it);
			}

			resource = nullptr;
		}

		static VmaAllocation get_allocation_from_resource(void* resource)
		{
			lock_guard<mutex> lock(mutex_allocation);

			auto it = allocations.find(resource_to_id(resource));
			if (it != allocations.end())
			{
				return it->second;
			}

			return nullptr;
		}
	}

	namespace descriptors
	{
		mutex descriptor_pipeline_mutex;
		uint32_t allocated_descriptor_sets = 0;
		VkDescriptorPool descriptor_pool = nullptr;

		// cache
		unordered_map<uint64_t, RHI_DescriptorSet> sets;
		unordered_map<uint64_t, shared_ptr<RHI_DescriptorSetLayout>> layouts;
		unordered_map<uint64_t, shared_ptr<RHI_Pipeline>> pipelines;
		unordered_map<uint64_t, vector<RHI_Descriptor>> descriptor_cache;

		void merge_descriptors(vector<RHI_Descriptor>& base_descriptors, const std::vector<RHI_Descriptor>& additional_descriptors)
		{
			for (const RHI_Descriptor& descriptor_additional : additional_descriptors)
			{
				bool updated_existing = false;
				for (RHI_Descriptor& descriptor_base : base_descriptors)
				{
					if (descriptor_base.slot == descriptor_additional.slot)
					{
						descriptor_base.stage |= descriptor_additional.stage;
						updated_existing = true;
						break;
					}
				}

				// if no updating took place, this is an additional shader only resource, add it
				if (!updated_existing)
				{
					base_descriptors.emplace_back(descriptor_additional);
				}
			}
		}

		void get_descriptors_from_pipeline_state(RHI_PipelineState& pipeline_state, vector<RHI_Descriptor>& descriptors)
		{
			pipeline_state.Prepare();

			// use the hash of the pipeline state as the key for the cache
			uint64_t pipeline_state_hash = pipeline_state.GetHash();

			// check if descriptors for this pipeline state are already cached
			auto cached_descriptors = descriptor_cache.find(pipeline_state_hash);
			if (cached_descriptors != descriptor_cache.end())
			{
				// fetch from cache
				descriptors = cached_descriptors->second;
				return;
			}

			// if not cached, generate descriptors
			descriptors.clear();

			if (pipeline_state.IsCompute())
			{
				LC_ASSERT(pipeline_state.shader_compute->GetCompilationState() == RHI_ShaderCompilationState::Succeeded);
				descriptors = pipeline_state.shader_compute->GetDescriptors();
			}
			else if (pipeline_state.IsGraphics())
			{
				if (pipeline_state.material_shader) // fast descriptor path, avoid merge
				{
					descriptors = pipeline_state.material_shader->GetMaterialDescriptors();
				}else
				{
					LC_ASSERT(pipeline_state.shader_vertex->GetCompilationState() == RHI_ShaderCompilationState::Succeeded);
					descriptors = pipeline_state.shader_vertex->GetDescriptors();

					if (pipeline_state.shader_pixel)
					{
						LC_ASSERT(pipeline_state.shader_pixel->GetCompilationState() == RHI_ShaderCompilationState::Succeeded);
						merge_descriptors(descriptors, pipeline_state.shader_pixel->GetDescriptors());
					}

					if (pipeline_state.shader_hull)
					{
						LC_ASSERT(pipeline_state.shader_hull->GetCompilationState() == RHI_ShaderCompilationState::Succeeded);
						merge_descriptors(descriptors, pipeline_state.shader_hull->GetDescriptors());
					}

					if (pipeline_state.shader_domain)
					{
						LC_ASSERT(pipeline_state.shader_domain->GetCompilationState() == RHI_ShaderCompilationState::Succeeded);
						merge_descriptors(descriptors, pipeline_state.shader_domain->GetDescriptors());
					}
				}
			}

			if(!pipeline_state.material_shader)
			{
				// sort descriptors by slot
				// this makes things easier to work with, for example dynamic offsets
				// are expected as a list which should be ordered by a slot
				sort(descriptors.begin(), descriptors.end(), [](const RHI_Descriptor& a, const RHI_Descriptor& b)
					{
						return a.slot < b.slot;
					});
			}

			// cache the newly created descriptors
			descriptor_cache[pipeline_state_hash] = descriptors;
		}

		shared_ptr<RHI_DescriptorSetLayout> get_or_create_descriptor_set_layout(RHI_PipelineState& pipeline_state)
		{
			// get descriptors from pipeline state
			vector<RHI_Descriptor> descriptors;
			get_descriptors_from_pipeline_state(pipeline_state, descriptors);

			// compute a hash for the descriptors
			uint64_t hash = 0;
			for (RHI_Descriptor& descriptor : descriptors)
			{
				hash = rhi_hash_combine(hash, static_cast<uint64_t>(descriptor.slot));
				hash = rhi_hash_combine(hash, static_cast<uint64_t>(descriptor.stage));
			}

			// search for a descriptor set layout which matches this hash
			auto it = layouts.find(hash);
			bool cached = it != layouts.end();

			// if there is no descriptor set layout for this particular hash, create one
			if (!cached)
			{
				// emplace a new descriptor set layout
				it = layouts.emplace(make_pair(hash, make_shared<RHI_DescriptorSetLayout>(descriptors, pipeline_state.name))).first;
			}
			shared_ptr<RHI_DescriptorSetLayout> descriptor_set_layout = it->second;

			if (cached)
			{
				descriptor_set_layout->ClearDescriptorData();
			}

			return descriptor_set_layout;
		}

		namespace bindless
		{
			array<VkDescriptorSet, 2> sets;
			array<VkDescriptorSetLayout, 2> layouts;
			//array<VkDescriptorSet, 3> sets;
			//array<VkDescriptorSetLayout, 3> layouts;

			void create_layout(const RHI_Device_Resource resource_type, const uint32_t binding, const uint32_t resource_count, const string& debug_name)
			{
				VkDescriptorSetLayoutBinding layout_binding = {};
				layout_binding.binding = binding;
				//layout_binding.descriptorType = resource_type == RHI_Device_Resource::textures_material ? VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE : VK_DESCRIPTOR_TYPE_SAMPLER;
				layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
				layout_binding.descriptorCount = rhi_max_array_size;
				layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;
				layout_binding.pImmutableSamplers = nullptr;

				VkDescriptorBindingFlags binding_flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;

				VkDescriptorSetLayoutBindingFlagsCreateInfo layout_binding_flags = {};
				layout_binding_flags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
				layout_binding_flags.bindingCount = 1;
				layout_binding_flags.pBindingFlags = &binding_flags;

				VkDescriptorSetLayoutCreateInfo layout_info = {};
				layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layout_info.bindingCount = 1;
				layout_info.pBindings = &layout_binding;
				layout_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
				layout_info.pNext = &layout_binding_flags;

				VkDescriptorSetLayout* layout = &layouts[static_cast<uint32_t>(resource_type)];
				LC_VK_ASSERT_MSG(vkCreateDescriptorSetLayout(RHI_Context::device, &layout_info, nullptr, layout), "Failed to create descriptor set layout");
				RHI_Device::SetResourceName(static_cast<void*>(*layout), RHI_Resource_Type::DescriptorSetLayout, debug_name);
			}

			void create_set(const RHI_Device_Resource resource_type, const uint32_t resource_count, const string& debug_name)
			{
				// allocate descriptor set with actual descriptor count
				VkDescriptorSetVariableDescriptorCountAllocateInfoEXT real_descriptor_count_info = {};
				real_descriptor_count_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT;
				real_descriptor_count_info.descriptorSetCount = 1;               // one descriptor set
				real_descriptor_count_info.pDescriptorCounts = &resource_count; // actual number of textures being used

				// allocate descriptor set
				VkDescriptorSetAllocateInfo allocation_info = {};
				allocation_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				allocation_info.descriptorPool = descriptors::descriptor_pool;
				allocation_info.descriptorSetCount = 1;
				allocation_info.pSetLayouts = &layouts[static_cast<uint32_t>(resource_type)];
				allocation_info.pNext = &real_descriptor_count_info;

				// create
				VkDescriptorSet* descriptor_set = &sets[static_cast<uint32_t>(resource_type)];
				LC_VK_ASSERT_MSG(vkAllocateDescriptorSets(RHI_Context::device, &allocation_info, descriptor_set), "Failed to allocate descriptor set");
				RHI_Device::SetResourceName(static_cast<void*>(*descriptor_set), RHI_Resource_Type::DescriptorSet, debug_name);
			}

			void update_samplers(const vector<shared_ptr<RHI_Sampler>>& samplers, const uint32_t binding_slot, const RHI_Device_Resource resource_type)
			{
				uint32_t sampler_count = static_cast<uint32_t>(samplers.size());
				uint32_t binding = rhi_shader_shift_register_s + binding_slot;

				// create layout and set (if needed)
				if (layouts[static_cast<uint32_t>(resource_type)] == nullptr)
				{
					string debug_name = resource_type == RHI_Device_Resource::sampler_comparison ? "samplers_comparison" : "samplers_regular";

					create_layout(resource_type, binding, sampler_count, debug_name);
					create_set(resource_type, sampler_count, debug_name);
				}

				// update
				{
					vector<VkDescriptorImageInfo> image_infos(sampler_count);
					for (uint32_t i = 0; i < sampler_count; i++)
					{
						image_infos[i].sampler = static_cast<VkSampler>(samplers[i]->GetRhiResource());
						image_infos[i].imageView = nullptr;
						image_infos[i].imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					}

					VkWriteDescriptorSet descriptor_write = {};
					descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptor_write.dstSet = sets[static_cast<uint32_t>(resource_type)];
					descriptor_write.dstBinding = binding;
					descriptor_write.dstArrayElement = 0; // starting element in that array
					descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
					descriptor_write.descriptorCount = sampler_count;
					descriptor_write.pImageInfo = image_infos.data();

					vkUpdateDescriptorSets(RHI_Context::device, 1, &descriptor_write, 0, nullptr);
				}
			}

			void update_textures(const array<RHI_Texture*, rhi_max_array_size>* textures, const uint32_t binding_slot)
			{
				uint32_t texture_count = static_cast<uint32_t>(textures->size());
				uint32_t binding = rhi_shader_shift_register_t + binding_slot;

				//// create layout and set (if needed)
				//if (layouts[static_cast<uint32_t>(RHI_Device_Resource::textures_material)] == nullptr)
				//{
				//	string debug_name = "textures_material";

				//	create_layout(RHI_Device_Resource::textures_material, binding, texture_count, debug_name);
				//	create_set(RHI_Device_Resource::textures_material, texture_count, debug_name);
				//}

				//// update
				//{
				//	vector<VkDescriptorImageInfo> image_infos(texture_count);
				//	for (uint32_t i = 0; i < texture_count; ++i)
				//	{
				//		RHI_Texture* texture = (*textures)[i];
				//		if (!texture)
				//			continue;

				//		// get texture, if unable to do so, fallback to a checkerboard texture, so we can spot it by eye
				//		void* srv_default = Renderer::GetStandardTexture(Renderer_StandardTexture::Checkerboard)->GetRhiSrv();
				//		void* resource = texture ? texture->GetRhiSrv() : srv_default;

				//		image_infos[i].sampler = nullptr;
				//		image_infos[i].imageView = static_cast<VkImageView>(resource);
				//		image_infos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				//	}

				//	VkWriteDescriptorSet descriptor_write = {};
				//	descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				//	descriptor_write.dstSet = sets[static_cast<uint32_t>(RHI_Device_Resource::textures_material)];
				//	descriptor_write.dstBinding = binding;
				//	descriptor_write.dstArrayElement = 0; // starting element in the array
				//	descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				//	descriptor_write.descriptorCount = texture_count;
				//	descriptor_write.pImageInfo = image_infos.data();

				//	vkUpdateDescriptorSets(RHI_Context::device, 1, &descriptor_write, 0, nullptr);
				//}
			}
		}

		void release()
		{
			sets.clear();
			layouts.clear();
			pipelines.clear();
			descriptor_cache.clear();

			for (uint32_t i = 0; i < static_cast<uint32_t>(bindless::layouts.size()); i++)
			{
				RHI_Device::DeletionQueueAdd(RHI_Resource_Type::DescriptorSetLayout, bindless::layouts[i]);
			}
		}
	}
	namespace device_features
	{
		VkPhysicalDeviceFeatures2 pNext = {};
		VkPhysicalDeviceRobustness2FeaturesEXT robustness_features_2 = {};
		VkPhysicalDeviceVulkan13Features features_1_3 = {};
		VkPhysicalDeviceVulkan12Features features_1_2 = {};
		VkPhysicalDeviceFragmentShadingRateFeaturesKHR shading_rate = {};

		uint32_t enabled_graphics_shader_stages;

		void detect(VkPhysicalDevice device_physical, bool* is_shading_rate_supported)
		{
			// structs which hold which features are enabled
			robustness_features_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
			shading_rate.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
			robustness_features_2.pNext = &shading_rate;
			features_1_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
			features_1_3.pNext = &robustness_features_2;
			features_1_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
			features_1_2.pNext = &features_1_3;
			pNext.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
			pNext.pNext = &features_1_2;

			// structs which hold which features are supported
			VkPhysicalDeviceFragmentShadingRateFeaturesKHR shading_rate_support = {};
			shading_rate_support.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
			VkPhysicalDeviceRobustness2FeaturesEXT robustness_2_support = {};
			robustness_2_support.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
			robustness_2_support.pNext = &shading_rate_support;
			VkPhysicalDeviceVulkan13Features features_1_3_support = {};
			features_1_3_support.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
			features_1_3_support.pNext = &robustness_2_support;
			VkPhysicalDeviceVulkan12Features features_1_2_support = {};
			features_1_2_support.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
			features_1_2_support.pNext = &features_1_3_support;
			VkPhysicalDeviceFeatures2 features_support = {};
			features_support.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
			features_support.pNext = &features_1_2_support;
			vkGetPhysicalDeviceFeatures2(device_physical, &features_support);

			// check if certain features are supported and enable them
			{
				// tessellation
				LC_ASSERT(features_support.features.tessellationShader == VK_TRUE);
				pNext.features.tessellationShader = VK_TRUE;

				// geometry
				LC_ASSERT(features_support.features.geometryShader == VK_TRUE);
				pNext.features.geometryShader = VK_TRUE;

				// variable shading rate
				*is_shading_rate_supported = shading_rate_support.attachmentFragmentShadingRate == VK_TRUE;
				if (*is_shading_rate_supported)
				{
					// conditionally enable this as a lot of people have an NV 1080
					// however NV didn't support this until the 1650 and later
					// https://vulkan.gpuinfo.org/listdevicescoverage.php?platform=windows&extension=VK_KHR_fragment_shading_rate
					shading_rate.attachmentFragmentShadingRate = VK_TRUE;
				}

				// depth clamp
				LC_ASSERT(features_support.features.depthClamp == VK_TRUE);
				pNext.features.depthClamp = VK_TRUE;

				// anisotropic filtering
				LC_ASSERT(features_support.features.samplerAnisotropy == VK_TRUE);
				pNext.features.samplerAnisotropy = VK_TRUE;

				// line and point rendering
				LC_ASSERT(features_support.features.fillModeNonSolid == VK_TRUE);
				pNext.features.fillModeNonSolid = VK_TRUE;

				// lines with adjustable thickness
				LC_ASSERT(features_support.features.wideLines == VK_TRUE);
				pNext.features.wideLines = VK_TRUE;

				// cubemaps
				LC_ASSERT(features_support.features.imageCubeArray == VK_TRUE);
				pNext.features.imageCubeArray = VK_TRUE;

				// timeline semaphores
				LC_ASSERT(features_1_2_support.timelineSemaphore == VK_TRUE);
				features_1_2.timelineSemaphore = VK_TRUE;

				// descriptors
				{
					LC_ASSERT(features_1_2_support.descriptorBindingVariableDescriptorCount == VK_TRUE);
					features_1_2.descriptorBindingVariableDescriptorCount = VK_TRUE;

					LC_ASSERT(features_1_2_support.descriptorBindingVariableDescriptorCount == VK_TRUE);
					features_1_2.descriptorBindingVariableDescriptorCount = VK_TRUE;

					LC_ASSERT(features_1_2_support.descriptorBindingSampledImageUpdateAfterBind == VK_TRUE);
					features_1_2.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;

					LC_ASSERT(features_1_2_support.descriptorBindingPartiallyBound == VK_TRUE);
					features_1_2.descriptorBindingPartiallyBound = VK_TRUE;

					LC_ASSERT(features_1_2_support.runtimeDescriptorArray == VK_TRUE);
					features_1_2.runtimeDescriptorArray = VK_TRUE;

					LC_ASSERT(robustness_2_support.nullDescriptor == VK_TRUE);
					robustness_features_2.nullDescriptor = VK_TRUE;
				}

				// quality of life improvements to the API
				{
					// rendering without render passes and frame buffer objects
					LC_ASSERT(features_1_3_support.dynamicRendering == VK_TRUE);
					features_1_3.dynamicRendering = VK_TRUE;

					LC_ASSERT(features_1_3_support.synchronization2 == VK_TRUE);
					features_1_3.synchronization2 = VK_TRUE;
				}

				// FSR 2
				{
					// extended types (int8, int16, int64, etc) - SPD
					LC_ASSERT(features_1_2_support.shaderSubgroupExtendedTypes == VK_TRUE);
					features_1_2.shaderSubgroupExtendedTypes = VK_TRUE;

					// float16 - If supported, FSR 2 will opt for it, so don't assert.
					if (features_1_2_support.shaderFloat16 == VK_TRUE)
					{
						features_1_2.shaderFloat16 = VK_TRUE;
					}

					// int16 - If supported, FSR 2 will opt for it, so don't assert.
					if (features_support.features.shaderInt16 == VK_TRUE)
					{
						pNext.features.shaderInt16 = VK_TRUE;
					}

					// wave64
					LC_ASSERT(features_1_3_support.shaderDemoteToHelperInvocation == VK_TRUE);
					features_1_3.shaderDemoteToHelperInvocation = VK_TRUE;

					// wave64 - If supported, FSR 2 will opt for it, so don't assert.
					if (features_1_3_support.subgroupSizeControl == VK_TRUE)
					{
						features_1_3.subgroupSizeControl = VK_TRUE;
					}
				}

				// enable certain graphics shader stages
				enabled_graphics_shader_stages = 0;
				enabled_graphics_shader_stages |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
				enabled_graphics_shader_stages |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
				enabled_graphics_shader_stages |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
				enabled_graphics_shader_stages |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
				enabled_graphics_shader_stages |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
		}
	}

	void RHI_Device::Initialize()
	{
		LC_ASSERT_MSG(RHI_Context::api_type == RHI_Api_Type::Vulkan, "RHI context not initialized");

		// #ifdef DEBUG
			// Add validation related extensions
		// todo:
		if (RHI_Context::validation)
		{
			RHI_Context::validation_extensions.emplace_back(VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT);
			RHI_Context::validation_extensions.emplace_back(VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT);
			RHI_Context::validation_extensions.emplace_back(VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT);
			// Add debugging related extensions
			RHI_Context::extensions_instance.emplace_back("VK_EXT_debug_report");
			RHI_Context::extensions_instance.emplace_back("VK_EXT_debug_utils");
		}else if(RHI_Context::gpu_markers)
		{
			RHI_Context::extensions_instance.emplace_back("VK_EXT_debug_utils");
		}
		// #endif

		// get the supported extensions out of the requested extensions
		vector<const char*> extensions_supported = get_supported_extensions(RHI_Context::extensions_instance);

		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		VkApplicationInfo app_info = create_application_info();
		create_info.pApplicationInfo = &app_info;

		create_info.enabledExtensionCount = static_cast<uint32_t>(extensions_supported.size());
		create_info.ppEnabledExtensionNames = extensions_supported.data();
		create_info.enabledLayerCount = 0;

		// validation features
		VkValidationFeaturesEXT validation_features = {};
		validation_features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
		validation_features.enabledValidationFeatureCount = static_cast<uint32_t>(RHI_Context::validation_extensions.size());
		validation_features.pEnabledValidationFeatures = RHI_Context::validation_extensions.data();

		if (RHI_Context::validation)
		{
			// Enable validation layer
			if (is_present_instance_layer(RHI_Context::validation_layers.front()))
			{
				// Validation layers
				create_info.enabledLayerCount = static_cast<uint32_t>(RHI_Context::validation_layers.size());
				create_info.ppEnabledLayerNames = RHI_Context::validation_layers.data();
				create_info.pNext = &validation_features;
			}
			else
			{
				DEBUG_LOG_ERROR("Validation layer was requested, but not available.");
			}
		}

		LC_ASSERT_MSG(vkCreateInstance(&create_info, nullptr, &RHI_Context::instance) == VK_SUCCESS, "Failed to create instance");

		// Get function pointers (from extensions)
		functions::initialize(RHI_Context::validation, RHI_Context::gpu_markers);

		// Debug
		if (RHI_Context::validation)
		{
			validation_layer_logging::initialize(RHI_Context::instance);
		}

		// Find a physical device
		LC_ASSERT_MSG(PhysicalDeviceDetect(), "Failed to detect any devices");
		PhysicalDeviceSelectPrimary();

		// Device
		{
			// Queue create info
			vector<VkDeviceQueueCreateInfo> queue_create_infos;
			{
				vector<uint32_t> unique_queue_families =
				{
					queues::index_graphics,
					queues::index_compute,
					queues::index_copy
				};

				float queue_priority = 1.0f;
				for (const uint32_t& queue_family : unique_queue_families)
				{
					VkDeviceQueueCreateInfo queue_create_info = {};
					queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
					queue_create_info.flags = 0;
					queue_create_info.queueFamilyIndex = queue_family;
					queue_create_info.queueCount = 1;
					queue_create_info.pQueuePriorities = &queue_priority;
					queue_create_infos.push_back(queue_create_info);
				}
			}

			// Detect device properties
			{
				VkPhysicalDeviceFragmentShadingRatePropertiesKHR shading_rate_properties = {};
				shading_rate_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;

				VkPhysicalDeviceVulkan13Properties device_properties_1_3 = {};
				device_properties_1_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;
				device_properties_1_3.pNext = &shading_rate_properties;

				VkPhysicalDeviceProperties2 properties_device = {};
				properties_device.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
				properties_device.pNext = &device_properties_1_3;

				vkGetPhysicalDeviceProperties2(static_cast<VkPhysicalDevice>(RHI_Context::device_physical), &properties_device);

				// save some properties
				m_timestamp_period = properties_device.properties.limits.timestampPeriod;
				m_min_uniform_buffer_offset_alignment = properties_device.properties.limits.minUniformBufferOffsetAlignment;
				m_min_storage_buffer_offset_alignment = properties_device.properties.limits.minStorageBufferOffsetAlignment;
				m_max_texture_1d_dimension = properties_device.properties.limits.maxImageDimension1D;
				m_max_texture_2d_dimension = properties_device.properties.limits.maxImageDimension2D;
				m_max_texture_3d_dimension = properties_device.properties.limits.maxImageDimension3D;
				m_max_texture_cube_dimension = properties_device.properties.limits.maxImageDimensionCube;
				m_max_texture_array_layers = properties_device.properties.limits.maxImageArrayLayers;
				m_max_push_constant_size = properties_device.properties.limits.maxPushConstantsSize;
				m_max_shading_rate_texel_size_x = shading_rate_properties.maxFragmentShadingRateAttachmentTexelSize.width;
				m_max_shading_rate_texel_size_y = shading_rate_properties.maxFragmentShadingRateAttachmentTexelSize.height;

				// Disable profiler if timestamps are not supported
				if (RHI_Context::gpu_profiling)
				{
					LC_ASSERT_MSG(properties_device.properties.limits.timestampComputeAndGraphics, "Device doesn't support timestamps");
				}
			}

			device_features::detect(RHI_Context::device_physical, &m_is_shading_rate_supported);

			// Get the supported extensions out of the requested extensions
			vector<const char*> extensions_supported = get_physical_device_supported_extensions(RHI_Context::extensions_device, RHI_Context::device_physical);

			// Device create info
			VkDeviceCreateInfo create_info = {};
			{
				create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
				create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
				create_info.pQueueCreateInfos = queue_create_infos.data();
				create_info.pNext = &device_features::pNext;
				create_info.enabledExtensionCount = static_cast<uint32_t>(extensions_supported.size());
				create_info.ppEnabledExtensionNames = extensions_supported.data();

				if (RHI_Context::validation)
				{
					create_info.enabledLayerCount = static_cast<uint32_t>(RHI_Context::validation_layers.size());
					create_info.ppEnabledLayerNames = RHI_Context::validation_layers.data();
				}
			}

			// Create
			LC_ASSERT_MSG(vkCreateDevice(RHI_Context::device_physical, &create_info, nullptr, &RHI_Context::device) == VK_SUCCESS, "Failed to create device");
		}

		// Get a graphics, compute and a copy queue.
		{
			vkGetDeviceQueue(RHI_Context::device, queues::index_graphics, 0, reinterpret_cast<VkQueue*>(&queues::graphics));
			SetResourceName(queues::graphics, RHI_Resource_Type::Queue, "graphics");

			vkGetDeviceQueue(RHI_Context::device, queues::index_compute, 0, reinterpret_cast<VkQueue*>(&queues::compute));
			SetResourceName(queues::compute, RHI_Resource_Type::Queue, "compute");

			vkGetDeviceQueue(RHI_Context::device, queues::index_copy, 0, reinterpret_cast<VkQueue*>(&queues::copy));
			SetResourceName(queues::copy, RHI_Resource_Type::Queue, "copy");
		}

		vulkan_memory_allocator::initialize(app_info.apiVersion);

		// Set the descriptor set capacity to an initial value
		CreateDescriptorPool();

		// Detect and log version
		{
			string version_major = to_string(VK_VERSION_MAJOR(app_info.apiVersion));
			string version_minor = to_string(VK_VERSION_MINOR(app_info.apiVersion));
			string version_patch = to_string(VK_VERSION_PATCH(app_info.apiVersion));
			string version = version_major + "." + version_minor + "." + version_patch;

			DEBUG_LOG_INFO("Vulkan {}", version);
		}
	}

	void RHI_Device::Tick(const uint64_t frame_count)
	{
		EASY_FUNCTION(profiler::colors::Magenta);
		// https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/staying_within_budget.html
		// Make sure to call vmaSetCurrentFrameIndex() every frame.
		// Budget is queried from Vulkan inside of it to avoid overhead of querying it with every allocation.
		vmaSetCurrentFrameIndex(vulkan_memory_allocator::allocator, static_cast<uint32_t>(frame_count));
	}

	void RHI_Device::Destroy()
	{
		LC_ASSERT(queues::graphics != nullptr);

		QueueWaitAll();

		// Destroy command pools
		command_pools::regular.clear();
		command_pools::immediate.fill(nullptr);

		// Descriptor pool
		vkDestroyDescriptorPool(RHI_Context::device, descriptors::descriptor_pool, nullptr);
		descriptors::descriptor_pool = nullptr;

		// debug messenger
		if (RHI_Context::validation)// TODO
		{
			validation_layer_logging::shutdown(RHI_Context::instance);
		}

		// descriptors
		descriptors::release();

		// the destructor of all the resources enqueues it's vk buffer memory for de-allocation
		// this is where we actually go through them and de-allocate them
		RHI_Device::DeletionQueueParse();

		// Allocator
		vulkan_memory_allocator::destroy();

		// Device and instance
		vkDestroyDevice(RHI_Context::device, nullptr);
		vkDestroyInstance(RHI_Context::instance, nullptr);
	}

	// physical device

	bool RHI_Device::PhysicalDeviceDetect()
	{
		uint32_t device_count = 0;
		LC_ASSERT_MSG(
			vkEnumeratePhysicalDevices(RHI_Context::instance, &device_count, nullptr) == VK_SUCCESS,
			"Failed to get physical device count"
		);

		LC_ASSERT_MSG(device_count != 0, "There are no available physical devices");

		vector<VkPhysicalDevice> physical_devices(device_count);
		LC_ASSERT_MSG(
			vkEnumeratePhysicalDevices(RHI_Context::instance, &device_count, physical_devices.data()) == VK_SUCCESS,
			"Failed to enumerate physical devices"
		);

		// Go through all the devices
		for (const VkPhysicalDevice& device_physical : physical_devices)
		{
			// Get device properties
			VkPhysicalDeviceProperties device_properties = {};
			vkGetPhysicalDeviceProperties(device_physical, &device_properties);

			VkPhysicalDeviceMemoryProperties device_memory_properties = {};
			vkGetPhysicalDeviceMemoryProperties(device_physical, &device_memory_properties);

			RHI_PhysicalDevice_Type type = RHI_PhysicalDevice_Type::Undefined;
			if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) type = RHI_PhysicalDevice_Type::Integrated;
			if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)   type = RHI_PhysicalDevice_Type::Discrete;
			if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)    type = RHI_PhysicalDevice_Type::Virtual;
			if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)            type = RHI_PhysicalDevice_Type::Cpu;

			// Let the engine know about it as it will sort all of the devices from best to worst
			PhysicalDeviceRegister(PhysicalDevice
			(
				device_properties.apiVersion,                                        // api version
				device_properties.driverVersion,                                     // driver version
				device_properties.vendorID,                                          // vendor id
				type,                                                                // type
				&device_properties.deviceName[0],                                    // name
				static_cast<uint64_t>(device_memory_properties.memoryHeaps[0].size), // memory
				static_cast<void*>(device_physical)                                  // data
			));
		}

		return true;
	}

	void RHI_Device::PhysicalDeviceSelectPrimary()
	{
		auto get_queue_family_index = [](VkQueueFlagBits queue_flags, const vector<VkQueueFamilyProperties>& queue_family_properties, uint32_t* index)
		{
			// Dedicated queue for compute
			// Try to find a queue family index that supports compute but not graphics
			if (queue_flags & VK_QUEUE_COMPUTE_BIT)
			{
				for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); i++)
				{
					if ((queue_family_properties[i].queueFlags & queue_flags) && ((queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
					{
						*index = i;
						return true;
					}
				}
			}

			// Dedicated queue for transfer
			// Try to find a queue family index that supports transfer but not graphics and compute
			if (queue_flags & VK_QUEUE_TRANSFER_BIT)
			{
				for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); i++)
				{
					if ((queue_family_properties[i].queueFlags & queue_flags) && ((queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
					{
						*index = i;
						return true;
					}
				}
			}

			// For other queue types or if no separate compute queue is present, return the first one to support the requested flags
			for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); i++)
			{
				if (queue_family_properties[i].queueFlags & queue_flags)
				{
					*index = i;
					return true;
				}
			}

			return false;
		};

		auto get_queue_family_indices = [&get_queue_family_index](const VkPhysicalDevice& physical_device)
		{
			uint32_t queue_family_count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

			vector<VkQueueFamilyProperties> queue_families_properties(queue_family_count);
			vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families_properties.data());

			// Graphics
			uint32_t index = 0;
			if (get_queue_family_index(VK_QUEUE_GRAPHICS_BIT, queue_families_properties, &index))
			{
				QueueSetIndex(RHI_Queue_Type::Graphics, index);
			}
			else
			{
				DEBUG_LOG_ERROR("Graphics queue not suported.");
				return false;
			}

			// Compute
			if (get_queue_family_index(VK_QUEUE_COMPUTE_BIT, queue_families_properties, &index))
			{
				QueueSetIndex(RHI_Queue_Type::Compute, index);
			}
			else
			{
				DEBUG_LOG_ERROR("Compute queue not supported.");
				return false;
			}

			// Copy
			if (get_queue_family_index(VK_QUEUE_TRANSFER_BIT, queue_families_properties, &index))
			{
				QueueSetIndex(RHI_Queue_Type::Copy, index);
			}
			else
			{
				DEBUG_LOG_ERROR("Copy queue not supported.");
				return false;
			}

			return true;
		};

		// Go through all the devices (sorted from best to worst based on their properties)
		for (uint32_t device_index = 0; device_index < PhysicalDeviceGet().size(); device_index++)
		{
			VkPhysicalDevice device = static_cast<VkPhysicalDevice>(PhysicalDeviceGet()[device_index].GetData());

			// Get the first device that has a graphics, a compute and a transfer queue
			if (get_queue_family_indices(device))
			{
				PhysicalDeviceSetPrimary(device_index);
				RHI_Context::device_physical = device;
				break;
			}
		}
	}

	// queues

	void RHI_Device::QueuePresent(void* swapchain, uint32_t* image_index, vector<RHI_Semaphore*>& wait_semaphores)
	{
		EASY_FUNCTION(profiler::colors::Magenta);
		lock_guard<mutex> lock(queues::mutex_queue);

		array<VkSemaphore, 3> vk_wait_semaphores = { nullptr, nullptr, nullptr };

		// Get semaphore Vulkan resource
		uint32_t semaphore_count = static_cast<uint32_t>(wait_semaphores.size());
		for (uint32_t i = 0; i < semaphore_count; i++)
		{
			LC_ASSERT_MSG(wait_semaphores[i]->GetStateCpu() == RHI_Sync_State::Submitted, "The wait semaphore hasn't been signaled");
			vk_wait_semaphores[i] = static_cast<VkSemaphore>(wait_semaphores[i]->GetRhiResource());
		}

		VkPresentInfoKHR present_info = {};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present_info.waitSemaphoreCount = semaphore_count;
		present_info.pWaitSemaphores = vk_wait_semaphores.data();
		present_info.swapchainCount = 1;
		present_info.pSwapchains = reinterpret_cast<VkSwapchainKHR*>(&swapchain);
		present_info.pImageIndices = image_index;

		LC_VK_ASSERT_MSG(vkQueuePresentKHR(static_cast<VkQueue>(queues::graphics), &present_info), "Failed to present");

		// Update semaphore state
		for (uint32_t i = 0; i < semaphore_count; i++)
		{
			wait_semaphores[i]->SetStateCpu(RHI_Sync_State::Idle);
		}
	}

	void RHI_Device::QueueSubmit(const RHI_Queue_Type type, const uint32_t wait_flags, void* cmd_buffer, RHI_Semaphore* wait_semaphore /*= nullptr*/, RHI_Semaphore* signal_semaphore /*= nullptr*/, RHI_Fence* signal_fence /*= nullptr*/)
	{
		EASY_FUNCTION(profiler::colors::Magenta);
		lock_guard<mutex> lock(queues::mutex_queue);

		LC_ASSERT_MSG(cmd_buffer != nullptr, "Invalid command buffer");

		// Validate semaphores
		if (wait_semaphore)   LC_ASSERT_MSG(wait_semaphore->GetStateCpu() != RHI_Sync_State::Idle, "Wait semaphore is in an idle state and will never be signaled");
		if (signal_semaphore) LC_ASSERT_MSG(signal_semaphore->GetStateCpu() != RHI_Sync_State::Submitted, "Signal semaphore is already in a signaled state.");
		if (signal_fence)     LC_ASSERT_MSG(signal_fence->GetStateCpu() != RHI_Sync_State::Submitted, "Signal fence is already in a signaled state.");

		VkSemaphoreSubmitInfoKHR wait_semaphore_info = {};
		if (wait_semaphore)
		{
			wait_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR;
			wait_semaphore_info.semaphore = static_cast<VkSemaphore>(wait_semaphore->GetRhiResource());
			wait_semaphore_info.stageMask = wait_flags; // the pipeline stage the semaphore waits at
			wait_semaphore_info.value = 0;          // timeline semaphore value to wait for (ignored for binary semaphores)
		}

		VkSemaphoreSubmitInfoKHR signal_semaphore_info = {};
		if (signal_semaphore)
		{
			signal_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR;
			signal_semaphore_info.semaphore = static_cast<VkSemaphore>(signal_semaphore->GetRhiResource());
			signal_semaphore_info.value = 0; // timeline semaphore value to signal (ignored for binary semaphores)
		}

		VkCommandBufferSubmitInfoKHR cmd_buffer_info = {};
		cmd_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR;
		cmd_buffer_info.commandBuffer = *reinterpret_cast<VkCommandBuffer*>(&cmd_buffer);

		VkSubmitInfo2 submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		submit_info.waitSemaphoreInfoCount = wait_semaphore ? 1 : 0;
		submit_info.pWaitSemaphoreInfos = &wait_semaphore_info;
		submit_info.signalSemaphoreInfoCount = signal_semaphore ? 1 : 0;
		submit_info.pSignalSemaphoreInfos = &signal_semaphore_info;
		submit_info.commandBufferInfoCount = 1;
		submit_info.pCommandBufferInfos = &cmd_buffer_info;

		// submit
		VkFence vk_signal_fence = static_cast<VkFence>(signal_fence ? signal_fence->GetRhiResource() : nullptr);
		LC_VK_ASSERT_MSG(vkQueueSubmit2(static_cast<VkQueue>(QueueGet(type)), 1, &submit_info, vk_signal_fence), "Failed to submit");

		// update semaphore states
		if (wait_semaphore)   wait_semaphore->SetStateCpu(RHI_Sync_State::Idle);
		if (signal_semaphore) signal_semaphore->SetStateCpu(RHI_Sync_State::Submitted);
		if (signal_fence)     signal_fence->SetStateCpu(RHI_Sync_State::Submitted);
	}

	void RHI_Device::QueueWait(const RHI_Queue_Type type)
	{
		EASY_FUNCTION(profiler::colors::Magenta);
		lock_guard<mutex> lock(queues::mutex_queue);

		LC_VK_ASSERT_MSG(vkQueueWaitIdle(static_cast<VkQueue>(QueueGet(type))), "Failed to wait for queue");
	}

	void* RHI_Device::QueueGet(const RHI_Queue_Type type)
	{
		if (type == RHI_Queue_Type::Graphics)
		{
			return queues::graphics;
		}
		else if (type == RHI_Queue_Type::Copy)
		{
			return queues::copy;
		}
		else if (type == RHI_Queue_Type::Compute)
		{
			return queues::compute;
		}

		return nullptr;
	}

	uint32_t RHI_Device::QueueGetIndex(const RHI_Queue_Type type)
	{
		if (type == RHI_Queue_Type::Graphics)
		{
			return queues::index_graphics;
		}
		else if (type == RHI_Queue_Type::Copy)
		{
			return queues::index_copy;
		}
		else if (type == RHI_Queue_Type::Compute)
		{
			return queues::index_compute;
		}

		return 0;
	}

	void RHI_Device::QueueSetIndex(const RHI_Queue_Type type, const uint32_t index)
	{
		if (type == RHI_Queue_Type::Graphics)
		{
			queues::index_graphics = index;
		}
		else if (type == RHI_Queue_Type::Copy)
		{
			queues::index_copy = index;
		}
		else if (type == RHI_Queue_Type::Compute)
		{
			queues::index_compute = index;
		}
	}

	// deletion queue

	void RHI_Device::DeletionQueueAdd(const RHI_Resource_Type resource_type, void* resource)
	{
		lock_guard<mutex> guard(mutex_deletion_queue);
		deletion_queue[resource_type].emplace_back(resource);
	}

	void RHI_Device::DeletionQueueParse()
	{
		EASY_FUNCTION(profiler::colors::Magenta);
		lock_guard<mutex> guard(mutex_deletion_queue);
		for (const auto& it : deletion_queue)
		{
			for (void* resource : it.second)
			{
				RHI_Resource_Type resource_type = it.first;

				switch (resource_type)
				{
				case RHI_Resource_Type::Texture:             MemoryTextureDestroy(resource); break;
				case RHI_Resource_Type::TextureView:         vkDestroyImageView(RHI_Context::device, static_cast<VkImageView>(resource), nullptr);                     break;
				case RHI_Resource_Type::Sampler:             vkDestroySampler(RHI_Context::device, reinterpret_cast<VkSampler>(resource), nullptr);                    break;
				case RHI_Resource_Type::Buffer:              MemoryBufferDestroy(resource);                                                                            break;
				case RHI_Resource_Type::Shader:              vkDestroyShaderModule(RHI_Context::device, static_cast<VkShaderModule>(resource), nullptr);               break;
				case RHI_Resource_Type::Semaphore:           vkDestroySemaphore(RHI_Context::device, static_cast<VkSemaphore>(resource), nullptr);                     break;
				case RHI_Resource_Type::Fence:               vkDestroyFence(RHI_Context::device, static_cast<VkFence>(resource), nullptr);                             break;
				case RHI_Resource_Type::DescriptorSetLayout: vkDestroyDescriptorSetLayout(RHI_Context::device, static_cast<VkDescriptorSetLayout>(resource), nullptr); break;
				case RHI_Resource_Type::QueryPool:           vkDestroyQueryPool(RHI_Context::device, static_cast<VkQueryPool>(resource), nullptr);                     break;
				case RHI_Resource_Type::Pipeline:            vkDestroyPipeline(RHI_Context::device, static_cast<VkPipeline>(resource), nullptr);                       break;
				case RHI_Resource_Type::PipelineLayout:      vkDestroyPipelineLayout(RHI_Context::device, static_cast<VkPipelineLayout>(resource), nullptr);           break;
				default:                                     LC_ASSERT_MSG(false, "Unknown resource");                                                                 break;
				}

				// delete descriptor sets which are now invalid (because they are referring to a deleted resource)
				if (resource_type == RHI_Resource_Type::TextureView || resource_type == RHI_Resource_Type::Buffer || resource_type == RHI_Resource_Type::Sampler)
				{
					for (auto it = descriptors::sets.begin(); it != descriptors::sets.end();)
					{
						if (it->second.IsReferingToResource(resource))
						{
							it = descriptors::sets.erase(it);
							// ideally the descriptor set pool is not oblivious to the fact that we don't use this set anymore
							// maybe after a certain number of deletions we reset the entire pool to free memory
						}
						else
						{
							++it;
						}
					}
				}
			}
		}

		deletion_queue.clear();
	}

	bool RHI_Device::DeletionQueueNeedsToParse()
	{
		return deletion_queue.size() > 5;
	}

	// descriptors

	void RHI_Device::CreateDescriptorPool()
	{
		static array<VkDescriptorPoolSize, 5> pool_sizes =
		{
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_SAMPLER,                rhi_max_array_size * rhi_max_descriptor_set_count },
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          rhi_max_array_size * rhi_max_descriptor_set_count },
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          rhi_max_array_size * rhi_max_descriptor_set_count },
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, rhi_max_array_size * rhi_max_descriptor_set_count }, // structured buffer
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, rhi_max_array_size * rhi_max_descriptor_set_count }
		};

		// describe
		VkDescriptorPoolCreateInfo pool_create_info = {};
		pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;
		pool_create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
		pool_create_info.pPoolSizes = pool_sizes.data();
		pool_create_info.maxSets = rhi_max_descriptor_set_count;

		// create
		LC_ASSERT(descriptors::descriptor_pool == nullptr);
		LC_VK_ASSERT_MSG(vkCreateDescriptorPool(RHI_Context::device, &pool_create_info, nullptr, &descriptors::descriptor_pool), "Failed to create descriptor pool");

		// Profiler::m_descriptor_set_count = 0;
	}

	void RHI_Device::AllocateDescriptorSet(void*& resource, RHI_DescriptorSetLayout* descriptor_set_layout, const vector<RHI_Descriptor>& descriptors_)
	{
		// verify that an allocation is possible
		{
			LC_ASSERT_MSG(descriptors::allocated_descriptor_sets < rhi_max_descriptor_set_count, "Reached descriptor set limit");

			uint32_t textures = 0;
			uint32_t storage_textures = 0;
			uint32_t storage_buffers = 0;
			uint32_t dynamic_constant_buffers = 0;
			uint32_t samplers = 0;
			for (const RHI_Descriptor& descriptor : descriptors_)
			{
				if (descriptor.type == RHI_Descriptor_Type::Sampler)
				{
					samplers++;
				}
				else if (descriptor.type == RHI_Descriptor_Type::Texture)
				{
					textures++;
				}
				else if (descriptor.type == RHI_Descriptor_Type::TextureStorage)
				{
					storage_textures++;
				}
				else if (descriptor.type == RHI_Descriptor_Type::StructuredBuffer)
				{
					storage_buffers++;
				}
				else if (descriptor.type == RHI_Descriptor_Type::ConstantBuffer)
				{
					dynamic_constant_buffers++;
				}
			}


			LC_ASSERT_MSG(samplers <= rhi_max_array_size, "Descriptor set requires more samplers");
			LC_ASSERT_MSG(textures <= rhi_max_array_size, "Descriptor set requires more textures");
			LC_ASSERT_MSG(storage_textures <= rhi_max_array_size, "Descriptor set requires more storage textures");
			LC_ASSERT_MSG(storage_buffers <= rhi_max_array_size, "Descriptor set requires more dynamic storage buffers");
			LC_ASSERT_MSG(dynamic_constant_buffers <= rhi_max_array_size, "Descriptor set requires more dynamic constant buffers");
		}

		// describe
		array<void*, 1> descriptor_set_layouts = { descriptor_set_layout->GetRhiResource() };
		VkDescriptorSetAllocateInfo allocate_info = {};
		allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocate_info.descriptorPool = static_cast<VkDescriptorPool>(descriptors::descriptor_pool);
		allocate_info.descriptorSetCount = 1;
		allocate_info.pSetLayouts = reinterpret_cast<VkDescriptorSetLayout*>(descriptor_set_layouts.data());

		// allocate
		LC_ASSERT(resource == nullptr);
		LC_VK_ASSERT_MSG(vkAllocateDescriptorSets(RHI_Context::device, &allocate_info, reinterpret_cast<VkDescriptorSet*>(&resource)),
			"Failed to allocate descriptor set");

		// track allocations
		descriptors::allocated_descriptor_sets++;
		// Profiler::m_descriptor_set_count++;
	}

	void* RHI_Device::GetDescriptorSet(const RHI_Device_Resource resource_type)
	{
		return static_cast<void*>(descriptors::bindless::sets[static_cast<uint32_t>(resource_type)]);
	}

	void* RHI_Device::GetDescriptorSetLayout(const RHI_Device_Resource resource_type)
	{
		return static_cast<void*>(descriptors::bindless::layouts[static_cast<uint32_t>(resource_type)]);
	}

	unordered_map<uint64_t, RHI_DescriptorSet>& RHI_Device::GetDescriptorSets()
	{
		return descriptors::sets;
	}

	uint32_t RHI_Device::GetDescriptorType(const RHI_Descriptor& descriptor)
	{
		if (descriptor.type == RHI_Descriptor_Type::Sampler)
			return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER;

		if (descriptor.type == RHI_Descriptor_Type::Texture)
			return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

		if (descriptor.type == RHI_Descriptor_Type::TextureStorage)
			return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

		if (descriptor.type == RHI_Descriptor_Type::StructuredBuffer)
			return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;

		if (descriptor.type == RHI_Descriptor_Type::ConstantBuffer)
			return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;

		LC_ASSERT_MSG(false, "Unhandled descriptor type");
		return VkDescriptorType::VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}

	void RHI_Device::UpdateBindlessResources(const array<shared_ptr<RHI_Sampler>, static_cast<uint32_t>(Renderer_Sampler::Max)>* samplers, array<RHI_Texture*, rhi_max_array_size>* textures)
	{
		descriptors::pipelines.clear();

		if (samplers)
		{
			// comparison
			{
				vector<shared_ptr<RHI_Sampler>> data =
				{
					(*samplers)[0],
				};

				descriptors::bindless::update_samplers(data, 0, RHI_Device_Resource::sampler_comparison);
			}

			// regular
			{
				vector<shared_ptr<RHI_Sampler>> data =
				{
					(*samplers)[1], // point_clamp_edge
					(*samplers)[2], // point_clamp_border
					(*samplers)[3], // point_wrap
					(*samplers)[4], // bilinear_clamp_edge
					(*samplers)[5], // bilinear_clamp_border
					(*samplers)[6], // bilinear_wrap
					(*samplers)[7], // trilinear_clamp
					(*samplers)[8]  // anisotropic_wrap
				};

				descriptors::bindless::update_samplers(data, 1, RHI_Device_Resource::sampler_regular);
			}
		}

		// textures
		{
			//const uint32_t binding_slot = static_cast<uint32_t>(Renderer_BindingsSrv::materials);

			//// by the time vkCmdBindDescriptorSets runs, we can't have a null descriptor set, so make sure there is something there
			//{
			//	bool first_run = descriptors::bindless::sets[static_cast<uint32_t>(RHI_Device_Resource::textures_material)] == nullptr;
			//	bool no_textures = textures == nullptr;

			//	if (first_run && no_textures)
			//	{
			//		array<RHI_Texture*, rhi_max_array_size> array_dummy;
			//		array_dummy.fill(nullptr);
			//		descriptors::bindless::update_textures(&array_dummy, binding_slot);
			//	}
			//}

			//if (textures)
			//{
			//	descriptors::bindless::update_textures(textures, binding_slot);
			//}
		}
	}

	// pipelines

	void RHI_Device::GetOrCreatePipeline(RHI_PipelineState& pso, RHI_Pipeline*& pipeline, RHI_DescriptorSetLayout*& descriptor_set_layout)
	{
		pso.Prepare();

		lock_guard<mutex> lock(descriptors::descriptor_pipeline_mutex);

		descriptor_set_layout = descriptors::get_or_create_descriptor_set_layout(pso).get();

		// if no pipeline exists, create one
		uint64_t hash = pso.GetHash();
		auto it = descriptors::pipelines.find(hash);
		if (it == descriptors::pipelines.end())
		{
			// create a new pipeline
			it = descriptors::pipelines.emplace(make_pair(hash, make_shared<RHI_Pipeline>(pso, descriptor_set_layout))).first;
		}

		pipeline = it->second.get();

	}

	uint32_t RHI_Device::GetPipelineCount()
	{
		return static_cast<uint32_t>(descriptors::pipelines.size());
	}

	// memory

	void* RHI_Device::MemoryGetMappedDataFromBuffer(void* resource)
	{
		if (VmaAllocation allocation = static_cast<VmaAllocation>(vulkan_memory_allocator::get_allocation_from_resource(resource)))
		{
			return allocation->GetMappedData();
		}

		return nullptr;
	}

	void RHI_Device::MemoryBufferCreate(void*& resource, const uint64_t size, uint32_t usage, uint32_t memory_property_flags, const void* data_initial, const char* name)
	{
		lock_guard<mutex> lock(vulkan_memory_allocator::mutex_allocator);

		// Deduce some memory properties
		bool is_buffer_storage = (usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) != 0; // aka structured buffer
		bool is_buffer_constant = (usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) != 0;
		bool is_buffer_index = (usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT) != 0;
		bool is_buffer_vertex = (usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) != 0;
		bool is_buffer_staging = (usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT) != 0;
		bool is_mappable = (memory_property_flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0;
		bool is_transfer_source = (usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT) != 0;
		bool is_transfer_destination = (usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT) != 0;
		bool is_transfer_buffer = is_transfer_source || is_transfer_destination;
		bool map_on_creation = is_buffer_storage || is_buffer_constant || is_buffer_index || is_buffer_vertex;

		// Buffer info
		VkBufferCreateInfo buffer_create_info = {};
		buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_create_info.size = size;
		buffer_create_info.usage = usage;
		buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // ��ռ

		// Allocation info
		VmaAllocationCreateInfo allocation_create_info = {};
		allocation_create_info.usage = VMA_MEMORY_USAGE_AUTO;
		allocation_create_info.requiredFlags = memory_property_flags;
		allocation_create_info.flags = 0;

		if (is_buffer_staging)
		{
			allocation_create_info.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
		}
		else
		{
			// Can it be mapped ? Buffers that use Map()/Unmap() need this, persistent buffers also need this.
			allocation_create_info.flags |= is_mappable ? VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT : 0;

			// Can it be mapped upon creation ? This is what a persistent buffer would use.
			allocation_create_info.flags |= (map_on_creation && !is_transfer_buffer) ? VMA_ALLOCATION_CREATE_MAPPED_BIT : 0;

			// Cached on the CPU ? Our constant buffers are using dynamic offsets and do a lot of updates, so we need fast access.
			allocation_create_info.flags |= (is_buffer_constant || is_buffer_storage) ? VK_MEMORY_PROPERTY_HOST_CACHED_BIT : 0;
		}

		// Create the buffer
		VmaAllocation allocation = nullptr;
		VmaAllocationInfo allocation_info;
		LC_VK_ASSERT_MSG(vmaCreateBuffer(
			vulkan_memory_allocator::allocator,
			&buffer_create_info,
			&allocation_create_info,
			reinterpret_cast<VkBuffer*>(&resource),
			&allocation,
			&allocation_info),
			"Failed to created buffer");

		// If a pointer to the buffer data has been passed, map the buffer and copy over the data
		if (data_initial != nullptr)
		{
			LC_ASSERT(is_mappable && "Mapping initial data requires the buffer to be created with a VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT memory flag.");

			// Memory in Vulkan doesn't need to be unmapped before using it on GPU, but unless a
			// memory type has VK_MEMORY_PROPERTY_HOST_COHERENT_BIT flag set, you need to manually
			// invalidate the cache before reading a mapped pointer and flush cache after writing to
			// it. Map/unmap operations don't do that automatically.

			void* mapped_data = nullptr;
			LC_VK_ASSERT_MSG(vmaMapMemory(vulkan_memory_allocator::allocator, allocation, &mapped_data), "Failed to map allocation");
			memcpy(mapped_data, data_initial, size);
			LC_VK_ASSERT_MSG(vmaFlushAllocation(vulkan_memory_allocator::allocator, allocation, 0, size), "Failed to flush allocation");
			vmaUnmapMemory(vulkan_memory_allocator::allocator, allocation);
		}

		vulkan_memory_allocator::save_allocation(resource, name, allocation);
	}

	void RHI_Device::MemoryBufferDestroy(void*& resource)
	{
		LC_ASSERT_MSG(resource != nullptr, "Resource is null");
		lock_guard<mutex> lock(vulkan_memory_allocator::mutex_allocator);

		if (VmaAllocation allocation = static_cast<VmaAllocation>(vulkan_memory_allocator::get_allocation_from_resource(resource)))
		{
			vmaDestroyBuffer(vulkan_memory_allocator::allocator, static_cast<VkBuffer>(resource), allocation);
			vulkan_memory_allocator::destroy_allocation(resource);
		}
	}

	void RHI_Device::MemoryTextureCreate(RHI_Texture* texture)
	{
		// describe image
		VkImageCreateInfo create_info_image = {};
		create_info_image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		create_info_image.imageType = VK_IMAGE_TYPE_2D;
		create_info_image.flags = texture->GetResourceType() == ResourceType::TextureCube ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
		create_info_image.usage = get_image_usage_flags(texture);
		create_info_image.extent.width = texture->GetWidth();
		create_info_image.extent.height = texture->GetHeight();
		create_info_image.extent.depth = 1;
		create_info_image.mipLevels = texture->GetMipCount();
		create_info_image.arrayLayers = texture->GetArrayLength();
		create_info_image.format = vulkan_format[rhi_format_to_index(texture->GetFormat())];
		create_info_image.tiling = get_format_tiling(texture);
		create_info_image.initialLayout = vulkan_image_layout[static_cast<uint8_t>(texture->GetLayout(0))];
		create_info_image.samples = VK_SAMPLE_COUNT_1_BIT;
		create_info_image.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


		// check physical device format support
		{
			VkPhysicalDeviceImageFormatInfo2 info = {};
			info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
			info.format = create_info_image.format;
			info.type = create_info_image.imageType;
			info.tiling = create_info_image.tiling;
			info.usage = create_info_image.usage;
			info.flags = create_info_image.flags;

			VkImageFormatProperties2 properties = {};
			properties.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;

			VkResult result = vkGetPhysicalDeviceImageFormatProperties2(RHI_Context::device_physical, &info, &properties);
			LC_ASSERT_MSG(result != VK_ERROR_FORMAT_NOT_SUPPORTED, "The GPU doesn't support this image format with the specified properties");
		}

		// describe allocation
		VmaAllocationCreateInfo create_info_allocation = {};
		create_info_allocation.usage = VMA_MEMORY_USAGE_AUTO;
		if (texture->GetFlags() & RHI_Texture_Mappable)
		{
			create_info_allocation.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		}

		// allocate
		VmaAllocationInfo allocation_info;
		VmaAllocation allocation;
		void*& resource = texture->GetRhiResource();
		LC_VK_ASSERT_MSG(vmaCreateImage(
			vulkan_memory_allocator::allocator,
			&create_info_image,
			&create_info_allocation,
			reinterpret_cast<VkImage*>(&resource),
			&allocation,
			&allocation_info),
			"Failed to allocate texture");

		// save mapped data pointer
		if (texture->GetFlags() & RHI_Texture_Mappable)
		{
			void*& mapped_data = texture->GetMappedData();
			mapped_data = allocation_info.pMappedData;
		}

		// save allocation
		vulkan_memory_allocator::save_allocation(resource, texture->GetObjectName().c_str(), allocation);
	}

	void RHI_Device::MemoryTextureDestroy(void*& resource)
	{
		LC_ASSERT_MSG(resource != nullptr, "Resource is null");
		lock_guard<mutex> lock(vulkan_memory_allocator::mutex_allocator);

		if (VmaAllocation allocation = static_cast<VmaAllocation>(vulkan_memory_allocator::get_allocation_from_resource(resource)))
		{
			vmaDestroyImage(vulkan_memory_allocator::allocator, static_cast<VkImage>(resource), allocation);
			vulkan_memory_allocator::destroy_allocation(resource);
		}
	}

	void RHI_Device::MemoryMap(void* resource, void*& mapped_data)
	{
		if (VmaAllocation allocation = static_cast<VmaAllocation>(vulkan_memory_allocator::get_allocation_from_resource(resource)))
		{
			LC_ASSERT_MSG(vmaMapMemory(vulkan_memory_allocator::allocator, allocation, reinterpret_cast<void**>(&mapped_data)) == VK_SUCCESS, "Failed to map memory");
		}
	}

	void RHI_Device::MemoryUnmap(void* resource)
	{
		if (VmaAllocation allocation = static_cast<VmaAllocation>(vulkan_memory_allocator::get_allocation_from_resource(resource)))
		{
			vmaUnmapMemory(vulkan_memory_allocator::allocator, static_cast<VmaAllocation>(allocation));
		}
	}

	uint32_t RHI_Device::MemoryGetUsageMb()
	{
		VkDeviceSize bytes = 0;

		// Get the physical device memory properties
		VkPhysicalDeviceMemoryProperties memory_properties;
		vkGetPhysicalDeviceMemoryProperties(static_cast<VkPhysicalDevice>(RHI_Context::device_physical), &memory_properties);

		// Get the memory usage
		VmaBudget budgets[VK_MAX_MEMORY_HEAPS];
		vmaGetHeapBudgets(vulkan_memory_allocator::allocator, budgets);
		for (uint32_t i = 0; i < VK_MAX_MEMORY_HEAPS; i++)
		{
			// Only consider device local heaps
			if (memory_properties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
			{
				if (budgets[i].budget < 1ull << 60)
				{
					bytes += budgets[i].usage;
				}
			}
		}

		return static_cast<uint32_t>(bytes / 1024 / 1024);
	}

	uint32_t RHI_Device::MemoryGetBudgetMb()
	{
		VkDeviceSize bytes = 0;

		// Get the physical device memory properties
		VkPhysicalDeviceMemoryProperties memory_properties;
		vkGetPhysicalDeviceMemoryProperties(static_cast<VkPhysicalDevice>(RHI_Context::device_physical), &memory_properties);

		// Get available memory
		VmaBudget budgets[VK_MAX_MEMORY_HEAPS];
		vmaGetHeapBudgets(vulkan_memory_allocator::allocator, budgets);
		for (uint32_t i = 0; i < VK_MAX_MEMORY_HEAPS; i++)
		{
			// Only consider device local heaps
			if (memory_properties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
			{
				if (budgets[i].budget < 1ull << 60)
				{
					bytes += budgets[i].budget;
				}
			}
		}

		return static_cast<uint32_t>(bytes / 1024 / 1024);
	}

	// immediate command list

	RHI_CommandList* RHI_Device::CmdImmediateBegin(const RHI_Queue_Type queue_type)
	{
		// Wait until it's safe to proceed
		unique_lock<mutex> lock(command_pools::mutex_immediate_execution);
		command_pools::condition_variable_immediate_execution.wait(lock, [] { return !command_pools::is_immediate_executing; });
		command_pools::is_immediate_executing = true;

		// Create command pool for the given queue type, if needed.
		uint32_t queue_index = static_cast<uint32_t>(queue_type);
		if (!command_pools::immediate[queue_index])
		{
			command_pools::immediate[queue_index] = make_shared<RHI_CommandPool>("cmd_immediate_execution", 0, queue_type);
		}

		//  Get command pool
		RHI_CommandPool* cmd_pool = command_pools::immediate[queue_index].get();

		cmd_pool->Tick();
		cmd_pool->GetCurrentCommandList()->Begin();

		return cmd_pool->GetCurrentCommandList();
	}

	void RHI_Device::CmdImmediateSubmit(RHI_CommandList* cmd_list)
	{
		cmd_list->End();
		cmd_list->Submit();
		cmd_list->WaitForExecution();

		// Signal that it's safe to proceed with the next ImmediateBegin()
		command_pools::is_immediate_executing = false;
		command_pools::condition_variable_immediate_execution.notify_one();
	}

	// command pools

	RHI_CommandPool* RHI_Device::CommandPoolAllocate(const char* name, const uint64_t swap_chain_id, const RHI_Queue_Type queue_type)
	{
		return command_pools::regular.emplace_back(make_shared<RHI_CommandPool>(name, swap_chain_id, queue_type)).get();
	}

	void RHI_Device::CommandPoolDestroy(RHI_CommandPool* cmd_pool)
	{
		vector<shared_ptr<RHI_CommandPool>>::iterator it;
		for (it = command_pools::regular.begin(); it != command_pools::regular.end();)
		{
			if (cmd_pool->GetObjectId() == (*it)->GetObjectId())
			{
				it = command_pools::regular.erase(it);
				return;
			}
			it++;
		}
	}

	const vector<shared_ptr<RHI_CommandPool>>& RHI_Device::GetCommandPools()
	{
		return command_pools::regular;
	}

	// markers

	void RHI_Device::MarkerBegin(RHI_CommandList* cmd_list, const char* name, const Vector4& color)
	{
		LC_ASSERT(RHI_Context::gpu_markers);
		LC_ASSERT(functions::marker_begin != nullptr);

		VkDebugUtilsLabelEXT label = {};
		label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
		label.pNext = nullptr;
		label.pLabelName = name;
		label.color[0] = color.x;
		label.color[1] = color.y;
		label.color[2] = color.z;
		label.color[3] = color.w;

		functions::marker_begin(static_cast<VkCommandBuffer>(cmd_list->GetRhiResource()), &label);
	}

	void RHI_Device::MarkerEnd(RHI_CommandList* cmd_list)
	{
		LC_ASSERT(RHI_Context::gpu_markers);

		functions::marker_end(static_cast<VkCommandBuffer>(cmd_list->GetRhiResource()));
	}

	// misc

	void RHI_Device::SetResourceName(void* resource, const RHI_Resource_Type resource_type, const std::string name)
	{
		if (RHI_Context::validation) // function pointers are not initialized if validation disabled 
		{
			LC_ASSERT(resource != nullptr);
			LC_ASSERT(functions::set_object_name != nullptr);

			VkDebugUtilsObjectNameInfoEXT name_info = {};
			name_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			name_info.pNext = nullptr;
			name_info.objectType = vulkan_object_type[static_cast<uint32_t>(resource_type)];
			name_info.objectHandle = reinterpret_cast<uint64_t>(resource);
			name_info.pObjectName = name.c_str();

			functions::set_object_name(RHI_Context::device, &name_info);
		}
	}

	uint32_t RHI_Device::GetEnabledGraphicsStages()
	{
		return device_features::enabled_graphics_shader_stages;
	}

	void RHI_Device::SetVariableRateShading(const RHI_CommandList* cmd_list, const bool enabled)
	{
		if (!m_is_shading_rate_supported)
			return;

		// set the fragment shading rate state for the current pipeline
		VkExtent2D fragment_size = { 1, 1 };
		VkFragmentShadingRateCombinerOpKHR combiner_operatins[2];

		// The combiners determine how the different shading rate values for the pipeline, primitives and attachment are combined
		if (enabled)
		{
			// If shading rate from attachment is enabled, we set the combiner, so that the values from the attachment are used
			// Combiner for pipeline (A) and primitive (B) - Not used in this sample
			combiner_operatins[0] = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;
			// Combiner for pipeline (A) and attachment (B), replace the pipeline default value (fragment_size) with the fragment sizes stored in the attachment
			combiner_operatins[1] = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_KHR;
		}
		else
		{
			// If shading rate from attachment is disabled, we keep the value set via the dynamic state
			combiner_operatins[0] = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;
			combiner_operatins[1] = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;
		}

		functions::set_fragment_shading_rate(static_cast<VkCommandBuffer>(cmd_list->GetRhiResource()), &fragment_size, combiner_operatins);
	}
}
