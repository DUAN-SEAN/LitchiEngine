
#include <glad/glad.h>
#include "mesh_renderer.h"
#include <rttr/registration>
#include <gtx/transform2.hpp>
#include "mesh_filter.h"
#include "gtx/quaternion.hpp"
#include "Runtime/Core/App/application_base.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Renderer/gpu_resource_mapper.h"
#include "Runtime/Function/Renderer/render_camera.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_filter.h"

namespace LitchiRuntime
{
	MeshRenderer::MeshRenderer() {

	}

	MeshRenderer::~MeshRenderer() {

	}

	void MeshRenderer::SetMaterial(Resource::Material* material) {
		material_ = material;
	}

	//void MeshRenderer::RenderOld(RenderCamera* renderCamera) {

	//	//从当前Camera获取View Projection
	//	auto current_camera = renderCamera;
	//	if (current_camera == nullptr) {
	//		return;
	//	}

	//	////判断相机的 culling_mask 是否包含当前物体 layer
	//	//if ((current_camera->culling_mask() & game_object()->layer()) == 0x00) {
	//	//    return;
	//	//}

	//	glm::mat4 view = current_camera->view_mat4();
	//	glm::mat4 projection = current_camera->projection_mat4();

	//	//主动获取 Transform 组件，计算mvp。
	//	auto transform = game_object()->GetComponent<Transform>();
	//	if (!transform) {
	//		return;
	//	}
	//	glm::mat4 trans = glm::translate(transform->position());
	//	glm::mat4 rotation = glm::toMat4(transform->rotation());
	//	glm::mat4 scale = glm::scale(transform->scale()); //缩放;
	//	glm::mat4 model = trans * scale * rotation;// 旋转->缩放->平移
	//	glm::mat4 mvp = projection * view * model;// 模型2世界->世界2相机->相机2投影

	//	//主动获取 MeshFilter 组件
	//	auto mesh_filter = game_object()->GetComponent<MeshFilter>();
	//	if (!mesh_filter) {
	//		return;
	//	}

	//	//获取`Shader`的`gl_program_id`，指定为目标Shader程序。
	//	GLuint gl_program_id = material_->shader()->gl_program_id();

	//	if (vertex_array_object_ == 0) {
	//		GLint vpos_location = glGetAttribLocation(gl_program_id, "a_pos");
	//		GLint vcol_location = glGetAttribLocation(gl_program_id, "a_color");
	//		GLint a_uv_location = glGetAttribLocation(gl_program_id, "a_uv");

	//		//在GPU上创建缓冲区对象
	//		glGenBuffers(1, &vertex_buffer_object_);
	//		//将缓冲区对象指定为顶点缓冲区对象
	//		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
	//		//上传顶点数据到缓冲区对象
	//		glBufferData(GL_ARRAY_BUFFER, mesh_filter->mesh()->vertex_num_ * sizeof(MeshFilter::Vertex), mesh_filter->mesh()->vertex_data_, GL_STATIC_DRAW);

	//		//在GPU上创建缓冲区对象
	//		glGenBuffers(1, &element_buffer_object_);
	//		//将缓冲区对象指定为顶点索引缓冲区对象
	//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_);
	//		//上传顶点索引数据到缓冲区对象
	//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_filter->mesh()->vertex_index_num_ * sizeof(unsigned short), mesh_filter->mesh()->vertex_index_data_, GL_STATIC_DRAW);

	//		glGenVertexArrays(1, &vertex_array_object_);

	//		//设置VAO
	//		glBindVertexArray(vertex_array_object_);
	//		{
	//			//指定当前使用的VBO
	//			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
	//			//将Shader变量(a_pos)和顶点坐标VBO句柄进行关联，最后的0表示数据偏移量。
	//			glVertexAttribPointer(vpos_location, 3, GL_FLOAT, false, sizeof(MeshFilter::Vertex), 0);
	//			//启用顶点Shader属性(a_color)，指定与顶点颜色数据进行关联
	//			glVertexAttribPointer(vcol_location, 4, GL_FLOAT, false, sizeof(MeshFilter::Vertex), (void*)(sizeof(float) * 3));
	//			//将Shader变量(a_uv)和顶点UV坐标VBO句柄进行关联，最后的0表示数据偏移量。
	//			glVertexAttribPointer(a_uv_location, 2, GL_FLOAT, false, sizeof(MeshFilter::Vertex), (void*)(sizeof(float) * (3 + 4)));

	//			glEnableVertexAttribArray(vpos_location);
	//			glEnableVertexAttribArray(vcol_location);
	//			glEnableVertexAttribArray(a_uv_location);

