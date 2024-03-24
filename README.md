# LitchiEngine

一个简单的物理引擎，名字叫荔枝



直接使用现成库的第三方库：

+ assimp

+ compressonator
+ dxcompiler
+ freetype
+ mono
+ PhysX
+ spirv_cross
+ VulkanSDK



项目构建中的问题：

1. gitignore忽略了部分lib文件
2. exe启动的时候会去找名叫libdxcompiler.dll的文件，但实际生成和链接应该的是dxcompiler.dll，先临时改变了名字为libdxcompiler.dll






原项目问题清单：

1. 所有库都是以静态库的形式存在的，因为设置了`set(BUILD_SHARED_LIBS OFF)`
2. 图标设置的地方会抛异常（文件位置ApplicationBase.cpp h99）
3. Assets文件没有正确拷贝到Build目录
4. SceneManager::LoadScene中对AssetManager::LoadAsset函数的调用三元运算符好像写反了
5. c#脚本项目 scriptcore 现在只是用的临时方案 看有没有更好的方案
6. Physx版本从142改为了143，有些方法调用需要更改
