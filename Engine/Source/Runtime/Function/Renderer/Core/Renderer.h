
#pragma once

#include <optional>

#include "Runtime/Function/Renderer/Driver.h"
#include "Runtime/Function/Renderer/Framebuffer4Depth.h"
#include "Runtime/Function/Renderer/RenderCamera.h"
#include "Runtime/Function/Renderer/Resources/Material.h"
#include "Runtime/Function/Renderer/Resources/Shader.h"
#include "Runtime/Function/Renderer/Resources/Model.h"
#include "Runtime/Function/Renderer/Settings/EComparaisonAlgorithm.h"
#include "Runtime/Function/Renderer/Settings/EPrimitiveMode.h"
#include "Runtime/Function/Renderer/Settings/ERasterizationMode.h"
#include "Runtime/Function/Renderer/Settings/EComparaisonAlgorithm.h"
#include "Runtime/Function/Renderer/Settings/EOperation.h"
#include "Runtime/Function/Renderer/Settings/ECullFace.h"
#include "Runtime/Function/Renderer/Settings/ECullingOptions.h"
#include "Runtime/Function/Renderer/Settings/EPixelDataFormat.h"
#include "Runtime/Function/Renderer/Settings/EPixelDataType.h"
#include "Runtime/Function/Renderer/Settings/ERenderingCapability.h"

namespace LitchiRuntime
{
	/**
	* The renderer is one of the main classes of OvRendering. It is necessary to have one to
	* send batches to the GPU
	*/
	class Renderer
	{
	public:
		/**
		* Contains some information about an elapsed frame
		*/
		struct FrameInfo
		{
			uint64_t batchCount		= 0;
			uint64_t instanceCount	= 0;
			uint64_t polyCount		= 0;
		};

		/**
		* Constructor of the Renderer
		* @param p_driver
		*/
		Renderer(LitchiRuntime::Driver& p_driver);

		/**
		* Constructor of the Renderer
		*/
		~Renderer() = default;

		/**
		* Define a clear color. This color will be use for the next Renderer::Clear() call
		* @param p_red
		* @param p_green
		* @param p_blue
		* @param p_alpha
		*/
		void SetClearColor(float p_red, float p_green, float p_blue, float p_alpha = 1.0f);

		/**
		* Clear the screen using the previously defined clear color (With Renderer::SetClearColor()) or by
		* using the OpenGL default one.
		* @param p_colorBuffer
		* @param p_depthBuffer
		* @param p_stencilBuffer
		*/
		void Clear(bool p_colorBuffer = true, bool p_depthBuffer = true, bool p_stencilBuffer = true);

		/**
		* Clear the screen using the camera clear color
		* @param p_camera
		* @param p_colorBuffer
		* @param p_depthBuffer
		* @param p_stencilBuffer
		*/
		void Clear(LitchiRuntime::RenderCamera& p_camera, bool p_colorBuffer = true, bool p_depthBuffer = true, bool p_stencilBuffer = true);

		/**
		* Defines the rasterization lines width
		* @param p_width
		*/
		void SetRasterizationLinesWidth(float p_width);

		/**
		* Defines the rasterization mode to use
		* @param p_rasterizationMode
		*/
		void SetRasterizationMode(LitchiRuntime::ERasterizationMode p_rasterizationMode);
		
		/**
		* Enable or disable an OpenGL capability
		* @param p_capability
		* @param p_value
		*/
		void SetCapability(LitchiRuntime::ERenderingCapability p_capability, bool p_value);

		/**
		* Returns true if the given OpenGL capability is enabled
		* @param p_capability
		*/
		bool GetCapability(LitchiRuntime::ERenderingCapability p_capability) const;

		/**
		* Defines the comparaison algorithm to use for the stencil buffer test
		* @param p_algorithm
		* @param p_reference
		* @param p_mask
		*/
		void SetStencilAlgorithm(LitchiRuntime::EComparaisonAlgorithm p_algorithm, int32_t p_reference, uint32_t p_mask);

		/**
		* Defines the comparaison algorithm to use for the depth buffer test
		* @param p_algorithm
		*/
		void SetDepthAlgorithm(LitchiRuntime::EComparaisonAlgorithm p_algorithm);

		/**
		* Control the front and back writing of individual bits in the stencil planes
		* @parma p_mask
		*/
		void SetStencilMask(uint32_t p_mask);

		/**
		* Defines the stencil test operations
		* @param p_stencilFail : Specifies the action to take when the stencil test fails
		* @param p_depthFail : Specifies the stencil action when the stencil test passes, but the depth test fails. 
		* @param p_bothPass : Specifies the stencil action when both the stencil test and the depth test pass, or when the stencil test passes and either there is no depth buffer or depth testing is not enabled.
		*/
		void SetStencilOperations(LitchiRuntime::EOperation p_stencilFail = LitchiRuntime::EOperation::KEEP, LitchiRuntime::EOperation p_depthFail = LitchiRuntime::EOperation::KEEP, LitchiRuntime::EOperation p_bothPass = LitchiRuntime::EOperation::KEEP);

		/**
		* Defines the culling faces
		* @param p_cullface
		*/
		void SetCullFace(LitchiRuntime::ECullFace p_cullFace);

		/**
		* Enable or disable writing into the depth buffer
		* @param p_enable
		*/
		void SetDepthWriting(bool p_enable);

		/**
		* Enable and disable writing color individual components into the frame buffer
		* @param p_enableRed
		* @param p_enableGreen
		* @param p_enableBlue
		* @param p_enableAlpha
		*/
		void SetColorWriting(bool p_enableRed, bool p_enableGreen, bool p_enableBlue, bool p_enableAlpha);

