
#include "application_standalone.h"
#include <memory>
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
#include "Runtime/Core/Log/debug.h"
#include "Runtime/Core/Screen/screen.h"
#include "Runtime/Function/Input/input.h"
#include "Runtime/Test/ShaderSource.h"

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
	LitchiRuntime::Input::RecordKey(key, action);
}
/// 鼠标按键回调
/// \param window
/// \param button
/// \param action
/// \param mods
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	LitchiRuntime::Input::RecordKey(button, action);
	//    std::cout<<"mouse_button_callback:"<<button<<","<<action<<std::endl;
}
/// 鼠标移动回调
/// \param window
/// \param x
/// \param y
static void mouse_move_callback(GLFWwindow* window, double x, double y)
{
	LitchiRuntime::Input::set_mousePosition(x, y);
	//    std::cout<<"mouse_move_callback:"<<x<<","<<y<<std::endl;
}
/// 鼠标滚轮回调
/// \param window
/// \param x
/// \param y
static void mouse_scroll_callback(GLFWwindow* window, double x, double y)
{
	LitchiRuntime::Input::RecordScroll(y);
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
	if (compileResult == GL_FALSE)
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
	if (linkResult == GL_FALSE)
	{
		GLchar message[256];
		glGetProgramInfoLog(program, sizeof(message), 0, message);
		DEBUG_LOG_ERROR("link program error");
		DEBUG_LOG_ERROR(message);
	}
}

namespace LitchiRuntime
{

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

}