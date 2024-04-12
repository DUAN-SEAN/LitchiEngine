
//= INCLUDES ============================
#include "Runtime/Core/pch.h"
#include "Runtime/Core/Window/Window.h"
#include "Renderer.h"
#include "Geometry.h"
#include "../Resource/ResourceCache.h"
#include "../RHI/RHI_Texture2D.h"
#include "../RHI/RHI_Texture2DArray.h"
#include "../RHI/RHI_TextureCube.h"
#include "../RHI/RHI_Shader.h"
#include "../RHI/RHI_Sampler.h"
#include "../RHI/RHI_BlendState.h"
#include "../RHI/RHI_ConstantBuffer.h"
#include "../RHI/RHI_RasterizerState.h"
#include "../RHI/RHI_DepthStencilState.h"
#include "../RHI/RHI_StructuredBuffer.h"
#include "../RHI/RHI_VertexBuffer.h"
#include "../RHI/RHI_IndexBuffer.h"
#include "../RHI/RHI_AMD_FidelityFX.h"
#include "../RHI/RHI_Device.h"
#include "../RHI/RHI_CommandPool.h"
#include "Runtime/Core/App/ApplicationBase.h"
//=======================================

//= NAMESPACES ===============
using namespace std;
using namespace LitchiRuntime::Math;
//============================

namespace LitchiRuntime
{
	namespace
	{
		// graphics states
		array<shared_ptr<RHI_RasterizerState>, 5>   rasterizer_states;
		array<shared_ptr<RHI_DepthStencilState>, 5> depth_stencil_states;
		array<shared_ptr<RHI_BlendState>, 3>        blend_states;

		// renderer resources
		array<shared_ptr<RHI_Texture>, static_cast<uint32_t>(Renderer_RenderTarget::max)>       render_targets;
		array<shared_ptr<RHI_Shader>, static_cast<uint32_t>(Renderer_Shader::max)>        shaders;
		array<shared_ptr<RHI_Sampler>, static_cast<uint32_t>(Renderer_Sampler::Max)>        samplers;
		array<shared_ptr<RHI_ConstantBuffer>, static_cast<uint32_t>(Renderer_ConstantBuffer::Max)> constant_buffers;
		array<shared_ptr<RHI_StructuredBuffer>, 2>                                        structured_buffers;

        // asset resources
		array<shared_ptr<RHI_Texture>, 11> standard_textures;
		array<shared_ptr<Mesh>, 7>         standard_meshes;
		shared_ptr<Font>                   standard_font;
		shared_ptr<Material>               standard_material;
	}

	void Renderer::CreateConstantBuffers()
	{
		uint32_t element_count = resources_frame_lifetime;

#define constant_buffer(x) constant_buffers[static_cast<uint8_t>(x)]
		constant_buffer(Renderer_ConstantBuffer::Frame) = make_shared<RHI_ConstantBuffer>(string("frame"));
		constant_buffer(Renderer_ConstantBuffer::Frame)->Create<Cb_Frame>(renderer_path_max * element_count);

		// will be remove
		constant_buffer(Renderer_ConstantBuffer::LightArr) = make_shared<RHI_ConstantBuffer>(string("lightArr"));
		constant_buffer(Renderer_ConstantBuffer::LightArr)->Create<Cb_Light_Arr>(renderer_path_max * element_count);// three view 

		constant_buffer(Renderer_ConstantBuffer::RendererPath) = make_shared<RHI_ConstantBuffer>(string("rendererPath"));
		constant_buffer(Renderer_ConstantBuffer::RendererPath)->Create<Cb_RendererPath>(renderer_path_max * element_count);// three view 
	}

	void Renderer::CreateStructuredBuffers()
	{
		uint32_t element_count = resources_frame_lifetime;

#define structured_buffer(x) structured_buffers[static_cast<uint8_t>(x)]

		// single dispatch downsample buffer
		{
			uint32_t times_used_in_frame = 12; // safe to tweak this, if it's not enough the engine will assert
			uint32_t stride = static_cast<uint32_t>(sizeof(uint32_t));
			structured_buffer(Renderer_StructuredBuffer::Spd) = make_shared<RHI_StructuredBuffer>(stride, element_count * times_used_in_frame, "spd_counter");

			// only needs to be set once, then after each use SPD resets it itself
			uint32_t counter_value = 0;
			structured_buffer(Renderer_StructuredBuffer::Spd)->Update(&counter_value);
		}

		uint32_t stride = static_cast<uint32_t>(sizeof(Cb_Light)) * rhi_max_array_size_lights;
		structured_buffer(Renderer_StructuredBuffer::Lights) = make_shared<RHI_StructuredBuffer>(stride, renderer_path_max, "lights");
	}

