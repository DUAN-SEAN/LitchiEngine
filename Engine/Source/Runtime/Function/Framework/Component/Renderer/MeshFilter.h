
#pragma once

#include <string>
#include <glm.hpp>
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/Resources/Model.h"

using std::string;

namespace LitchiRuntime
{
    class MeshFilter :public Component {
    public:
        MeshFilter();
        ~MeshFilter();
    public:
        ////Mesh文件头
        //
        Model* GetModel() { return m_model; }
        IMesh* GetExtraMesh() { return m_ExtraMesh; }
        void CreateUIMesh(const std::vector<Vertex>& vector, const std::vector<uint32_t>& index_vector);

        int GetMeshIndex(){ return meshIndex; }

    public:

        void PostResourceLoaded() override;
        void PostResourceModify() override;

    	/**
         * \brief 模型路径
         */
        std::string modelPath;

        int meshIndex;

        RTTR_ENABLE(Component)
    private:
        Model* m_model = nullptr;//Mesh对象
        IMesh* m_ExtraMesh = nullptr;
    };
}
