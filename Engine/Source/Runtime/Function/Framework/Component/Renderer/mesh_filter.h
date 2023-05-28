
#ifndef UNTITLED_MESH_FILTER_H
#define UNTITLED_MESH_FILTER_H

#include <string>
#include <glm.hpp>
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/Resources/Model.h"

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
        ////Mesh文件头
        //
        Model* GetModel() { return model_; };//Mesh对象

        int GetMeshIndex(){ return mesh_Index_; }

    public:

        void PostResourceLoaded() override;
        void PostResourceModify() override;

    	/**
         * \brief 模型路径
         */
        std::string model_path;

        int mesh_Index_;

        RTTR_ENABLE(Component)
    private:
        Model* model_;//Mesh对象
    };
}
#endif //UNTITLED_MESH_FILTER_H
