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

1. exe启动的时候会去找名叫libdxcompiler.dll的文件，但实际生成和链接的应该是dxcompiler.dll，先临时改变了库的名字为libdxcompiler.dll




原项目问题清单：

1. 图标设置的地方会抛异常（文件位置ApplicationBase.cpp h99）
2. SceneManager::LoadScene中对AssetManager::LoadAsset函数的调用三元运算符好像写反了
3. c#脚本项目 scriptcore 现在只是用的临时方案 看有没有更好的方案
4. Physx版本从142改为了143，有些方法调用需要更改
