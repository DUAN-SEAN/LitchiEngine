
#include "MeshFilter.h"

#include <fstream>
#include "Runtime/Core/App/application.h"
#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Resource/ModelManager.h"

using std::ifstream;
using std::ios;

namespace LitchiRuntime
{

    MeshFilter::MeshFilter()
        :m_model(nullptr),meshIndex(0) {

    }

    MeshFilter::~MeshFilter() {

    }

    void MeshFilter::CreateUIMesh(const std::vector<Vertex>& vector, const std::vector<uint32_t>& index_vector)
    {
        if(m_ExtraMesh!=nullptr)
        {
            delete m_ExtraMesh;
            m_ExtraMesh = nullptr;
        }

        m_ExtraMesh = new UIMesh(vector, index_vector,0);
    }

    void MeshFilter::PostResourceLoaded()
    {
        // 资源加载后
        if(modelPath.empty())
        {
            return;
        }

        // 通过路径加载模型资源
        m_model = ApplicationBase::Instance()->modelManager->GetResource(modelPath);
    }

    void MeshFilter::PostResourceModify()
    {
        // 资源加载后
        if (modelPath.empty())
        {
            return;
        }

        // 通过路径加载模型资源
        m_model = ApplicationBase::Instance()->modelManager->GetResource(modelPath);
    }
}
