
//= INCLUDES ==========================
#include "Runtime/Core/pch.h"
#include "../RHI_Pipeline.h"
#include "../RHI_Implementation.h"
#include "../RHI_Shader.h"
#include "../RHI_SwapChain.h"
#include "../RHI_BlendState.h"
#include "../RHI_InputLayout.h"
#include "../RHI_RasterizerState.h"
#include "../RHI_DepthStencilState.h"
#include "../RHI_DescriptorSetLayout.h"
#include "../RHI_Device.h"
#include "../RHI_Texture.h"
//=====================================

//= NAMESPACES =====
using namespace std;
//==================

namespace LitchiRuntime
{
    RHI_Pipeline::RHI_Pipeline(RHI_PipelineState& pipeline_state, RHI_DescriptorSetLayout* descriptor_set_layout)
    {
        m_state = pipeline_state;

        // pipeline layout
        {
            // order is important here, as it will be used to index the descriptor sets
            array<void*, 3> layouts =
            {
                descriptor_set_layout->GetRhiResource(),
                RHI_Device::GetDescriptorSetLayout(RHI_Device_Resource::sampler_comparison),
                RHI_Device::GetDescriptorSetLayout(RHI_Device_Resource::sampler_regular)
            };

            // validate descriptor set layouts
            for (void* layout : layouts)
            {
                LC_ASSERT(layout != nullptr);
            }

            // push constant buffers
            vector<VkPushConstantRange> push_constant_ranges;
            for (const RHI_Descriptor& descriptor : descriptor_set_layout->GetDescriptors())
            {
                if (descriptor.type == RHI_Descriptor_Type::PushConstantBuffer)
                { 
                    LC_ASSERT(descriptor.struct_size <= RHI_Device::PropertyGetMaxPushConstantSize());

                    VkPushConstantRange push_constant_range = {};
                    push_constant_range.offset = 0;
                    push_constant_range.size = descriptor.struct_size;
                    push_constant_range.stageFlags = 0;
                    push_constant_range.stageFlags |= (descriptor.stage & RHI_Shader_Stage::RHI_Shader_Vertex) ? VK_SHADER_STAGE_VERTEX_BIT : 0;
                    push_constant_range.stageFlags |= (descriptor.stage & RHI_Shader_Stage::RHI_Shader_Hull) ? VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT : 0;
                    push_constant_range.stageFlags |= (descriptor.stage & RHI_Shader_Stage::RHI_Shader_Domain) ? VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT : 0;
                    push_constant_range.stageFlags |= (descriptor.stage & RHI_Shader_Stage::RHI_Shader_Pixel) ? VK_SHADER_STAGE_FRAGMENT_BIT : 0;
                    push_constant_range.stageFlags |= (descriptor.stage & RHI_Shader_Stage::RHI_Shader_Compute) ? VK_SHADER_STAGE_COMPUTE_BIT : 0;

                    push_constant_ranges.emplace_back(push_constant_range);

                    push_constant_ranges.emplace_back(push_constant_range);
                }
            }
         
            // pipeline layout
            VkPipelineLayoutCreateInfo pipeline_layout_info = {};
            pipeline_layout_info.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipeline_layout_info.pushConstantRangeCount     = 0;
            pipeline_layout_info.setLayoutCount             = static_cast<uint32_t>(layouts.size());
            pipeline_layout_info.pSetLayouts                = reinterpret_cast<VkDescriptorSetLayout*>(layouts.data());
            pipeline_layout_info.pushConstantRangeCount     = static_cast<uint32_t>(push_constant_ranges.size());
            pipeline_layout_info.pPushConstantRanges        = push_constant_ranges.data();

            // create
            LC_VK_ASSERT_MSG(vkCreatePipelineLayout(RHI_Context::device, &pipeline_layout_info, nullptr, reinterpret_cast<VkPipelineLayout*>(&m_resource_pipeline_layout)),
                "Failed to create pipeline layout");

            // name
            RHI_Device::SetResourceName(m_resource_pipeline_layout, RHI_Resource_Type::PipelineLayout, pipeline_state.name);
        }

        // viewport & scissor
        vector<VkDynamicState> dynamic_states            = {};
        VkPipelineDynamicStateCreateInfo dynamic_state   = {};
        VkViewport vkViewport                            = {};
        VkRect2D scissor                                 = {};
        VkPipelineViewportStateCreateInfo viewport_state = {};
        {
            // enable dynamic states
            dynamic_states.push_back(VK_DYNAMIC_STATE_VIEWPORT);
            if (m_state.IsGraphics())
            {
                dynamic_states.push_back(VK_DYNAMIC_STATE_SCISSOR);
                dynamic_states.push_back(VK_DYNAMIC_STATE_CULL_MODE);
                dynamic_states.push_back(VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR);
            }

            // dynamic states
            dynamic_state.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamic_state.pNext             = nullptr;
            dynamic_state.flags             = 0;
            dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
            dynamic_state.pDynamicStates    = dynamic_states.data();
        
            // viewport
            vkViewport.x        = 0;
            vkViewport.y        = 0;
            vkViewport.width    = static_cast<float>(m_state.GetWidth());
            vkViewport.height   = static_cast<float>(m_state.GetHeight());
            vkViewport.minDepth = 0.0f;
            vkViewport.maxDepth = 1.0f;
        
            // scissor
            scissor.offset.x      = 0;
            scissor.offset.y      = 0;
            scissor.extent.width  = static_cast<uint32_t>(vkViewport.width);
            scissor.extent.height = static_cast<uint32_t>(vkViewport.height);
        
            // viewport state
            viewport_state.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewport_state.viewportCount = 1;
            viewport_state.pViewports    = &vkViewport;
            viewport_state.scissorCount  = 1;
            viewport_state.pScissors     = &scissor;
        }
        
        // shader stages
        vector<VkPipelineShaderStageCreateInfo> shader_stages;
        
        // Shader - Vertex
        if (m_state.shader_vertex)
        {
            VkPipelineShaderStageCreateInfo shader_stage_info = {};
            shader_stage_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shader_stage_info.stage                           = VK_SHADER_STAGE_VERTEX_BIT;
            shader_stage_info.module                          = static_cast<VkShaderModule>(m_state.shader_vertex->GetRhiResource());
            shader_stage_info.pName                           = m_state.shader_vertex->GetEntryPoint();

            // Validate shader stage
            LC_ASSERT(shader_stage_info.module != nullptr);
            LC_ASSERT(shader_stage_info.pName != nullptr);

            shader_stages.push_back(shader_stage_info);
        }

        // shader - hull
        if (m_state.shader_hull)
        {
            VkPipelineShaderStageCreateInfo shader_stage_info = {};
            shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shader_stage_info.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            shader_stage_info.module = static_cast<VkShaderModule>(m_state.shader_hull->GetRhiResource());
            shader_stage_info.pName = m_state.shader_hull->GetEntryPoint();

            // validate shader stage
            LC_ASSERT(shader_stage_info.module != nullptr);
            LC_ASSERT(shader_stage_info.pName != nullptr);

            shader_stages.push_back(shader_stage_info);
        }

        // shader - domain
        if (m_state.shader_domain)
        {
            VkPipelineShaderStageCreateInfo shader_stage_info = {};
            shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shader_stage_info.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
            shader_stage_info.module = static_cast<VkShaderModule>(m_state.shader_domain->GetRhiResource());
            shader_stage_info.pName = m_state.shader_domain->GetEntryPoint();

            // validate shader stage
            LC_ASSERT(shader_stage_info.module != nullptr);
            LC_ASSERT(shader_stage_info.pName != nullptr);

            shader_stages.push_back(shader_stage_info);
        }

        // Shader - Pixel
        if (m_state.shader_pixel)
        {
            VkPipelineShaderStageCreateInfo shader_stage_info = {};
            shader_stage_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shader_stage_info.stage                           = VK_SHADER_STAGE_FRAGMENT_BIT;
            shader_stage_info.module                          = static_cast<VkShaderModule>(m_state.shader_pixel->GetRhiResource());
            shader_stage_info.pName                           = m_state.shader_pixel->GetEntryPoint();

            // Validate shader stage
            LC_ASSERT(shader_stage_info.module != nullptr);
            LC_ASSERT(shader_stage_info.pName != nullptr);

            shader_stages.push_back(shader_stage_info);
        }

        // Shader - Compute
        if (m_state.shader_compute)
        {
            VkPipelineShaderStageCreateInfo shader_stage_info = {};
            shader_stage_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shader_stage_info.stage                           = VK_SHADER_STAGE_COMPUTE_BIT;
            shader_stage_info.module                          = static_cast<VkShaderModule>(m_state.shader_compute->GetRhiResource());
            shader_stage_info.pName                           = m_state.shader_compute->GetEntryPoint();

            // Validate shader stage
            LC_ASSERT(shader_stage_info.module != nullptr);
            LC_ASSERT(shader_stage_info.pName != nullptr);

            shader_stages.push_back(shader_stage_info);
        }
        
        // Binding description
        VkVertexInputBindingDescription binding_description = {};
        binding_description.binding                         = 0;
        binding_description.inputRate                       = VK_VERTEX_INPUT_RATE_VERTEX;
        binding_description.stride                          = m_state.shader_vertex ? m_state.shader_vertex->GetVertexSize() : 0;
        
        // Vertex attributes description
        vector<VkVertexInputBindingDescription> vertex_input_binding_descs;
        vector<VkVertexInputAttributeDescription> vertex_attribute_descs;
        if (m_state.shader_vertex)
        {
            vertex_input_binding_descs.push_back
            ({
                0,
                m_state.shader_vertex ? m_state.shader_vertex->GetVertexSize() : 0,
                VK_VERTEX_INPUT_RATE_VERTEX
                });

            if (RHI_InputLayout* input_layout = m_state.shader_vertex->GetInputLayout().get())
            {
                vertex_attribute_descs.reserve(input_layout->GetAttributeDescriptions().size());
                for (const auto& desc : input_layout->GetAttributeDescriptions())
                {
                    vertex_attribute_descs.push_back
                    ({
                        desc.location,                                   // location
                        desc.binding,                                    // binding
                        vulkan_format[rhi_format_to_index(desc.format)], // format
                        desc.offset                                      // offset
                        });
                }
            }

            //if (m_state.instancing)
            //{
            //    // hardcoded for now
            //    vertex_input_binding_descs.push_back
            //    ({
            //        1,                            // binding
            //        sizeof(Math::Matrix),         // stride
            //        VK_VERTEX_INPUT_RATE_INSTANCE // inputRate
            //        });
            //}


            //if (m_state.instancing)
            //{
            //    // update the attribute descriptions to pass the entire matrix
            //    // each row of the matrix is treated as a separate attribute
            //    for (uint32_t i = 0; i < 4; i++)
            //    {
            //        vertex_attribute_descs.push_back
            //        ({
            //            static_cast<uint32_t>(vertex_attribute_descs.size()), // location, assuming the next available location
            //            1,                                                    // binding
            //            VK_FORMAT_R32G32B32A32_SFLOAT,                        // format, assuming 32-bit float components
            //            i * sizeof(Math::Vector4)                             // offset, assuming Math::Vector4 is the type of each row
            //            });
            //    }
            //}
        }

        // Vertex input state
        VkPipelineVertexInputStateCreateInfo vertex_input_state = {};
        {
            vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertex_input_state.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_input_binding_descs.size());
            vertex_input_state.pVertexBindingDescriptions = vertex_input_binding_descs.data();
            vertex_input_state.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_attribute_descs.size());
            vertex_input_state.pVertexAttributeDescriptions = vertex_attribute_descs.data();
        }


        // input assembly state
        VkPipelineInputAssemblyStateCreateInfo input_assembly_state = {};
        {
            input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            input_assembly_state.topology = m_state.HasTessellation() ? VK_PRIMITIVE_TOPOLOGY_PATCH_LIST : vulkan_primitive_topology[static_cast<uint32_t>(m_state.primitive_topology)];
            input_assembly_state.primitiveRestartEnable = VK_FALSE;
        }

        //// tessellation state
        //VkPipelineTessellationStateCreateInfo tesselation_state = {};
        //{
        //    tesselation_state.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        //    tesselation_state.patchControlPoints = 3;
        //}

        // Rasterizer state
        VkPipelineRasterizationStateCreateInfo rasterizer_state             = {};
        if (m_state.rasterizer_state)
        {
            rasterizer_state.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer_state.depthClampEnable        = m_state.rasterizer_state->GetDepthClipEnabled();
            rasterizer_state.rasterizerDiscardEnable = VK_FALSE;
            rasterizer_state.polygonMode             = vulkan_polygon_mode[static_cast<uint32_t>(m_state.rasterizer_state->GetPolygonMode())];
            rasterizer_state.lineWidth               = m_state.rasterizer_state->GetLineWidth();
            rasterizer_state.cullMode                = vulkan_cull_mode[static_cast<uint32_t>(m_state.rasterizer_state->GetCullMode())];
            rasterizer_state.frontFace               = VK_FRONT_FACE_CLOCKWISE;
            rasterizer_state.depthBiasEnable         = m_state.rasterizer_state->GetDepthBias() != 0.0f ? VK_TRUE : VK_FALSE;
            rasterizer_state.depthBiasConstantFactor = Math::Helper::Floor(m_state.rasterizer_state->GetDepthBias() * (float)(1 << 24));
            rasterizer_state.depthBiasClamp          = m_state.rasterizer_state->GetDepthBiasClamp();
            rasterizer_state.depthBiasSlopeFactor    = m_state.rasterizer_state->GetDepthBiasSlopeScaled();
        }
        
        // Mutlisampling
        VkPipelineMultisampleStateCreateInfo multisampling_state = {};
        {
            multisampling_state.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling_state.sampleShadingEnable  = VK_FALSE;
            multisampling_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        }
        
        VkPipelineColorBlendStateCreateInfo color_blend_state = {};
        vector<VkPipelineColorBlendAttachmentState> blend_state_attachments;
        if (m_state.blend_state)
        {
            // Blend state attachments
            {
                // Same blend state for all
                VkPipelineColorBlendAttachmentState blend_state_attachment = {};
                blend_state_attachment.colorWriteMask                      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
                blend_state_attachment.blendEnable                         = m_state.blend_state->GetBlendEnabled() ? VK_TRUE : VK_FALSE;
                blend_state_attachment.srcColorBlendFactor                 = vulkan_blend_factor[static_cast<uint32_t>(m_state.blend_state->GetSourceBlend())];
                blend_state_attachment.dstColorBlendFactor                 = vulkan_blend_factor[static_cast<uint32_t>(m_state.blend_state->GetDestBlend())];
                blend_state_attachment.colorBlendOp                        = vulkan_blend_operation[static_cast<uint32_t>(m_state.blend_state->GetBlendOp())];
                blend_state_attachment.srcAlphaBlendFactor                 = vulkan_blend_factor[static_cast<uint32_t>(m_state.blend_state->GetSourceBlendAlpha())];
                blend_state_attachment.dstAlphaBlendFactor                 = vulkan_blend_factor[static_cast<uint32_t>(m_state.blend_state->GetDestBlendAlpha())];
                blend_state_attachment.alphaBlendOp                        = vulkan_blend_operation[static_cast<uint32_t>(m_state.blend_state->GetBlendOpAlpha())];

                // Swapchain
                if (m_state.render_target_swapchain)
                {
                    blend_state_attachments.push_back(blend_state_attachment);
                }

                // Render target(s)
                for (uint8_t i = 0; i < rhi_max_render_target_count; i++)
                {
                    if (m_state.render_target_color_textures[i] != nullptr)
                    {
                        blend_state_attachments.push_back(blend_state_attachment);
                    }
                }
            }
            
            color_blend_state.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            color_blend_state.logicOpEnable     = VK_FALSE;
            color_blend_state.logicOp           = VK_LOGIC_OP_COPY;
            color_blend_state.attachmentCount   = static_cast<uint32_t>(blend_state_attachments.size());
            color_blend_state.pAttachments      = blend_state_attachments.data();
            color_blend_state.blendConstants[0] = m_state.blend_state->GetBlendFactor();
            color_blend_state.blendConstants[1] = m_state.blend_state->GetBlendFactor();
            color_blend_state.blendConstants[2] = m_state.blend_state->GetBlendFactor();
            color_blend_state.blendConstants[3] = m_state.blend_state->GetBlendFactor();
        }
        
        // Depth-stencil state
        VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {};
        if (m_state.depth_stencil_state)
        {
            depth_stencil_state.sType             = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depth_stencil_state.depthTestEnable   = m_state.depth_stencil_state->GetDepthTestEnabled();
            depth_stencil_state.depthWriteEnable  = m_state.depth_stencil_state->GetDepthWriteEnabled();
            depth_stencil_state.depthCompareOp    = vulkan_compare_operator[static_cast<uint32_t>(m_state.depth_stencil_state->GetDepthComparisonFunction())];
            depth_stencil_state.stencilTestEnable = m_state.depth_stencil_state->GetStencilTestEnabled();
            depth_stencil_state.front.compareOp   = vulkan_compare_operator[static_cast<uint32_t>(m_state.depth_stencil_state->GetStencilComparisonFunction())];
            depth_stencil_state.front.failOp      = vulkan_stencil_operation[static_cast<uint32_t>(m_state.depth_stencil_state->GetStencilFailOperation())];
            depth_stencil_state.front.depthFailOp = vulkan_stencil_operation[static_cast<uint32_t>(m_state.depth_stencil_state->GetStencilDepthFailOperation())];
            depth_stencil_state.front.passOp      = vulkan_stencil_operation[static_cast<uint32_t>(m_state.depth_stencil_state->GetStencilPassOperation())];
            depth_stencil_state.front.compareMask = m_state.depth_stencil_state->GetStencilReadMask();
            depth_stencil_state.front.writeMask   = m_state.depth_stencil_state->GetStencilWriteMask();
            depth_stencil_state.front.reference   = 1;
            depth_stencil_state.back              = depth_stencil_state.front;
            depth_stencil_state.minDepthBounds = 1.0f; // functionality of DirectX with reverse-z
            depth_stencil_state.maxDepthBounds = 0.0f; // functionality of DirectX with reverse-z
        }

        // Pipeline
        {
            VkPipeline* pipeline = reinterpret_cast<VkPipeline*>(&m_resource_pipeline);

            if (pipeline_state.IsGraphics())
            {
                // Enable dynamic rendering - VK_KHR_dynamic_rendering.
                // This means no render passes and no frame buffer objects.
                VkPipelineRenderingCreateInfoKHR pipeline_rendering_create_info = {};
                VkPipelineFragmentShadingRateStateCreateInfoKHR fragment_shading_rate_state = {};
                vector<VkFormat> attachment_formats_color;
                VkFormat attachment_format_depth   = VK_FORMAT_UNDEFINED;
                VkFormat attachment_format_stencil = VK_FORMAT_UNDEFINED;
                {
                    // Swapchain buffer as a render target
                    if (m_state.render_target_swapchain)
                    {
                        attachment_formats_color.push_back(vulkan_format[rhi_format_to_index(m_state.render_target_swapchain->GetFormat())]);
                    }
                    else // Regular render target(s)
                    {
                        for (uint32_t i = 0; i < rhi_max_render_target_count; i++)
                        {
                            RHI_Texture* texture = m_state.render_target_color_textures[i];
                            if (texture == nullptr)
                                break;

                            attachment_formats_color.push_back(vulkan_format[rhi_format_to_index(texture->GetFormat())]);
                        }
                    }

                    // Depth
                    if (m_state.render_target_depth_texture)
                    {
                        RHI_Texture* tex_depth    = m_state.render_target_depth_texture;
                        attachment_format_depth   = vulkan_format[rhi_format_to_index(tex_depth->GetFormat())];
                        attachment_format_stencil = tex_depth->IsStencilFormat() ? attachment_format_depth : VK_FORMAT_UNDEFINED;
                    }

                    //// variable rate shading
                    //if (m_state.vrs_input_texture)
                    //{
                    //    fragment_shading_rate_state.sType = VK_STRUCTURE_TYPE_PIPELINE_FRAGMENT_SHADING_RATE_STATE_CREATE_INFO_KHR;
                    //    fragment_shading_rate_state.combinerOps[0] = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_KHR;
                    //    fragment_shading_rate_state.combinerOps[1] = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_KHR;
                    //    fragment_shading_rate_state.fragmentSize = { 1, 1 };

                    //    pipeline_rendering_create_info.pNext = &fragment_shading_rate_state;
                    //}

                    pipeline_rendering_create_info.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
                    pipeline_rendering_create_info.colorAttachmentCount    = static_cast<uint32_t>(attachment_formats_color.size());
                    pipeline_rendering_create_info.pColorAttachmentFormats = attachment_formats_color.data();
                    pipeline_rendering_create_info.depthAttachmentFormat   = attachment_format_depth;
                    pipeline_rendering_create_info.stencilAttachmentFormat = attachment_format_stencil;


                }

                // Describe
                VkGraphicsPipelineCreateInfo pipeline_info = {};
                pipeline_info.pNext                        = &pipeline_rendering_create_info;
                pipeline_info.sType                        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                pipeline_info.stageCount                   = static_cast<uint32_t>(shader_stages.size());
                pipeline_info.pStages                      = shader_stages.data();
                pipeline_info.pVertexInputState            = &vertex_input_state;
                pipeline_info.pInputAssemblyState          = &input_assembly_state;
                // pipeline_info.pTessellationState = m_state.HasTessellation() ? &tesselation_state : nullptr;
                pipeline_info.pTessellationState =nullptr;
                pipeline_info.pDynamicState                = &dynamic_state;
                pipeline_info.pViewportState               = &viewport_state;
                pipeline_info.pRasterizationState          = &rasterizer_state;
                pipeline_info.pMultisampleState            = &multisampling_state;
                pipeline_info.pColorBlendState             = &color_blend_state;
                pipeline_info.pDepthStencilState           = &depth_stencil_state;
                pipeline_info.layout                       = static_cast<VkPipelineLayout>(m_resource_pipeline_layout);
                pipeline_info.renderPass                   = nullptr;
               // pipeline_info.flags = m_state.vrs_input_texture ? VK_PIPELINE_CREATE_RENDERING_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR : 0;
        
                // Create
                LC_VK_ASSERT_MSG(vkCreateGraphicsPipelines(RHI_Context::device, nullptr, 1, &pipeline_info, nullptr, pipeline),
                    "Failed to create graphics pipeline");

                // Disable naming until I can come up with a more meaningful name
                // vulkan_utility::debug::set_name(*pipeline, m_state.pass_name);
            }
            else if (pipeline_state.IsCompute())
            {
                // Describe
                VkComputePipelineCreateInfo pipeline_info = {};
                pipeline_info.sType                       = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
                pipeline_info.layout                      = static_cast<VkPipelineLayout>(m_resource_pipeline_layout);
                pipeline_info.stage                       = shader_stages[0];

                // Create
                LC_VK_ASSERT_MSG(vkCreateComputePipelines(RHI_Context::device, nullptr, 1, &pipeline_info, nullptr, pipeline),
                    "Failed to create compute pipeline");

                // Name
                RHI_Device::SetResourceName(static_cast<void*>(*pipeline), RHI_Resource_Type::Pipeline, pipeline_state.name);
            }
        }
    }
    
    RHI_Pipeline::~RHI_Pipeline()
    {
        RHI_Device::DeletionQueueAdd(RHI_Resource_Type::Pipeline, m_resource_pipeline);
        m_resource_pipeline = nullptr;
        
        RHI_Device::DeletionQueueAdd(RHI_Resource_Type::PipelineLayout, m_resource_pipeline_layout);
        m_resource_pipeline_layout = nullptr;
    }
}
