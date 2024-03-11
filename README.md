# LitchiEngine

一个简单的物理引擎，名字叫荔枝



项目构建中的问题：

1. Severity	Code	Description	Project	File	Line	Suppression State
   Error	C2039	'null': is not a member of '_GLFWwindow'	GLFW	E:\CppProjects\VSCode\LitchiEngine-ex\Engine\ThirdParty(new)\glfw\src\null_window.c	37	
2. Severity	Code	Description	Project	File	Line	Suppression State
   Error	LNK1104	cannot open file '..\..\..\bin\Debug\LitchiRuntimed.lib'	LitchiEditor	E:\CppProjects\VSCode\LitchiEngine-ex\Build\Engine\Source\Editor\LINK	1	
3. Severity	Code	Description	Project	File	Line	Suppression State
   Error	C1083	Cannot open include file: 'assimp/config.h': No such file or directory (compiling source file E:\CppProjects\VSCode\LitchiEngine-ex\Engine\Source\Runtime\Function\Renderer\Resource\Import\ModelImporter.cpp)	LitchiRuntime	E:\CppProjects\VSCode\LitchiEngine-ex\Engine\ThirdParty(new)\assimp\include\assimp\defs.h	55	
4. Severity	Code	Description	Project	File	Line	Suppression State
   Error	C1083	Cannot open include file: 'compressonator.h': No such file or directory	LitchiRuntime	E:\CppProjects\VSCode\LitchiEngine-ex\Engine\Source\Runtime\Function\Renderer\RHI\RHI_Texture.cpp	8	
5. Severity	Code	Description	Project	File	Line	Suppression State
   Error	C1083	Cannot open include file: 'spirv_cross/spirv_hlsl.hpp': No such file or directory	LitchiRuntime	E:\CppProjects\VSCode\LitchiEngine-ex\Engine\Source\Runtime\Function\Renderer\RHI\Vulkan\Vulkan_Shader.cpp	11
6. Severity	Code	Description	Project	File	Line	Suppression State
   Error	LNK2001	unresolved external symbol _calloc_dbg	profiler_converter	E:\CppProjects\VSCode\LitchiEngine-ex\Build\Engine\ThirdParty(new)\easy_profiler\easy_profiler_converter\libcpmtd.lib(xlocale.obj)	1	
7. Severity	Code	Description	Project	File	Line	Suppression State
   Error	LNK2001	unresolved external symbol _calloc_dbg	dxcompiler	E:\CppProjects\VSCode\LitchiEngine-ex\Build\Engine\ThirdParty(new)\dxcompiler\source\dxcompiler\libcpmtd.lib(xlocale.obj)	1	



原项目问题清单：

1. CMake中写了"Debug"模式下使用"MTd"设置Runtime Library，但是实际上每个都是"Mt"
2. 所有库都是以静态库的形式存在的，因为设置了`set(BUILD_SHARED_LIBS OFF)`
3. 图标设置的地方会抛异常（文件位置ApplicationBase.cpp h99）
4. Assets文件没有正确拷贝到Build目录
5. SceneManager::LoadScene中对AssetManager::LoadAsset函数的调用三元运算符好像写反了
6. release编译时 physx报的NDebug _Debug 宏那个编译问题 你现在有解决方案么
7. cmake debug和release编译配置 vs有个debug宏在release还存在
8. c#脚本项目 scriptcore 现在只是用的临时方案 看有没有更好的方案
