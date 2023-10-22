
#pragma once

#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/Rendering//Material.h"

namespace LitchiRuntime
{
	class RenderCamera;
	class MeshFilter;
	class MeshRenderer :public Component {
	public:
		MeshRenderer();
		~MeshRenderer();

		void SetMaterial(Material* material);//设置Material
		Material* GetMaterial() { return material; }

		// void RenderOld(RenderCamera* renderCamera);//渲染
		// virtual void Render(RenderCamera* renderCamera, glm::mat4 const* lightVPMat, Framebuffer4Depth* shadowMapFBO) ;

		virtual void RenderShadowMap();
	public:
		void PostResourceLoaded() override;
		void PostResourceModify() override;

		RTTR_ENABLE(Component)
	private:
		unsigned int vertex_buffer_object_ = 0;//顶点缓冲区对象
		unsigned int element_buffer_object_ = 0;//索引缓冲区对象
		unsigned int vertex_array_object_ = 0;//顶点数组对象

	public:
		LitchiRuntime::Material* material;

		/**
		 * \brief 材质名称
		 */
		std::string materialPath;
	};
}
