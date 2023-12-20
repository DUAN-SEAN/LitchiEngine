
#pragma once

#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/Rendering//Material.h"
#include "Runtime/Function/UI/ImGui/Extensions/ImGuiExtension.h"

namespace LitchiRuntime
{
	class RenderCamera;
	class MeshFilter;
	class MeshRenderer :public Component {
	public:
		MeshRenderer();
		~MeshRenderer();


		// void RenderOld(RenderCamera* renderCamera);//渲染
		// virtual void Render(RenderCamera* renderCamera, Matrix const* lightVPMat, Framebuffer4Depth* shadowMapFBO) ;
		void Update() override;
	public:
		void PostResourceLoaded() override;
		void PostResourceModify() override;

		RTTR_ENABLE(Component)

	public:
	

		//= MATERIAL ====================================================================
		// Sets a material from memory (adds it to the resource cache by default)
		Material* SetMaterial(Material* material);

		// Loads a material and the sets it
		Material* SetMaterial(const std::string& file_path);

		void SetDefaultMaterial();
		std::string GetMaterialName() const;
		Material* GetMaterial()       const { return m_material; }
		auto HasMaterial()            const { return m_material != nullptr; }
		//===============================================================================

		// Shadows
		void SetCastShadows(const bool cast_shadows) { m_cast_shadows = cast_shadows; }
		auto GetCastShadows() const { return m_cast_shadows; }
	private:
		unsigned int vertex_buffer_object_ = 0;//顶点缓冲区对象
		unsigned int element_buffer_object_ = 0;//索引缓冲区对象
		unsigned int vertex_array_object_ = 0;//顶点数组对象

	public:

		/**
		 * \brief 材质名称
		 */
		std::string materialPath;
	protected:

		// material
		bool m_material_default = false;
		Material* m_material = nullptr;

		bool m_cast_shadows = true;
	};
}
