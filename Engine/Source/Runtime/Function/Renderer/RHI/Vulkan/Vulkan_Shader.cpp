
//= INCLUDES ============================
#include "Runtime/Core/pch.h"
#include "../RHI_Implementation.h"
#include "../RHI_Device.h"
#include "../RHI_Shader.h"
#include "../RHI_InputLayout.h"
#include "../RHI_DirectXShaderCompiler.h"
SP_WARNINGS_OFF
#include <spirv_cross/spirv_hlsl.hpp>
SP_WARNINGS_ON
//=======================================

//= NAMESPACES =======================
using namespace std;
using namespace SPIRV_CROSS_NAMESPACE;
//====================================

namespace Spartan
{
    namespace
    {
        void spirv_resources_to_descriptors(
            const CompilerHLSL& compiler,
            vector<RHI_Descriptor>& descriptors,
            const SmallVector<Resource>& resources,
            const RHI_Descriptor_Type descriptor_type,
            const RHI_Shader_Stage shader_stage
        )
        {
            // this only matters for textures
            RHI_Image_Layout layout = RHI_Image_Layout::Undefined;
            layout                  = descriptor_type == RHI_Descriptor_Type::TextureStorage ? RHI_Image_Layout::General                  : layout;
            layout                  = descriptor_type == RHI_Descriptor_Type::Texture        ? RHI_Image_Layout::Shader_Read_Only_Optimal : layout;

            for (const Resource& resource : resources)
            {
                uint32_t slot         = compiler.get_decoration(resource.id, spv::DecorationBinding);
                SPIRType type         = compiler.get_type(resource.type_id);
                uint32_t array_length = !type.array.empty() ? type.array[0] : 0;
                uint32_t size         = 0;
                if (descriptor_type == RHI_Descriptor_Type::ConstantBuffer || descriptor_type == RHI_Descriptor_Type::PushConstantBuffer)
                {
                    size = static_cast<uint32_t>(compiler.get_declared_struct_size(type));
                }

                descriptors.emplace_back
                (
                    resource.name,   // name
                    descriptor_type, // type
                    layout,          // layout
                    slot,            // slot
                    array_length,    // array length
                    shader_stage,    // stage
                    size             // struct size
                );
            }
        };
    }

    RHI_Shader::~RHI_Shader()
    {
        if (m_rhi_resource)
        {
            RHI_Device::DeletionQueueAdd(RHI_Resource_Type::Shader, m_rhi_resource);
            m_rhi_resource = nullptr;
        }
    }

