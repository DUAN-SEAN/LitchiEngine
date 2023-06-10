
#ifndef UNTITLED_MESH_RENDERER_H
#define UNTITLED_MESH_RENDERER_H

#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/Resources/Material.h"

namespace LitchiRuntime
{
	class RenderCamera;
	class MeshFilter;
	class Texture2D;
	class MeshRenderer :public Component {
	public:
		MeshRenderer();
		~MeshRenderer();

		void SetMaterial(Resource::Material* material);//设置Material
		Resource::Material* material() { return material_; }

		// void RenderOld(RenderCamera* renderCamera);//渲染
		void Render(RenderCamera* renderCamera);

		void RenderShadowMap();
	public:
		void PostResourceLoaded() override;
		void PostResourceModify() override;

		RTTR_ENABLE(Component)
	private:
		unsigned int vertex_buffer_object_ = 0;//顶点缓冲区对象
		unsigned int element_buffer_object_ = 0;//索引缓冲区对象
		unsigned int vertex_array_object_ = 0;//顶点数组对象

	public:
		LitchiRuntime::Resource::Material* material_;

		/**
		 * \brief 材质名称
		 */
		std::string material_path;
	};
}


#endif //UNTITLED_MESH_RENDERER_H
