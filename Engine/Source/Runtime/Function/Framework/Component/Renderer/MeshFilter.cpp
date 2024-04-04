
#include "Runtime/Core/pch.h"
#include "MeshFilter.h"

#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Framework/Component/Animation/animator.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Resource/ModelManager.h"

using std::ifstream;
using std::ios;

namespace LitchiRuntime
{

    MeshFilter::MeshFilter()
        :m_subMeshIndex(0) {

    }

    MeshFilter::~MeshFilter() {

    }

   /* void MeshFilter::CreateUIMesh(const std::vector<Vertex>& vector, const std::vector<uint32_t>& index_vector)
    {
        if(m_ExtraMesh!=nullptr)
        {
            delete m_ExtraMesh;
            m_ExtraMesh = nullptr;
        }

        m_ExtraMesh = new UIMesh(vector, index_vector,0);
    }*/

    void MeshFilter::PostResourceLoaded()
    {
        PostResourceModify();

        //auto animator = GetGameObject()->GetComponent<Animator>();
        //if (m_mesh && animator)
        //{
        //    std::unordered_map<std::string, AnimationClip> animations;
        //    m_mesh->GetAnimations(animations);
        //    animator->SetAnimationClipMap(animations);

        //    // todo temp
        //    auto firstClipName = animations.begin()->first;
        //    animator->Play(firstClipName);
        //}
    }

    void MeshFilter::PostResourceModify()
    {
        // 资源加载后
        if (m_meshPath.empty() || m_meshPath == "Empty")
        {
            return;
        }

        // 通过路径加载模型资源
        auto mesh  = ApplicationBase::Instance()->modelManager->GetResource(m_meshPath);
        m_mesh = mesh;
        if (m_mesh == nullptr)
        {
            return;
        }

        bool result;
        auto _ = m_mesh->GetSubMesh(m_subMeshIndex, result);
        if(!result)
        {
            return;
        }

        SetGeometry(mesh, m_subMeshIndex);
    }

    void MeshFilter::SetGeometry(Mesh* mesh, int subMeshIndex /* = 0,*/, const BoundingBox aabb /* = BoundingBox::Undefined*/)
    {
        m_mesh = mesh;
        m_subMeshIndex = subMeshIndex;
        m_meshPath = m_mesh->GetResourceFilePathAsset();
        bool result;
        m_subMesh = m_mesh->GetSubMesh(subMeshIndex, result);

        m_boundingBox = aabb;
        if (m_boundingBox == BoundingBox::Undefined)
        {
            m_boundingBox = m_mesh->GetAabb();
        }
    }

	void MeshFilter::GetGeometry(std::vector<uint32_t>* indices, std::vector<RHI_Vertex_PosTexNorTan>* vertices) const
	{
		LC_ASSERT_MSG(m_mesh != nullptr, "Invalid mesh");

		m_mesh->GetGeometry(m_subMesh.m_geometryIndexOffset, m_subMesh.m_geometryIndexCount, m_subMesh.m_geometryVertexOffset, m_subMesh.m_geometryVertexCount, indices, vertices);
	}

	const BoundingBox& MeshFilter::GetAAbb()
	{
		// update if dirty
		if (m_lastTransform != GetGameObject()->GetComponent<Transform>()->GetMatrix() || m_boundingBoxTransformed == BoundingBox::Undefined)
		{
			m_boundingBoxTransformed = m_boundingBox.Transform(GetGameObject()->GetComponent<Transform>()->GetMatrix());
			m_lastTransform = GetGameObject()->GetComponent<Transform>()->GetMatrix();
		}

		return m_boundingBoxTransformed;
	}

}
