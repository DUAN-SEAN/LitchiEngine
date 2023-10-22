
#include "MeshFilter.h"

#include <fstream>
#include "Runtime/Core/App/application.h"
#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Resource/ModelManager.h"

using std::ifstream;
using std::ios;

namespace LitchiRuntime
{

    MeshFilter::MeshFilter()
        :meshIndex(0) {

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
        //// 资源加载后
        //if(modelPath.empty())
        //{
        //    return;
        //}

        //// 通过路径加载模型资源
        //m_model = ApplicationBase::Instance()->modelManager->GetResource(modelPath);
    }

    void MeshFilter::PostResourceModify()
    {
        //// 资源加载后
        //if (modelPath.empty())
        //{
        //    return;
        //}

        //// 通过路径加载模型资源
        //m_model = ApplicationBase::Instance()->modelManager->GetResource(modelPath);
    }


	void MeshFilter::SetGeometry(
		Mesh* mesh,
		const BoundingBox aabb /*= Math::BoundingBox::Undefined*/,
		uint32_t index_offset  /*= 0*/, uint32_t index_count  /*= 0*/,
		uint32_t vertex_offset /*= 0*/, uint32_t vertex_count /*= 0 */
	)
	{
		m_mesh = mesh;
		m_bounding_box = aabb;
		m_geometry_index_offset = index_offset;
		m_geometry_index_count = index_count;
		m_geometry_vertex_offset = vertex_offset;
		m_geometry_vertex_count = vertex_count;

		if (m_geometry_index_count == 0)
		{
			m_geometry_index_count = m_mesh->GetIndexCount();
		}

		if (m_geometry_vertex_count == 0)
		{
			m_geometry_vertex_count = m_mesh->GetVertexCount();
		}

		if (m_bounding_box == BoundingBox::Undefined)
		{
			m_bounding_box = m_mesh->GetAabb();
		}

		/*SP_ASSERT(m_geometry_index_count != 0);
		SP_ASSERT(m_geometry_vertex_count != 0);
		SP_ASSERT(m_bounding_box != BoundingBox::Undefined);*/
	}

	void MeshFilter::GetGeometry(std::vector<uint32_t>* indices, std::vector<RHI_Vertex_PosTexNorTan>* vertices) const
	{
		SP_ASSERT_MSG(m_mesh != nullptr, "Invalid mesh");
		m_mesh->GetGeometry(m_geometry_index_offset, m_geometry_index_count, m_geometry_vertex_offset, m_geometry_vertex_count, indices, vertices);
	}

	const BoundingBox& MeshFilter::GetAabb()
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
