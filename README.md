# LitchiEngine

一个简单的物理引擎，名字叫荔枝



问题清单：

1. CMake中写了"Debug"模式下使用"MTd"设置Runtime Library，但是实际上每个都是"Mt"
2. 所有库都是以静态库的形式存在的，因为设置了`set(BUILD_SHARED_LIBS OFF)`
3. 图标设置的地方会抛异常（文件位置ApplicationBase.cpp h99）
4. Assets文件没有正确拷贝到Build目录
5. SceneManager::LoadScene中对AssetManager::LoadAsset函数的调用三元运算符好像写反了
6. 