	void Renderer::CreateDepthStencilStates()
	{
		RHI_Comparison_Function reverse_z_aware_comp_func = RHI_Comparison_Function::GreaterEqual; // reverse-z

#define depth_stencil_state(x) depth_stencil_states[static_cast<uint8_t>(x)]
		// arguments: depth_test, depth_write, depth_function, stencil_test, stencil_write, stencil_function
		depth_stencil_state(Renderer_DepthStencilState::Off) = make_shared<RHI_DepthStencilState>(false, false, RHI_Comparison_Function::Never, false, false, RHI_Comparison_Function::Never);
		depth_stencil_state(Renderer_DepthStencilState::Depth_read_write_stencil_read) = make_shared<RHI_DepthStencilState>(true, true, reverse_z_aware_comp_func, false, false, RHI_Comparison_Function::Never);
		depth_stencil_state(Renderer_DepthStencilState::Depth_read) = make_shared<RHI_DepthStencilState>(true, false, reverse_z_aware_comp_func, false, false, RHI_Comparison_Function::Never);
		depth_stencil_state(Renderer_DepthStencilState::Stencil_read) = make_shared<RHI_DepthStencilState>(false, false, RHI_Comparison_Function::Never, true, false, RHI_Comparison_Function::Equal);
		depth_stencil_state(Renderer_DepthStencilState::Depth_read_write_stencil_write) = make_shared<RHI_DepthStencilState>(true, true, reverse_z_aware_comp_func, false, true, RHI_Comparison_Function::Always);
	}

	void Renderer::CreateRasterizerStates()
	{
		float depth_bias = Light::GetBias();
		float depth_bias_clamp = 0.0f;
		float depth_bias_slope_scaled = Light::GetBiasSlopeScaled();
		float line_width = 2.0f;

#define rasterizer_state(x) rasterizer_states[static_cast<uint8_t>(x)]
		// cull mode, filled mode, depth clip, scissor, bias, bias clamp, slope scaled bias, line width
		rasterizer_state(Renderer_RasterizerState::Solid_cull_back) = make_shared<RHI_RasterizerState>(RHI_CullMode::Back, RHI_PolygonMode::Solid, true, false, 0.0f, 0.0f, 0.0f, line_width);
		rasterizer_state(Renderer_RasterizerState::Solid_cull_none) = make_shared<RHI_RasterizerState>(RHI_CullMode::None, RHI_PolygonMode::Solid, true, false, 0.0f, 0.0f, 0.0f, line_width);
		rasterizer_state(Renderer_RasterizerState::Wireframe_cull_none) = make_shared<RHI_RasterizerState>(RHI_CullMode::None, RHI_PolygonMode::Wireframe, true, false, 0.0f, 0.0f, 0.0f, line_width);
		rasterizer_state(Renderer_RasterizerState::Light_point_spot) = make_shared<RHI_RasterizerState>(RHI_CullMode::Back, RHI_PolygonMode::Solid, true, false, depth_bias, depth_bias_clamp, depth_bias_slope_scaled, 0.0f);
		rasterizer_state(Renderer_RasterizerState::Light_directional) = make_shared<RHI_RasterizerState>(RHI_CullMode::Back, RHI_PolygonMode::Solid, false, false, depth_bias * 0.1f, depth_bias_clamp, depth_bias_slope_scaled, 0.0f);
	}

	void Renderer::CreateBlendStates()
	{
#define blend_state(x) blend_states[static_cast<uint8_t>(x)]
		// blend_enabled, source_blend, dest_blend, blend_op, source_blend_alpha, dest_blend_alpha, blend_op_alpha, blend_factor
		blend_state(Renderer_BlendState::Disabled) = make_shared<RHI_BlendState>(false);
		blend_state(Renderer_BlendState::Alpha) = make_shared<RHI_BlendState>(true, RHI_Blend::Src_Alpha, RHI_Blend::Inv_Src_Alpha, RHI_Blend_Operation::Add, RHI_Blend::One, RHI_Blend::One, RHI_Blend_Operation::Add, 0.0f);
		blend_state(Renderer_BlendState::Additive) = make_shared<RHI_BlendState>(true, RHI_Blend::One, RHI_Blend::One, RHI_Blend_Operation::Add, RHI_Blend::One, RHI_Blend::One, RHI_Blend_Operation::Add, 1.0f);
	}

