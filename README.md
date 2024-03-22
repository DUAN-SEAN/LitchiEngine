# LitchiEngine

一个简单的物理引擎，名字叫荔枝



直接使用现成库的第三方库：

+ compressonator
+ dxcompiler
+ freetype
+ mono
+ PhysX
+ spirv_cross
+ VulkanSDK



项目构建中的问题：

1. Severity	Code	Description	Project	File	Line	Suppression State
   Error	LNK1104	cannot open file 'PhysXDevice64.lib'	LitchiEditor	E:\CppProjects\VSCode\LitchiEngine-ex\Build\Engine\Source\Editor\LINK	1	



原项目问题清单：

1. 所有库都是以静态库的形式存在的，因为设置了`set(BUILD_SHARED_LIBS OFF)`
2. 图标设置的地方会抛异常（文件位置ApplicationBase.cpp h99）
3. Assets文件没有正确拷贝到Build目录
4. SceneManager::LoadScene中对AssetManager::LoadAsset函数的调用三元运算符好像写反了
5. release编译时 physx报的NDebug _Debug 宏那个编译问题 你现在有解决方案么
6. cmake debug和release编译配置 vs有个debug宏在release还存在
7. c#脚本项目 scriptcore 现在只是用的临时方案 看有没有更好的方案
