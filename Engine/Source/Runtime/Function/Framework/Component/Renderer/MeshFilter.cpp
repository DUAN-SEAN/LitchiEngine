
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

    void MeshFilter::SetGeometry(Mesh* mesh, int subMeshIndex /* = 0,*/, const BoundingBox aabb /* = BoundingBox::Max*/)
    {
        bool result;
        m_mesh = mesh;
        m_subMeshIndex = subMeshIndex;
        m_bounding_box_untransformed = aabb;
        m_meshPath = m_mesh->GetResourceFilePathAsset();
        m_subMesh = m_mesh->GetSubMesh(subMeshIndex, result);

        if (m_subMesh.m_geometryIndexCount == 0)
        {
            m_subMesh.m_geometryIndexCount = m_mesh->GetIndexCount();
        }

        if (m_subMesh.m_geometryVertexCount == 0)
        {
            m_subMesh.m_geometryVertexCount = m_mesh->GetVertexCount();
        }

        if (m_bounding_box_untransformed == BoundingBox::Undefined)
        {
            m_bounding_box_untransformed = m_mesh->GetAabb();
        }

        LC_ASSERT(m_subMesh.m_geometryIndexCount != 0)
        LC_ASSERT(m_subMesh.m_geometryVertexCount != 0)
    }

	void MeshFilter::GetGeometry(std::vector<uint32_t>* indices, std::vector<RHI_Vertex_PosTexNorTan>* vertices) const
	{
		LC_ASSERT_MSG(m_mesh != nullptr, "Invalid mesh");

		m_mesh->GetGeometry(m_subMesh.m_geometryIndexOffset, m_subMesh.m_geometryIndexCount, m_subMesh.m_geometryVertexOffset, m_subMesh.m_geometryVertexCount, indices, vertices);
	}

    const BoundingBox& MeshFilter::GetBoundingBox(const BoundingBoxType type, const uint32_t instance_group_index)
    {
        auto transform = GetGameObject()->GetComponent<Transform>()->GetMatrix();

        // compute if dirty
        if (m_bounding_box_dirty || m_transform_previous != transform)
        {
            // transformed
            {
                m_boundingBox = m_bounding_box_untransformed.Transform(transform);
            }

            //// transformed instances
            //{
            //    // loop through each instance and expand the bounding box
            //    m_bounding_box_instances = BoundingBox::Undefined;
            //    for (const Matrix& instance_transform : m_instances)
            //    {
            //        // transform * instance_transform, this is not the order of operation the engine is using but in this case it works
            //        // possibly due to how the transform is calculated, the space it's in and relative to what
            //        BoundingBox bounding_box_instance = m_bounding_box_untransformed.Transform(transform * instance_transform);
            //        m_bounding_box_instances.Merge(bounding_box_instance);
            //    }
            //}

            //// transformed instance groups
            //{
            //    // loop through each group end index
            //    m_bounding_box_instance_group.clear();
            //    uint32_t start_index = 0;
            //    for (const uint32_t group_end_index : m_instance_group_end_indices)
            //    {
            //        // loop through the instances in this group
            //        BoundingBox bounding_box_group = BoundingBox::Undefined;
            //        for (uint32_t i = start_index; i < group_end_index; i++)
            //        {
            //            BoundingBox bounding_box_instance = m_bounding_box_untransformed.Transform(transform * m_instances[i]);
            //            bounding_box_group.Merge(bounding_box_instance);
            //        }

            //        m_bounding_box_instance_group.push_back(bounding_box_group);
            //        start_index = group_end_index;
            //    }
            //}

            m_transform_previous = transform;
            m_bounding_box_dirty = false;
        }

        // return
        if (type == BoundingBoxType::Untransformed)
        {
            return m_bounding_box_untransformed;
        }
        else if (type == BoundingBoxType::Transformed)
        {
            return m_boundingBox;
        }
        else if (type == BoundingBoxType::TransformedInstances)
        {
            return m_bounding_box_instances;
        }
        else if (type == BoundingBoxType::TransformedInstanceGroup)
        {
            return m_bounding_box_instance_group[instance_group_index];
        }

        return BoundingBox::Undefined;
    }

}