    void* RHI_Shader::RHI_Compile()
    {
        vector<string> arguments;

        // arguments
        {
            arguments.emplace_back("-E"); arguments.emplace_back(GetEntryPoint());
            arguments.emplace_back("-T"); arguments.emplace_back(GetTargetProfile());

            // spir-v
            {
                arguments.emplace_back("-spirv");                     // generate SPIR-V code
                arguments.emplace_back("-fspv-target-env=vulkan1.3"); // specify the target environment

                // this prevents all sorts of issues with constant buffers having random data.
                arguments.emplace_back("-fspv-preserve-bindings");  // preserves all bindings declared within the module, even when those bindings are unused
                arguments.emplace_back("-fspv-preserve-interface"); // preserves all interface variables in the entry point, even when those variables are unused

                // shift registers to avoid conflicts
                arguments.emplace_back("-fvk-u-shift"); arguments.emplace_back(to_string(rhi_shader_shift_register_u)); arguments.emplace_back("all"); // binding number shift for u-type (read/write buffer) register
                arguments.emplace_back("-fvk-b-shift"); arguments.emplace_back(to_string(rhi_shader_shift_register_b)); arguments.emplace_back("all"); // binding number shift for b-type (buffer) register
                arguments.emplace_back("-fvk-t-shift"); arguments.emplace_back(to_string(rhi_shader_shift_register_t)); arguments.emplace_back("all"); // binding number shift for t-type (texture) register
                arguments.emplace_back("-fvk-s-shift"); arguments.emplace_back(to_string(rhi_shader_shift_register_s)); arguments.emplace_back("all"); // binding number shift for s-type (sampler) register
            }

            // directX conventions
            {
                arguments.emplace_back("-fvk-use-dx-layout");     // use DirectX memory layout for Vulkan resources
                arguments.emplace_back("-fvk-use-dx-position-w"); // reciprocate SV_Position.w after reading from stage input in PS to accommodate the difference between Vulkan and DirectX

                // Negate SV_Position.y before writing to stage output in VS/DS/GS to accommodate Vulkan's coordinate system
                if (m_shader_type == RHI_Shader_Vertex)
                {
                    arguments.emplace_back("-fvk-invert-y");
                }
            }

            // debug: disable optimizations and embed HLSL source in the shaders
            #ifdef DEBUG
            arguments.emplace_back("-Od");           // disable optimizations
            arguments.emplace_back("-Zi");           // enable debug information
            arguments.emplace_back("-Qembed_debug"); // embed PDB in shader container (must be used with -Zi)
            #endif

            // misc
            arguments.emplace_back("-Zpc"); // pack matrices in column-major order
        }

        // defines
        for (const auto& define : m_defines)
        {
            arguments.emplace_back("-D"); arguments.emplace_back(define.first + "=" + define.second);
        }

        // compile
        if (IDxcResult* dxc_result = DirecXShaderCompiler::Compile(m_preprocessed_source, arguments))
        {
            // get compiled shader buffer
            IDxcBlob* shader_buffer = nullptr;
            dxc_result->GetResult(&shader_buffer);

            // create shader module
            VkShaderModule shader_module         = nullptr;
            VkShaderModuleCreateInfo create_info = {};
            create_info.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            create_info.codeSize                 = static_cast<size_t>(shader_buffer->GetBufferSize());
            create_info.pCode                    = reinterpret_cast<const uint32_t*>(shader_buffer->GetBufferPointer());

            SP_VK_ASSERT_MSG(vkCreateShaderModule(RHI_Context::device, &create_info, nullptr, &shader_module), "Failed to create shader module");

            // name the shader module (useful for GPU-based validation)
            RHI_Device::SetResourceName(static_cast<void*>(shader_module), RHI_Resource_Type::Shader, m_object_name.c_str());

            // reflect shader resources (so that descriptor sets can be created later)
            Reflect
            (
                m_shader_type,
                reinterpret_cast<uint32_t*>(shader_buffer->GetBufferPointer()),
                static_cast<uint32_t>(shader_buffer->GetBufferSize() / 4)
            );
            
            // create input layout
            if (m_input_layout)
            {
                m_input_layout->Create(m_vertex_type, nullptr);
            }

            // release
            dxc_result->Release();

            return static_cast<void*>(shader_module);
        }

        return nullptr;
    }

    void RHI_Shader::Reflect(const RHI_Shader_Stage shader_stage, const uint32_t* ptr, const uint32_t size)
    {
        SP_ASSERT(ptr != nullptr);
        SP_ASSERT(size != 0);
        
        const CompilerHLSL compiler = CompilerHLSL(ptr, size);
        ShaderResources resources   = compiler.get_shader_resources();

        spirv_resources_to_descriptors(compiler, m_descriptors, resources.separate_images,       RHI_Descriptor_Type::Texture,            shader_stage); // SRVs
        spirv_resources_to_descriptors(compiler, m_descriptors, resources.storage_images,        RHI_Descriptor_Type::TextureStorage,     shader_stage); // UAVs
        spirv_resources_to_descriptors(compiler, m_descriptors, resources.storage_buffers,       RHI_Descriptor_Type::StructuredBuffer,   shader_stage);
        spirv_resources_to_descriptors(compiler, m_descriptors, resources.uniform_buffers,       RHI_Descriptor_Type::ConstantBuffer,     shader_stage);
        spirv_resources_to_descriptors(compiler, m_descriptors, resources.push_constant_buffers, RHI_Descriptor_Type::PushConstantBuffer, shader_stage);
        spirv_resources_to_descriptors(compiler, m_descriptors, resources.separate_samplers,     RHI_Descriptor_Type::Sampler,            shader_stage);
    }
}