	void Renderer::CreateSamplers(const bool create_only_anisotropic /*= false*/)
	{
#define sampler(type, filter_min, filter_mag, filter_mip, address_mode, comparison_func, anisotropy, comparison_enabled, mip_bias) \
        samplers[static_cast<uint8_t>(type)] = make_shared<RHI_Sampler>(filter_min, filter_mag, filter_mip, address_mode, comparison_func, anisotropy, comparison_enabled, mip_bias)

		// non anisotropic
	{
		static bool samplers_created = false;
		if (!samplers_created)
		{
			// note: clamp to border uses black transparent RGBA(0, 0, 0, 0)
			sampler(Renderer_Sampler::Compare_depth, RHI_Filter::Linear, RHI_Filter::Linear, RHI_Filter::Nearest, RHI_Sampler_Address_Mode::ClampToBorder, RHI_Comparison_Function::Greater, 0.0f, true, 0.0f); // reverse-z
			sampler(Renderer_Sampler::Point_clamp_edge, RHI_Filter::Nearest, RHI_Filter::Nearest, RHI_Filter::Nearest, RHI_Sampler_Address_Mode::ClampToEdge, RHI_Comparison_Function::Always, 0.0f, false, 0.0f);
			sampler(Renderer_Sampler::Point_clamp_border, RHI_Filter::Nearest, RHI_Filter::Nearest, RHI_Filter::Nearest, RHI_Sampler_Address_Mode::ClampToBorder, RHI_Comparison_Function::Always, 0.0f, false, 0.0f);
			sampler(Renderer_Sampler::Point_wrap, RHI_Filter::Nearest, RHI_Filter::Nearest, RHI_Filter::Nearest, RHI_Sampler_Address_Mode::Wrap, RHI_Comparison_Function::Always, 0.0f, false, 0.0f);
			sampler(Renderer_Sampler::Bilinear_clamp_edge, RHI_Filter::Linear, RHI_Filter::Linear, RHI_Filter::Nearest, RHI_Sampler_Address_Mode::ClampToEdge, RHI_Comparison_Function::Always, 0.0f, false, 0.0f);
			sampler(Renderer_Sampler::Bilienar_clamp_border, RHI_Filter::Linear, RHI_Filter::Linear, RHI_Filter::Nearest, RHI_Sampler_Address_Mode::ClampToBorder, RHI_Comparison_Function::Always, 0.0f, false, 0.0f);
			sampler(Renderer_Sampler::Bilinear_wrap, RHI_Filter::Linear, RHI_Filter::Linear, RHI_Filter::Nearest, RHI_Sampler_Address_Mode::Wrap, RHI_Comparison_Function::Always, 0.0f, false, 0.0f);
			sampler(Renderer_Sampler::Trilinear_clamp, RHI_Filter::Linear, RHI_Filter::Linear, RHI_Filter::Linear, RHI_Sampler_Address_Mode::ClampToEdge, RHI_Comparison_Function::Always, 0.0f, false, 0.0f);

			samplers_created = true;
		}
	}

	// anisotropic
	{
		// compute mip bias for enhanced texture detail in upsampling, applicable when output resolution is higher than render resolution
		// this adjustment, beneficial even without FSR 2, ensures textures remain detailed at higher output resolutions by applying a negative bias
		float mip_bias_new = 0.0f;
		if (GetResolutionOutput().x > GetResolutionRender().x)
		{
			mip_bias_new = log2(GetResolutionRender().x / GetResolutionOutput().x) - 1.0f;
		}

		static float mip_bias = numeric_limits<float>::max();
		if (mip_bias_new != mip_bias)
		{
			mip_bias = mip_bias_new;
			float anisotropy = GetOption<float>(Renderer_Option::Anisotropy);
			sampler(Renderer_Sampler::Anisotropic_wrap, RHI_Filter::Linear, RHI_Filter::Linear, RHI_Filter::Linear, RHI_Sampler_Address_Mode::Wrap, RHI_Comparison_Function::Always, anisotropy, false, mip_bias);
		}
	}

	RHI_Device::UpdateBindlessResources(&samplers, nullptr);
	}