		/**
		* Enable and disable writing color into the frame buffer (Equivalent to Renderer::SetColorWriting(p_enable, p_enable, p_enable, p_enable))
		* @param p_enable
		*/
		void SetColorWriting(bool p_enable);


		/**
		 * Set the viewport parameters.
		 * @param x
		 * @param y
		 * @param width
		 * @param height
		 */
		void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		/**
		 * Read a block of pixels from the frame buffer.
		 * @param x
		 * @param y
		 * @param width
		 * @param height
		 * @param format
		 * @param type
		 * @param data
		 */
		void ReadPixels(uint32_t x, uint32_t y, uint32_t width, uint32_t height, LitchiRuntime::EPixelDataFormat format,LitchiRuntime::EPixelDataType type, void* data);

		/**
		* Return the value associated to the given GLenum
		* @param p_parameter
		*/
		bool GetBool(GLenum p_parameter);

		/**
		* Return the value associated to the given GLenum at the given index.
		* @param p_parameter
		* @param p_index
		*/
		bool GetBool(GLenum p_parameter, uint32_t p_index);

		/**
		* Return the value associated to the given GLenum
		* @param p_parameter
		*/
		int GetInt(GLenum p_parameter);

		/**
		* Return the value associated to the given GLenum at the given index.
		* @param p_parameter
		* @param p_index
		*/
		int GetInt(GLenum p_parameter, uint32_t p_index);

		/**
		* Return the value associated to the given GLenum
		* @param p_parameter
		*/
		float GetFloat(GLenum p_parameter);

		/**
		* Return the value associated to the given GLenum at the given index.
		* @param p_parameter
		* @param p_index
		*/
		float GetFloat(GLenum p_parameter, uint32_t p_index);

		/**
		* Return the value associated to the given GLenum
		* @param p_parameter
		*/
		double GetDouble(GLenum p_parameter);

		/**
		* Return the value associated to the given GLenum at the given index.
		* @param p_parameter
		* @param p_index
		*/
		double GetDouble(GLenum p_parameter, uint32_t p_index);

		/**
		* Return the value associated to the given GLenum
		* @param p_parameter
		*/
		int64_t GetInt64(GLenum p_parameter);

		/**
		* Return the value associated to the given GLenum at the given index.
		* @param p_parameter
		* @param p_index
		*/
		int64_t GetInt64(GLenum p_parameter, uint32_t p_index);

		/**
		* Return the value associated to the given GLenum
		* @param p_parameter
		*/
		std::string GetString(GLenum p_parameter);

		/**
		* Return the value associated to the given GLenum at the given index.
		* @param p_parameter
		* @param p_index
		*/
		std::string GetString(GLenum p_parameter, uint32_t p_index);

		/**
		* Clear frame informations
		*/
		void ClearFrameInfo();


		/**
		* Draw the model using the given material (The material will be applied to every submeshes of the the model)
		* @param p_model
		* @param p_material
		* @param p_modelMatrix
		* @param p_defaultMaterial (Used if the given material has no shader attached)
		*/
		void DrawModelWithSingleMaterial(Model& p_model, Resource::Material& p_material, glm::mat4 const* p_modelMatrix, Resource::Material* p_defaultMaterial = nullptr);

		/**
		* Draw the model using the given materials
		* @param p_model
		* @param p_modelMatrix
		* @param p_defaultMaterial (Used when a submesh material has no shader attached)
		*/
		void DrawModelWithMaterials(Model& p_model, std::vector<Resource::Material*> p_materials, glm::mat4 const* p_modelMatrix, Resource::Material* p_defaultMaterial = nullptr);

		/**
		*  Draw Mesh
		*  @param p_mesh
		*  @param p_material
		*  @param p_modelMatrix
		*/
		void DrawMesh(IMesh& p_mesh, Resource::Material& p_material, glm::mat4 const* p_modelMatrix);
		void DrawMesh(IMesh& p_mesh, Resource::Material& p_material, glm::mat4 const* p_modelMatrix, glm::mat4 const* lightVPMat, Framebuffer4Depth* shadowMapFBO);

		/**
		* Draw the given model the amount of times passed in parameter (Instance rendering if p_instance > 1)
		* @param p_mesh
		* @param p_primitiveMode
		* @param p_instances
		*/
		void Draw(LitchiRuntime::IMesh& p_mesh, LitchiRuntime::EPrimitiveMode p_primitiveMode = LitchiRuntime::EPrimitiveMode::TRIANGLES, uint32_t p_instances = 1);

		///**
		//* Returns the list of meshes from a model that should be rendered
		//* @param p_model
		//* @param p_modelBoundingSphere
		//* @param p_modelTransform
		//* @param p_frustum
		//* @param p_cullingOptions
		//*/
		//std::vector<std::reference_wrapper<LitchiRuntime::Mesh>> GetMeshesInFrustum
		//(
		//	const LitchiRuntime::Model& p_model,
		//	const BoundingSphere& p_modelBoundingSphere,
		//	const OvMaths::FTransform& p_modelTransform,
		//	const Data::Frustum& p_frustum,
		//	LitchiRuntime::ECullingOptions p_cullingOptions
		//);

		/**
		* Fetch and returns the actual OpenGL state
		*/
		uint8_t FetchGLState();

		/**
		* Apply mask to modify OpenGL state
		* @param p_mask
		*/
		void ApplyStateMask(uint8_t p_mask);

		/**
		* Store the given state in the renderer
		* @param p_state
		*/
		void SetState(uint8_t p_state);

		/**
		* Returns the FrameInfo structure
		*/
		const FrameInfo& GetFrameInfo() const;

	private:
		Driver&	m_driver;
		FrameInfo			m_frameInfo;
		uint8_t				m_state;

		Texture* m_emptyTexture = nullptr;
	};
}
