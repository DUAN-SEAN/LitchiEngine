//
// Created by captainchen on 2021/5/14.
//

#include "application_standalone.h"
#include <memory>
#include <iostream>
#include "rttr/registration"
#include <glad/glad.h>

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
#include "gtx/euler_angles.hpp"
#include "gtx/transform.hpp"
#include "Runtime/Core/Log/debug.h"
#include "Runtime/Core/Screen/screen.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Function/Input/input.h"
#include "Runtime/Test/ShaderSource.h"
#include "Runtime/Test/VertexData.h"

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


GLuint vertex_shader, fragment_shader, program;
GLint mvp_location, vpos_location, vcol_location;

void CompilerShader()
{
	// 创建vs GLuint
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	// 指定shader源码
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	// 编译shader
	glCompileShader(vertex_shader);
	GLint compileResult = GL_FALSE;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compileResult);
	if(compileResult == GL_FALSE)
	{
		GLchar message[256];
		glGetShaderInfoLog(vertex_shader, sizeof(message), 0, message);
		DEBUG_LOG_ERROR("compile vs error");
		DEBUG_LOG_ERROR(message);
	}

	// 创建fs GLuint
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	// 指定shader源码
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	// 编译Shader
	glCompileShader(fragment_shader);
	// 获取编译结果
	compileResult = GL_FALSE;
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE)
	{
		GLchar message[256];
		DEBUG_LOG_ERROR("compile fs error");
		DEBUG_LOG_ERROR(message);
	}

	// 创建GPU程序
	program = glCreateProgram();
	// 附加Shader
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	// 将program链接到当前的OpenGL状态机
	glLinkProgram(program);
	// 获取编译结果
	GLint linkResult = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &linkResult);
	if(linkResult == GL_FALSE)
	{
		GLchar message[256];
		glGetProgramInfoLog(program, sizeof(message), 0, message);
		DEBUG_LOG_ERROR("link program error");
		DEBUG_LOG_ERROR(message);
	}
}



void ApplicationStandalone::UpdateScreenSize()
{
	int width, height;
	glfwGetFramebufferSize(glfw_window_, &width, &height);
	glViewport(0, 0, width, height);
	Screen::set_width_height(width, height);
}

void ApplicationStandalone::Init()
{
	ApplicationBase::Init();


	//// 编译Shader
	//CompilerShader();


	//mvp_location = glGetUniformLocation(program, "u_mvp");
	//vpos_location = glGetAttribLocation(program, "a_pos");
	//vcol_location = glGetAttribLocation(program, "a_color");
}

void ApplicationStandalone::OneFrame()
{
	// 遍历所有的GameObject
	ApplicationBase::OneFrame();

	glfwSwapBuffers(glfw_window_);
}

//void ApplicationStandalone::OneFrame()
//{
//	//获取shader属性ID
//
//	float ratio;
//	int width, height;
//	glm::mat4 model, view, projection, mvp;
//
//	// 获取画面宽高
//	glfwGetFramebufferSize(glfw_window_, &width, &height);
//	ratio = width / (float)height;
//
//	glViewport(0,0, width, height);
//
//	// 清理屏幕缓存区，并设置缓冲区颜色
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glClearColor(49.f / 255, 77.f / 255, 121.f / 255, 1.f);
//
//	// 坐标系变换
//	glm::mat4 trans = glm::translate(glm::vec3(0, 0, 0));
//	static float rotate_eulerAngle = 0.f;
//	rotate_eulerAngle += 1;
//	// 按照YXZ的顺序旋转
//	glm::mat4 rotation = glm::eulerAngleYXZ(glm::radians(rotate_eulerAngle),
//		glm::radians(rotate_eulerAngle), glm::radians(rotate_eulerAngle)); //使用欧拉角旋转;
//	glm::mat4 scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)); //缩放;
//
//	// 计算模型在世界中的变换矩阵
//	model = trans * scale * rotation;
//
//	// 计算相机的视口坐标系
//	view = glm::lookAt(glm::vec3(0, 0, 10), glm::vec3(0, 0,10), glm::vec3(0, 1, 0));
//	
//	// 构造相机透视投影
//	projection = glm::perspective(glm::radians(60.f), ratio, 1.f, 1000.f);
//
//	// 计算mvp矩阵（从模型空间到相机空间）
//	mvp = projection * view * model;
//
//	//指定GPU程序(就是指定顶点着色器、片段着色器)
//	glUseProgram(program);
//	glEnable(GL_DEPTH_TEST);// 开启深度测试
//	glEnable(GL_CULL_FACE);//开启背面剔除
//
//	//启用顶点Shader属性(a_pos)，指定与顶点坐标数据进行关联
//	GLuint vertex_buffer_object_;
//	//在GPU上创建缓冲区对象
//	glGenBuffers(1, &vertex_buffer_object_);
//	//将缓冲区对象指定为顶点缓冲区对象
//	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
//	//上传顶点数据到缓冲区对象
//	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(glm::vec3), kPositions, GL_STATIC_DRAW);
//	glEnableVertexAttribArray(vpos_location);
//	glVertexAttribPointer(vpos_location, 3, GL_FLOAT, false, sizeof(glm::vec3), 0);
//
//	//在GPU上创建缓冲区对象
//	GLuint vertex_buffer_object_2;
//	glGenBuffers(1, &vertex_buffer_object_2);
//	//将缓冲区对象指定为顶点缓冲区对象
//	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_2);
//	//上传顶点数据到缓冲区对象
//	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(glm::vec4), kColors, GL_STATIC_DRAW);
//	//启用顶点Shader属性(a_color)，指定与顶点颜色数据进行关联
//	glEnableVertexAttribArray(vcol_location);
//	glVertexAttribPointer(vcol_location, 4, GL_FLOAT, false, sizeof(glm::vec4), 0);
//
//
//	//上传mvp矩阵
//	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
//
//	//上传顶点数据并进行绘制
//	glDrawArrays(GL_TRIANGLES, 0, 36);
//
//	// 交换缓冲区 双缓冲区
//	glfwSwapBuffers(glfw_window_);
//
//}
//

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
	gladLoadGL();
	//gladLoadGL(glfwGetProcAddress);
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