	void Renderer::CreateRenderTargets(const bool create_render, const bool create_output,  const bool create_dynamic)
	{
		// Get render resolution
		uint32_t width_render = static_cast<uint32_t>(GetResolutionRender().x);
		uint32_t height_render = static_cast<uint32_t>(GetResolutionRender().y);

		// Get output resolution
		uint32_t width_output = static_cast<uint32_t>(GetResolutionOutput().x);
		uint32_t height_output = static_cast<uint32_t>(GetResolutionOutput().y);

		// Deduce how many mips are required to scale down any dimension close to 16px (or exactly)
		uint32_t mip_count = 1;
		uint32_t width = width_render;
		uint32_t height = height_render;
		uint32_t smallest_dimension = 1;
		while (width > smallest_dimension && height > smallest_dimension)
		{
			width /= 2;
			height /= 2;
			mip_count++;
		}

		// Notes.
		// Gbuffer_Normal: Any format with or below 8 bits per channel, will produce banding.
#define render_target(x) render_targets[static_cast<uint8_t>(x)]

		// typical flags
		uint32_t flags_standard = RHI_Texture_Uav | RHI_Texture_Srv;
		uint32_t flags_render_target = flags_standard | RHI_Texture_Rtv;
		uint32_t flags_depth_buffer = RHI_Texture_Rtv | RHI_Texture_Srv;

		// Render resolution
		if (create_render)
		{
			uint32_t frame_render_flags = flags_render_target | RHI_Texture_ClearBlit;
			RHI_Format frame_render_format = RHI_Format::R16G16B16A16_Float;
			render_target(Renderer_RenderTarget::frame_render) = make_unique<RHI_Texture2D>(width_render, height_render, 1, frame_render_format, frame_render_flags, "frame_render");
			render_target(Renderer_RenderTarget::frame_render_2) = make_unique<RHI_Texture2D>(width_render, height_render, 1, frame_render_format, frame_render_flags, "frame_render_2");
		}

		// Output resolution
		if (create_output)
		{
			// frame
			render_target(Renderer_RenderTarget::frame_output) = make_unique<RHI_Texture2D>(width_output, height_output, 1, RHI_Format::R16G16B16A16_Float, flags_render_target | RHI_Texture_ClearBlit, "frame_output");
			render_target(Renderer_RenderTarget::frame_output_2) = make_unique<RHI_Texture2D>(width_output, height_output, 1, RHI_Format::R16G16B16A16_Float, flags_render_target | RHI_Texture_ClearBlit, "frame_output_2");
		}

		RHI_Device::QueueWaitAll();
	}

