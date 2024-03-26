
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
        :m_sub_mesh_index(0) {

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
        if (m_mesh_Path.empty() || m_mesh_Path == "Empty")
        {
            return;
        }

        // 通过路径加载模型资源
        auto mesh  = ApplicationBase::Instance()->modelManager->GetResource(m_mesh_Path);
        m_mesh = mesh;
        if (m_mesh == nullptr)
        {
            return;
        }

        bool result;
        auto _ = m_mesh->GetSubMesh(m_sub_mesh_index, result);
        if(!result)
        {
            return;
        }

        SetGeometry(mesh, m_sub_mesh_index);
    }

    void MeshFilter::SetGeometry(Mesh* mesh, int subMeshIndex /* = 0,*/, const BoundingBox aabb /* = BoundingBox::Undefined*/)
    {
        m_mesh = mesh;
        m_sub_mesh_index = subMeshIndex;
        m_mesh_Path = m_mesh->GetResourceFilePathAsset();
        bool result;
        m_sub_mesh = m_mesh->GetSubMesh(subMeshIndex, result);

        m_bounding_box = aabb;
        if (m_bounding_box == BoundingBox::Undefined)
        {
            m_bounding_box = m_mesh->GetAabb();
        }
    }

	void MeshFilter::GetGeometry(std::vector<uint32_t>* indices, std::vector<RHI_Vertex_PosTexNorTan>* vertices) const
	{
		LC_ASSERT_MSG(m_mesh != nullptr, "Invalid mesh");

		m_mesh->GetGeometry(m_sub_mesh.m_geometry_index_offset, m_sub_mesh.m_geometry_index_count, m_sub_mesh.m_geometry_vertex_offset, m_sub_mesh.m_geometry_vertex_count, indices, vertices);
	}

	const BoundingBox& MeshFilter::GetAAbb()
	{
		// update if dirty
		if (m_last_transform != GetGameObject()->GetComponent<Transform>()->GetMatrix() || m_bounding_box_transformed == BoundingBox::Undefined)
		{
			m_bounding_box_transformed = m_bounding_box.Transform(GetGameObject()->GetComponent<Transform>()->GetMatrix());
			m_last_transform = GetGameObject()->GetComponent<Transform>()->GetMatrix();
		}

		return m_bounding_box_transformed;
	}

}
