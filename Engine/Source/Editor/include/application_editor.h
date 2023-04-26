
#ifndef UNTITLED_APPLICATION_EDITOR_H
#define UNTITLED_APPLICATION_EDITOR_H

#include <string>
#include "Runtime/Core/App/application_base.h"
#include "Runtime/Core/DataStruct/tree.h"
#include <glad/glad.h>

using namespace LitchiRuntime;

	struct GLFWwindow;
	class ApplicationEditor : public ApplicationBase {
	public:
		ApplicationEditor() :ApplicationBase() {}
		~ApplicationEditor() {}

		void Run();

		/// 绘制节点结构
		/// \param node
		/// \param label
		/// \param base_flags
		void DrawHierarchy(Tree::Node* node, const char* label, int base_flags);
	public:
		/// 初始化图形库，例如glfw
		virtual void InitGraphicsLibraryFramework() override;

		virtual void Render() override;

		virtual void Exit() override;
	private:
		GLFWwindow* editor_glfw_window_;//编辑器窗口
		GLFWwindow* game_glfw_window_;//游戏窗口

		Tree::Node* selected_node_ = nullptr;//记录Hierarchy当前选中的Node

		GLuint color_texture_id_ = 0;//FBO 颜色纹理
		GLuint depth_texture_id_ = 0;//FBO 深度纹理
		GLuint frame_buffer_object_id_ = 0;//FBO 深度纹理
	};




#endif //UNTITLED_APPLICATION_EDITOR_H