	void Renderer::CreateShaders()
	{
		const bool async = false;// Ĭ��ʹ��ͬ��
		const string shader_dir = ApplicationBase::Instance()->GetEngineAssetsPath() + "Shaders/";
#define shader(x) shaders[static_cast<uint8_t>(x)]

		// G-Buffer
		shader(Renderer_Shader::forward_v) = make_shared<RHI_Shader>();
		shader(Renderer_Shader::forward_v)->Compile(RHI_Shader_Vertex, shader_dir + "forward.hlsl", async, RHI_Vertex_Type::PosUvNorTan);
		shader(Renderer_Shader::forward_p) = make_shared<RHI_Shader>();
		shader(Renderer_Shader::forward_p)->Compile(RHI_Shader_Pixel, shader_dir + "forward.hlsl", async);

		// SkyBox
		shader(Renderer_Shader::skybox_v) = make_shared<RHI_Shader>();
		shader(Renderer_Shader::skybox_v)->Compile(RHI_Shader_Vertex, shader_dir + "skyBox.hlsl", async, RHI_Vertex_Type::PosUvNorTan);
		shader(Renderer_Shader::skybox_p) = make_shared<RHI_Shader>();
		shader(Renderer_Shader::skybox_p)->Compile(RHI_Shader_Pixel, shader_dir + "skyBox.hlsl", async);

		// Depth light
		{
			shader(Renderer_Shader::depth_light_V) = make_shared<RHI_Shader>();
			shader(Renderer_Shader::depth_light_V)->Compile(RHI_Shader_Vertex, shader_dir + "depth_light.hlsl", async, RHI_Vertex_Type::PosUvNorTan);

			shader(Renderer_Shader::depth_light_p) = make_shared<RHI_Shader>();
			shader(Renderer_Shader::depth_light_p)->Compile(RHI_Shader_Pixel, shader_dir + "depth_light.hlsl", async);

			shader(Renderer_Shader::depth_light_skin_V) = make_shared<RHI_Shader>();
			shader(Renderer_Shader::depth_light_skin_V)->Compile(RHI_Shader_Vertex, shader_dir + "depth_light_skin.hlsl", async, RHI_Vertex_Type::PosUvNorTanBone);

			shader(Renderer_Shader::depth_light_skin_p) = make_shared<RHI_Shader>();
			shader(Renderer_Shader::depth_light_skin_p)->Compile(RHI_Shader_Pixel, shader_dir + "depth_light_skin.hlsl", async);
		}

		// Font
		shader(Renderer_Shader::font_v) = make_shared<RHI_Shader>();
		shader(Renderer_Shader::font_v)->Compile(RHI_Shader_Vertex, shader_dir + "Forward/Standard_Text.hlsl", async, RHI_Vertex_Type::PosUv);
		shader(Renderer_Shader::font_p) = make_shared<RHI_Shader>();
		shader(Renderer_Shader::font_p)->Compile(RHI_Shader_Pixel, shader_dir + "Forward/Standard_Text.hlsl", async);

		// Image
		shader(Renderer_Shader::ui_image_v) = make_shared<RHI_Shader>();
		shader(Renderer_Shader::ui_image_v)->Compile(RHI_Shader_Vertex, shader_dir + "Forward/Standard_Image.hlsl", async, RHI_Vertex_Type::PosUv);
		shader(Renderer_Shader::ui_image_p) = make_shared<RHI_Shader>();
		shader(Renderer_Shader::ui_image_p)->Compile(RHI_Shader_Pixel, shader_dir + "Forward/Standard_Image.hlsl", async);

		// Line
		shader(Renderer_Shader::line_v) = make_shared<RHI_Shader>();
		shader(Renderer_Shader::line_v)->Compile(RHI_Shader_Vertex, shader_dir + "line.hlsl", async, RHI_Vertex_Type::PosCol);
		shader(Renderer_Shader::line_p) = make_shared<RHI_Shader>();
		shader(Renderer_Shader::line_p)->Compile(RHI_Shader_Pixel, shader_dir + "line.hlsl", async);
	}

	void Renderer::CreateFonts()
	{
		// Get standard font directory
		const string dir_font = ApplicationBase::Instance()->GetEngineAssetsPath() + "Fonts/";

		// Load a font (used for performance metrics)
		// m_font = make_unique<Font>(dir_font + "CalibriBold.ttf", static_cast<uint32_t>(13 * Window::GetDpiScale()), Vector4(0.8f, 0.8f, 0.8f, 1.0f));
		standard_font = make_unique<Font>(dir_font + "CalibriBold.ttf", static_cast<uint32_t>(13), Vector4(0.8f, 0.8f, 0.8f, 1.0f));
	}

	void Renderer::CreateStandardMeshes()
	{
		auto create_mesh = [](const Renderer_MeshType type)
			{
				const string project_directory = ApplicationBase::Instance()->GetEngineAssetsPath();
				shared_ptr<Mesh> mesh = make_shared<Mesh>();
				vector<RHI_Vertex_PosTexNorTan> vertices;
				vector<uint32_t> indices;

				if (type == Renderer_MeshType::Cube)
				{
					Geometry::CreateCube(&vertices, &indices);
					mesh->SetResourceFilePath(project_directory + "standard_cube" + EXTENSION_MODEL);
				}
				else if (type == Renderer_MeshType::Quad)
				{
					Geometry::CreateQuad(&vertices, &indices);
					mesh->SetResourceFilePath(project_directory + "standard_quad" + EXTENSION_MODEL);
				}
				else if (type == Renderer_MeshType::Grid)
				{
					uint32_t resolution = 512; // a high number here can kill performance in the forest scene
					Geometry::CreateGrid(&vertices, &indices, resolution);
					mesh->SetResourceFilePath(project_directory + "standard_grid" + EXTENSION_MODEL);
				}
				else if (type == Renderer_MeshType::Sphere)
				{
					Geometry::CreateSphere(&vertices, &indices);
					mesh->SetResourceFilePath(project_directory + "standard_sphere" + EXTENSION_MODEL);
				}
				else if (type == Renderer_MeshType::Cylinder)
				{
					Geometry::CreateCylinder(&vertices, &indices);
					mesh->SetResourceFilePath(project_directory + "standard_cylinder" + EXTENSION_MODEL);
				}
				else if (type == Renderer_MeshType::Cone)
				{
					Geometry::CreateCone(&vertices, &indices);
					mesh->SetResourceFilePath(project_directory + "standard_cone" + EXTENSION_MODEL);
				}

				mesh->AddIndices(indices);
				mesh->AddVertices(vertices);
				mesh->ComputeAabb();
				mesh->ComputeNormalizedScale();
				mesh->CreateGpuBuffers();

				standard_meshes[static_cast<uint8_t>(type)] = mesh;
			};

		create_mesh(Renderer_MeshType::Cube);
		create_mesh(Renderer_MeshType::Quad);
		create_mesh(Renderer_MeshType::Sphere);
		create_mesh(Renderer_MeshType::Cylinder);
		create_mesh(Renderer_MeshType::Cone);
		create_mesh(Renderer_MeshType::Grid);

		// this buffers holds all debug primitives that can be drawn
		m_vertex_buffer_lines = make_shared<RHI_VertexBuffer>(true, "lines");
	}

