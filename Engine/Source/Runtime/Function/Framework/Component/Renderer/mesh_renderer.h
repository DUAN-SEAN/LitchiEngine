﻿
#ifndef UNTITLED_MESH_RENDERER_H
#define UNTITLED_MESH_RENDERER_H

#include <memory>
#include <glm.hpp>
#include "Runtime/Function/Framework/Component/Base/component.h"

namespace LitchiRuntime
{

    class Material;
    class MeshFilter;
    class Texture2D;
    class MeshRenderer :public Component {
    public:
        MeshRenderer();
        ~MeshRenderer();

        void SetMaterial(Material* material);//设置Material
        Material* material() { return material_; }

        void Render();//渲染
    private:
        Material* material_;

        unsigned int vertex_buffer_object_ = 0;//顶点缓冲区对象
        unsigned int element_buffer_object_ = 0;//索引缓冲区对象
        unsigned int vertex_array_object_ = 0;//顶点数组对象
    };
}


#endif //UNTITLED_MESH_RENDERER_H
