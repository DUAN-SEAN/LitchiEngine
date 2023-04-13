﻿//
// Created by captainchen on 2021/5/14.
//

#include "application_standalone.h"
#include <memory>
#include <iostream>
#include "rttr/registration"
#include "glad/gl.h"
#ifdef WIN32
// 避免出现APIENTRY重定义警告。
// freetype引用了windows.h，里面定义了APIENTRY。
// glfw3.h会判断是否APIENTRY已经定义然后再定义一次。
// 但是从编译顺序来看glfw3.h在freetype之前被引用了，判断不到 Windows.h中的定义，所以会出现重定义。
// 所以在 glfw3.h之前必须引用  Windows.h。
#include <Windows.h>
#endif
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "Runtime/Core/Log/debug.h"
#include "Runtime/Function/Input/input.h"

static void error_callback(int error, const char* description)
{
	DEBUG_LOG_ERROR("glfw error:{} description:{}", error, description);
}

/// 键盘回调
/// \param window
/// \param key
/// \param scancode
/// \param action
/// \param mods
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Input::RecordKey(key, action);
}
/// 鼠标按键回调
/// \param window
/// \param button
/// \param action
/// \param mods
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	Input::RecordKey(button, action);
	//    std::cout<<"mouse_button_callback:"<<button<<","<<action<<std::endl;
}
/// 鼠标移动回调
/// \param window
/// \param x
/// \param y
static void mouse_move_callback(GLFWwindow* window, double x, double y)
{
	Input::set_mousePosition(x, y);
	//    std::cout<<"mouse_move_callback:"<<x<<","<<y<<std::endl;
}
/// 鼠标滚轮回调
/// \param window
/// \param x
/// \param y
static void mouse_scroll_callback(GLFWwindow* window, double x, double y)
{
	Input::RecordScroll(y);
	//    std::cout<<"mouse_scroll_callback:"<<x<<","<<y<<std::endl;
}

void ApplicationStandalone::Init()
{
	ApplicationBase::Init();
	InitGraphicsLibraryFramework();
}


void ApplicationStandalone::InitGraphicsLibraryFramework() {

	glfwSetErrorCallback(error_callback);
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

	glfw_window_ = glfwCreateWindow(960, 640, title_.c_str(), NULL, NULL);
	if (!glfw_window_)
	{
		DEBUG_LOG_ERROR("glfwCreateWindow error!");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// 设置glfw
	glfwMakeContextCurrent(glfw_window_);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);

	glfwSetKeyCallback(glfw_window_, key_callback);
	glfwSetMouseButtonCallback(glfw_window_, mouse_button_callback);
	glfwSetScrollCallback(glfw_window_, mouse_scroll_callback);
	glfwSetCursorPosCallback(glfw_window_, mouse_move_callback);

	DEBUG_LOG_INFO("ApplicationStandalone::InitGraphicsLibraryFramework done");
}

void ApplicationStandalone::Run() {
	ApplicationBase::Run();

	while (true) {

		if (glfwWindowShouldClose(glfw_window_)) {
			break;
		}

		OneFrame();
		glfwPollEvents();
	}
	Exit();
}

void ApplicationStandalone::Exit() {
	ApplicationBase::Exit();

	glfwDestroyWindow(glfw_window_);
	glfwTerminate();
}