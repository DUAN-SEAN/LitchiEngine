
#include "mesh_filter.h"
#include <fstream>
#include "Runtime/Core/App/application.h"
#include "Runtime/Core/App/application_base.h"
#include "Runtime/Resource/ModelManager.h"

using std::ifstream;
using std::ios;

namespace LitchiRuntime
{

    MeshFilter::MeshFilter()
        :model_(nullptr),mesh_Index_(0) {

    }

    MeshFilter::~MeshFilter() {

    }

    void MeshFilter::CreateUIMesh(const std::vector<Vertex>& vector, const std::vector<uint32_t>& index_vector)
    {
        if(extra_mesh_!=nullptr)
        {
            delete extra_mesh_;
            extra_mesh_ = nullptr;
        }

        extra_mesh_ = new UIMesh(vector, index_vector,0);
    }

    void MeshFilter::PostResourceLoaded()
    {
        // 资源加载后
        if(model_path.empty())
        {
            return;
        }

        // 通过路径加载模型资源
        model_ = ApplicationBase::Instance()->modelManager->GetResource(model_path);
    }

    void MeshFilter::PostResourceModify()
    {
        // 资源加载后
        if (model_path.empty())
        {
            return;
        }

        // 通过路径加载模型资源
        model_ = ApplicationBase::Instance()->modelManager->GetResource(model_path);
    }
}
