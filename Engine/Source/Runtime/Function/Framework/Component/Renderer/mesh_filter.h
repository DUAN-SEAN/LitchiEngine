﻿
#ifndef UNTITLED_MESH_FILTER_H
#define UNTITLED_MESH_FILTER_H

#include <string>
#include <glm.hpp>
#include "Runtime/Function/Framework/Component/Base/component.h"

using std::string;

namespace LitchiRuntime
{
	class VertexRes
	{
	public:

        glm::vec3 pos_;
        glm::vec4 color_;
        glm::vec2 uv_;

		RTTR_ENABLE()
	};

    class MeshFilter :public Component {
    public:
        MeshFilter();
        ~MeshFilter();
    public:
        //顶点
        struct Vertex
        {
            glm::vec3 pos_;
            glm::vec4 color_;
            glm::vec2 uv_;
        };

        //Mesh文件头
        struct MeshFileHead {
            char type_[4];
            unsigned short vertex_num_;//顶点个数
            unsigned short vertex_index_num_;//索引个数
        };

        //Mesh数据
        struct Mesh {
            unsigned short vertex_num_;//顶点个数
            unsigned short vertex_index_num_;//索引个数
            Vertex* vertex_data_;//顶点数据
            unsigned short* vertex_index_data_;//顶点索引数据
        };

        void LoadMesh(string mesh_file_path);//加载Mesh文件

        Mesh* mesh() { return mesh_; };//Mesh对象
    private:
        Mesh* mesh_;//Mesh对象
    };
}
#endif //UNTITLED_MESH_FILTER_H
