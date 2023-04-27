
#ifndef UNTITLED_APPLICATION_EDITOR_H
#define UNTITLED_APPLICATION_EDITOR_H

#include "Runtime/Core/App/application_base.h"
#include "Runtime/Function/Scene/scene_manager.h"
#include <glad/glad.h>

using namespace LitchiRuntime;

struct GLFWwindow;
class ApplicationEditor : public ApplicationBase
{

public:
	ApplicationEditor() :ApplicationBase() {}
	~ApplicationEditor() {}

	void Run();

public:
	/// 初始化图形库，例如glfw
	virtual void InitGraphicsLibraryFramework() override;

	virtual void Render() override;

	virtual void Exit() override;

private:
	
	void DrawHierarchy(bool* p_open);
	void DrawEditorFileContentWindow(bool* p_open);
	void DrawEditorGameWindow(bool* p_open);
	void DrawEditorInspectorWindow(bool* p_open);

private:
	GLFWwindow* editor_glfw_window_;//编辑器窗口

	Scene* selected_scene_;//记录Hierarchy当前选中的Node
	GameObject* selected_go_;


	GLuint color_texture_id_ = 0;//FBO 颜色纹理
	GLuint depth_texture_id_ = 0;//FBO 深度纹理
	GLuint frame_buffer_object_id_ = 0;//FBO 深度纹理
};




#endif //UNTITLED_APPLICATION_EDITOR_H