	void Renderer::CreateStandardTextures()
	{
		const string dir_texture = ApplicationBase::Instance()->GetEngineAssetsPath() + "Textures/";
#define standard_texture(x) standard_textures[static_cast<uint8_t>(x)]

		// Noise textures
		{
			standard_texture(Renderer_StandardTexture::Noise_normal) = make_shared<RHI_Texture2D>(RHI_Texture_Srv, "standard_noise_normal");
			standard_texture(Renderer_StandardTexture::Noise_normal)->LoadFromFile(dir_texture + "noise_normal.png");

			standard_texture(Renderer_StandardTexture::Noise_blue) = static_pointer_cast<RHI_Texture>(make_shared<RHI_Texture2DArray>(RHI_Texture_Srv, "standard_noise_blue"));
			standard_texture(Renderer_StandardTexture::Noise_blue)->LoadFromFile(dir_texture + "noise_blue_0.png");
		}

		// Color textures
		{
			standard_texture(Renderer_StandardTexture::White) = make_shared<RHI_Texture2D>(RHI_Texture_Srv, "standard_white");
			standard_texture(Renderer_StandardTexture::White)->LoadFromFile(dir_texture + "white.png");

			standard_texture(Renderer_StandardTexture::Black) = make_shared<RHI_Texture2D>(RHI_Texture_Srv, "standard_black");
			standard_texture(Renderer_StandardTexture::Black)->LoadFromFile(dir_texture + "black.png");

			standard_texture(Renderer_StandardTexture::Transparent) = make_shared<RHI_Texture2D>(RHI_Texture_Srv, "standard_transparent");
			standard_texture(Renderer_StandardTexture::Transparent)->LoadFromFile(dir_texture + "transparent.png");

			standard_texture(Renderer_StandardTexture::Checkerboard) = make_shared<RHI_Texture2D>(RHI_Texture_Srv, "standard_transparent");
			standard_texture(Renderer_StandardTexture::Checkerboard)->LoadFromFile(dir_texture + "no_texture.png");
		}

		// Gizmo icons
		{
			standard_texture(Renderer_StandardTexture::Gizmo_light_directional) = make_shared<RHI_Texture2D>(RHI_Texture_Srv, "standard_icon_light_directional");
			standard_texture(Renderer_StandardTexture::Gizmo_light_directional)->LoadFromFile(dir_texture + "sun.png");

			standard_texture(Renderer_StandardTexture::Gizmo_light_point) = make_shared<RHI_Texture2D>(RHI_Texture_Srv, "standard_icon_light_point");
			standard_texture(Renderer_StandardTexture::Gizmo_light_point)->LoadFromFile(dir_texture + "light_bulb.png");

			standard_texture(Renderer_StandardTexture::Gizmo_light_spot) = make_shared<RHI_Texture2D>(RHI_Texture_Srv, "standard_icon_light_spot");
			standard_texture(Renderer_StandardTexture::Gizmo_light_spot)->LoadFromFile(dir_texture + "flashlight.png");

			standard_texture(Renderer_StandardTexture::Gizmo_audio_source) = make_shared<RHI_Texture2D>(RHI_Texture_Srv, "standard_icon_audio_source");
			standard_texture(Renderer_StandardTexture::Gizmo_audio_source)->LoadFromFile(dir_texture + "audio.png");
		}

		// Sky Box
		{
			standard_texture(Renderer_StandardTexture::SkyBox) = make_shared<RHI_TextureCube>(RHI_Texture_Srv, "standard_skyBox");
			standard_texture(Renderer_StandardTexture::SkyBox)->LoadFromFile(dir_texture + "skyBox_texture_0.png");
		}
	}

