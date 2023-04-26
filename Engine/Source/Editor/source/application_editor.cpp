
#include <memory>
#include <iostream>
#include "rttr/registration"

#ifdef WIN32
// 避免出现APIENTRY重定义警告。
// freetype引用了windows.h，里面定义了APIENTRY。
// glfw3.h会判断是否APIENTRY已经定义然后再定义一次。
// 但是从编译顺序来看glfw3.h在freetype之前被引用了，判断不到 Windows.h中的定义，所以会出现重定义。
// 所以在 glfw3.h之前必须引用  Windows.h。
#include <Windows.h>
#endif

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Runtime/Core/Log/debug.h"

#include "Editor/include/application_editor.h"
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"

using namespace LitchiRuntime;

static void glfw_error_callback(int error, const char* description)
{
	DEBUG_LOG_ERROR("glfw error:{} description:{}", error, description);
}

void ApplicationEditor::InitGraphicsLibraryFramework() {
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
	{
		DEBUG_LOG_ERROR("glfw init failed!");
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	
	//创建编辑器窗口，并将游戏Context共享。
	editor_glfw_window_ = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
	if (!editor_glfw_window_)
	{
		DEBUG_LOG_ERROR("glfwCreateWindow error!");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//设置编辑器主线程使用的是 Editor Context
	glfwMakeContextCurrent(editor_glfw_window_);
	gladLoadGL();
	//开启垂直同步
	glfwSwapInterval(1); // Enable vsync

	//ImGui初始化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	//设置主题
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	//配置后端
	ImGui_ImplGlfw_InitForOpenGL(editor_glfw_window_, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	//创建全局FBO，将整个游戏渲染到FBO，提供给编辑器，作为Game视图显示
	frame_buffer_object_id_ = 0;
	glGenFramebuffers(1, &frame_buffer_object_id_); __CHECK_GL_ERROR__
		if (frame_buffer_object_id_ == 0) {
			DEBUG_LOG_ERROR("CreateFBO FBO Error!");
			return;
		}
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_object_id_); __CHECK_GL_ERROR__

		//创建颜色纹理 Attach到FBO颜色附着点上
		glGenTextures(1, &color_texture_id_);
	glBindTexture(GL_TEXTURE_2D, color_texture_id_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 480, 320, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr); __CHECK_GL_ERROR__
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture_id_, 0); __CHECK_GL_ERROR__

		//创建深度纹理 Attach到FBO深度附着点上
		glGenTextures(1, &depth_texture_id_);
	glBindTexture(GL_TEXTURE_2D, depth_texture_id_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 480, 320, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr); __CHECK_GL_ERROR__
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_id_, 0); __CHECK_GL_ERROR__

		//检测帧缓冲区完整性
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); __CHECK_GL_ERROR__
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			DEBUG_LOG_ERROR("BindFBO FBO Error,Status:{} !", status);//36055 = 0x8CD7 GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 附着点没有东西
			return;
		}

}

void ApplicationEditor::Render()
{
	// 设置FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_object_id_); __CHECK_GL_ERROR__
	//glClearColor(0,0,0,1);
	//glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	//检测帧缓冲区完整性
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); __CHECK_GL_ERROR__
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			DEBUG_LOG_ERROR("BindFBO FBO Error,Status:{} !", status);//36055 = 0x8CD7 GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 附着点没有东西
			return;
		}

	// 执行渲染
	ApplicationBase::Render();

	// 还原FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); __CHECK_GL_ERROR__
}