	//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_);
	//		}
	//		glBindBuffer(GL_ARRAY_BUFFER, 0);
	//	}

	//	glUseProgram(gl_program_id);
	//	{
	//		glEnable(GL_DEPTH_TEST);
	//		glEnable(GL_CULL_FACE);//开启背面剔除
	//		//上传mvp矩阵
	//		glUniformMatrix4fv(glGetUniformLocation(gl_program_id, "u_mvp"), 1, GL_FALSE, &mvp[0][0]);

	//		//从Pass节点拿到保存的Texture
	//		std::vector<std::pair<std::string, Texture2D*>> textures = material_->textures();
	//		for (int texture_index = 0; texture_index < textures.size(); ++texture_index) {
	//			GLint u_texture_location = glGetUniformLocation(gl_program_id, textures[texture_index].first.c_str());
	//			//激活纹理单元0
	//			glActiveTexture(GL_TEXTURE0 + texture_index);
	//			//将加载的图片纹理句柄，绑定到纹理单元0的Texture2D上。

	//			glBindTexture(GL_TEXTURE_2D, GPUResourceMapper::GetTexture(textures[texture_index].second->texture_handle()));
	//			//设置Shader程序从纹理单元0读取颜色数据
	//			glUniform1i(u_texture_location, texture_index);
	//		}

	//		glBindVertexArray(vertex_array_object_);
	//		{
	//			glDrawElements(GL_TRIANGLES, mesh_filter->mesh()->vertex_index_num_, GL_UNSIGNED_SHORT, 0);//使用顶点索引进行绘制，最后的0表示数据偏移量。
	//		}
	//		glBindVertexArray(0);
	//	}

	//}

	void MeshRenderer::Render(RenderCamera* renderCamera, glm::mat4 const* lightVPMat, Framebuffer4Depth* shadowMapFBO)
	{
		// 通过MeshFilter获取当前的Mesh
		auto meshFilter = game_object()->GetComponent<MeshFilter>();
		if(meshFilter==nullptr)
		{
			return;
		}
		auto model = meshFilter->GetModel();
		if(model == nullptr)
		{
			return;
		}

		if (model->GetMeshes().size() <= meshFilter->GetMeshIndex())
		{
			return;
		}

		auto mesh = model->GetMeshes().at(meshFilter->GetMeshIndex());
		if(mesh == nullptr)
		{
			return;
		}

		// 计算模型矩阵
		auto transform = game_object()->GetComponent<Transform>();
		glm::mat4 modelMatrix = transform->GetWorldMatrix();// 旋转->缩放->平移 TRS

		// vp矩阵绑定？

		// 调用Draw执行绘制
		// 内部将设置模型矩阵到UBO中
		ApplicationBase::Instance()->renderer->DrawMesh(*mesh, *material_, &modelMatrix, lightVPMat,shadowMapFBO);

	}

	void MeshRenderer::RenderShadowMap()
	{
		// 通过MeshFilter获取当前的Mesh
		auto meshFilter = game_object()->GetComponent<MeshFilter>();
		if (meshFilter == nullptr)
		{
			return;
		}
		auto model = meshFilter->GetModel();
		if (model == nullptr)
		{
			return;
		}

		if(model->GetMeshes().size() <= meshFilter->GetMeshIndex())
		{
			return;
		}

		auto mesh = model->GetMeshes().at(meshFilter->GetMeshIndex());
		if (mesh == nullptr)
		{
			return;
		}

		// 计算模型矩阵
		auto transform = game_object()->GetComponent<Transform>();
		glm::mat4 modelMatrix = transform->GetWorldMatrix();// 旋转->缩放->平移 TRS
		
		ApplicationBase::Instance()->engineUBO->SetSubData(modelMatrix, 0);

		// 调用Draw执行绘制
		// 内部将设置模型矩阵到UBO中
		ApplicationBase::Instance()->renderer->Draw(*mesh);
	}

	void MeshRenderer::PostResourceLoaded()
	{
		DEBUG_LOG_INFO("PostResourceLoaded Path {}", material_path);

		if(material_path.empty())
		{
			return;
		}

		material_ = ApplicationBase::Instance()->materialManager->GetResource(material_path);
		if(material_ != nullptr)
		{
			material_->PostResourceLoaded();
		}
	}
	void MeshRenderer::PostResourceModify()
	{
		DEBUG_LOG_INFO("PostResourceModify Path {}", material_path);

		if (material_path.empty())
		{
			return;
		}

		material_ = ApplicationBase::Instance()->materialManager->GetResource(material_path);
		if (material_ != nullptr)
		{
			material_->PostResourceModify();
		}
	}
}

