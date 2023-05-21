
#ifndef UNTITLED_MESH_RENDERER_H
#define UNTITLED_MESH_RENDERER_H

#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/Resources/Material.h"

namespace LitchiRuntime
{
	class RenderCamera;
	class Material;
	class MeshFilter;
	class Texture2D;
	class MeshRenderer :public Component {
	public:
		MeshRenderer();
		~MeshRenderer();

		void SetMaterial(Material* material);//设置Material
		Material* material() { return material_; }

		void Render(RenderCamera* renderCamera);//渲染




	public:

		void PostResourceLoaded() override;

		RTTR_ENABLE()
	private:
		Material* material_;

		unsigned int vertex_buffer_object_ = 0;//顶点缓冲区对象
		unsigned int element_buffer_object_ = 0;//索引缓冲区对象
		unsigned int vertex_array_object_ = 0;//顶点数组对象

	public:
		LitchiRuntime::Resource::Material* m_material;

		/**
		 * \brief 材质名称
		 */
		std::string material_path;
	};
}


#endif //UNTITLED_MESH_RENDERER_H