void ApplicationEditor::Run() {
	ApplicationBase::Run();

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	while (!glfwWindowShouldClose(editor_glfw_window_))
	{
		glfwPollEvents();

		//渲染游戏
		OneFrame();

		//ImGui刷帧
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//// 1. 状态
		//{
		//	ImGui::Begin("Status");
		//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//	ImGui::End();
		//}

		// 2. 游戏渲染画面
		{
			ImGui::Begin("ViewPort");
			if (ImGui::BeginTabBar("ViewPortTabBar", ImGuiTabBarFlags_None)) {
				// 2.1 Game视图
				if (ImGui::BeginTabItem("Game")) {

					//从游戏渲染线程拿到FBO Attach Texture id
					GLuint texture_id = color_texture_id_;
					ImTextureID image_id = (void*)(intptr_t)texture_id;

					// 第一个参数：生成的纹理的id
					// 第2个参数：Image的大小
					// 第3，4个参数：UV的起点坐标和终点坐标，UV是被规范化到（0，1）之间的坐标
					// 第5个参数：图片的色调
					// 第6个参数：图片边框的颜色
					ImGui::Image(image_id, ImVec2(480, 320), ImVec2(0.0, 1.0), ImVec2(1.0, 0.0), ImVec4(255, 255, 255, 1), ImVec4(0, 255, 0, 1));

					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			ImGui::End();
		}


		//// 3. Hierarchy
		//{
		//	ImGui::Begin("Hierarchy");
		//	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		//	Tree::Node* root_node = GameObject::game_object_tree().root_node();
		//	DrawHierarchy(root_node, "scene", base_flags);
		//	ImGui::End();
		//}

		////4. Property
		//{
		//	ImGui::Begin("Property");

		//	//4.1 GameObject属性
		//	GameObject* game_object = nullptr;
		//	if (selected_node_ != nullptr) {
		//		game_object = dynamic_cast<GameObject*>(selected_node_);
		//	}
		//	if (game_object != nullptr) {
		//		//是否Active
		//		bool active_self = game_object->active_self();
		//		if (ImGui::Checkbox("active", &active_self)) {
		//			game_object->set_active_self(active_self);
		//		}
		//		//Layer
		//		int layer = game_object->layer();
		//		if (ImGui::InputInt("Layer", &layer)) {
		//			game_object->set_layer(layer);
		//		}

		//	}
		//	else {
		//		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "no valid GameObject");
		//	}

		//	//4.2 Transform属性
		//	Transform* transform = nullptr;
		//	glm::vec3 position, rotation, scale;

		//	if (game_object != nullptr) {
		//		transform = game_object->GetComponent<Transform>();
		//	}

		//	if (transform != nullptr) {
		//		position = transform->position();
		//		rotation = transform->rotation();
		//		scale = transform->scale();

		//		if (ImGui::TreeNode("Transform")) {
		//			// 显示属性，如果数值改变，将数据写回Transform
		//			if (ImGui::InputFloat3("position", (float*)&position)) {
		//				transform->set_position(position);
		//			}
		//			if (ImGui::InputFloat3("rotation", (float*)&rotation)) {
		//				transform->set_rotation(rotation);
		//			}
		//			if (ImGui::InputFloat3("scale", (float*)&scale)) {
		//				transform->set_scale(scale);
		//			}
		//			ImGui::TreePop();
		//		}
		//	}
		//	else {
		//		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "not found Transform");
		//	}

		//	ImGui::End();
		//}

		//绘制
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(editor_glfw_window_, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(editor_glfw_window_);


	}

	Exit();
}

void ApplicationEditor::DrawHierarchy(Tree::Node* node, const char* label, int base_flags) {
	//int flags = base_flags;

	//if (selected_node_ == node) {//如果当前Node是被选中的，那么设置flag，显示样式为选中。
	//	flags |= ImGuiTreeNodeFlags_Selected;
	//}

	//std::list<Tree::Node*>& children = node->children();
	//if (children.size() > 0) {
	//	if (ImGui::TreeNodeEx(label, flags)) {//如果被点击，就展开子节点。
	//		if (ImGui::IsItemClicked()) {
	//			selected_node_ = node;
	//		}
	//		for (auto* child : children) {
	//			GameObject* game_object = dynamic_cast<GameObject*>(child);
	//			//                DEBUG_LOG_INFO("game object:{} depth:{}",game_object->name(),game_object->depth());
	//			DrawHierarchy(child, game_object->name(), base_flags);
	//		}
	//		ImGui::TreePop();//可以点击展开的TreeNode，需要加上TreePop()。
	//	}
	//}
	//else {//没有子节点，不显示展开按钮
	//	flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	//	ImGui::TreeNodeEx(label, flags);
	//	if (ImGui::IsItemClicked()) {
	//		selected_node_ = node;
	//	}
	//}
}

void ApplicationEditor::Exit() {
	ApplicationBase::Exit();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(editor_glfw_window_);
	glfwTerminate();
}