	void Renderer::CreateSkyBoxMesh() {
		// Create CubeMesh for skyBox
		std::vector<RHI_Vertex_PosTexNorTan> vertices;
		std::vector<uint32_t> indices;
		Geometry::CreateCube(&vertices, &indices, 8);

		m_vertex_buffer_skyBox = make_shared<RHI_VertexBuffer>(true, "skyBox");
		m_index_buffer_skyBox = make_shared<RHI_IndexBuffer>(true, "skyBox");

		m_vertex_buffer_skyBox->Create(vertices);
		m_index_buffer_skyBox->Create(indices);
	}

	void Renderer::LoadDefaultMaterials()
	{
		m_default_standard_material = ApplicationBase::Instance()->materialManager->LoadResource(":Materials/Standard.mat");
		m_default_standard_skin_material = ApplicationBase::Instance()->materialManager->LoadResource(":Materials/StandardSkin.mat");
	}

	void Renderer::DestroyResources()
	{
		render_targets.fill(nullptr);
		shaders.fill(nullptr);
		samplers.fill(nullptr);
		standard_textures.fill(nullptr);
		standard_meshes.fill(nullptr);
		structured_buffers.fill(nullptr);
		constant_buffers.fill(nullptr);
		standard_font = nullptr;
		standard_material = nullptr;
		m_vertex_buffer_skyBox = nullptr;// todo 
		m_index_buffer_skyBox = nullptr;
	}

	array<shared_ptr<RHI_Texture>, static_cast<uint32_t>(Renderer_RenderTarget::max)>& Renderer::GetRenderTargets()
	{
		return render_targets;
	}

	array<shared_ptr<RHI_Shader>, static_cast<uint32_t>(Renderer_Shader::max)>& Renderer::GetShaders()
	{
		return shaders;
	}

	array<shared_ptr<RHI_ConstantBuffer>, static_cast<uint32_t>(Renderer_ConstantBuffer::Max)>& Renderer::GetConstantBuffers()
	{
		return constant_buffers;
	}

	array<shared_ptr<RHI_StructuredBuffer>, 2>& Renderer::GetStructuredBuffers()
	{
		return structured_buffers;
	}

	shared_ptr<RHI_RasterizerState> Renderer::GetRasterizerState(const Renderer_RasterizerState type)
	{
		return rasterizer_states[static_cast<uint8_t>(type)];
	}

	shared_ptr<RHI_DepthStencilState> Renderer::GetDepthStencilState(const Renderer_DepthStencilState type)
	{
		return depth_stencil_states[static_cast<uint8_t>(type)];
	}

	shared_ptr<RHI_BlendState> Renderer::GetBlendState(const Renderer_BlendState type)
	{
		return blend_states[static_cast<uint8_t>(type)];
	}

	shared_ptr<RHI_Texture> Renderer::GetRenderTarget(const Renderer_RenderTarget type)
	{
		return render_targets[static_cast<uint8_t>(type)];
	}

	shared_ptr<RHI_Shader> Renderer::GetShader(const Renderer_Shader type)
	{
		return shaders[static_cast<uint8_t>(type)];
	}

	shared_ptr<RHI_Sampler> Renderer::GetSampler(const Renderer_Sampler type)
	{
		return samplers[static_cast<uint8_t>(type)];
	}

	shared_ptr<RHI_ConstantBuffer> Renderer::GetConstantBuffer(const Renderer_ConstantBuffer type)
	{
		return constant_buffers[static_cast<uint8_t>(type)];
	}

	shared_ptr<RHI_StructuredBuffer> Renderer::GetStructuredBuffer(const Renderer_StructuredBuffer type)
	{
		return structured_buffers[static_cast<uint8_t>(type)];
	}

	shared_ptr<RHI_Texture> Renderer::GetStandardTexture(const Renderer_StandardTexture type)
	{
		return standard_textures[static_cast<uint8_t>(type)];
	}

	shared_ptr<Mesh> Renderer::GetStandardMesh(const Renderer_MeshType type)
	{
		return standard_meshes[static_cast<uint8_t>(type)];
	}

}
