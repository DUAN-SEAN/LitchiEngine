
#pragma once

#include <string>

#include "Runtime/Core/Math/BoundingBox.h"
#include "Runtime/Core/Math/Matrix.h"
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/Rendering/Mesh.h"

using std::string;

namespace LitchiRuntime
{
	enum class BoundingBoxType
	{
		Untransformed,            // the bounding box of the mesh
		Transformed,              // the transformed bounding box of the mesh
		TransformedInstances,     // the transformed bounding box of all the instances
		TransformedInstanceGroup, // the transformed bounding box of an instance group
	};

	/**
	 * @brief MeshFilter Component
	 * @note holds a reference to a mesh.
	 *       It works with a Mesh Renderer component on the same GameObject
	 *       the Mesh Renderer renders the mesh that the Mesh Filter references.
	*/
	class MeshFilter :public Component {
	public:

		/**
		 * @brief Default Constructor
		*/
		MeshFilter();

		/**
		 * @brief Default Destructor
		*/
		~MeshFilter() override;

		/**
		 * @brief Call before object resource change
		*/
		void PostResourceModify() override;

		/**
		 * @brief Call before object resource loaded
		 * when instantiate prefab, add component, resource loaded etc
		 * after call resource load completed
		*/
		void PostResourceLoaded() override;

	public:

		/**
		 * @brief Set Geometry from mesh
		 * @param mesh Mesh pointer
		 * @param subMeshIndex SubMesh Index
		 * @param aabb Mesh bounding box of aabb
		*/
		void SetGeometry(
			Mesh* mesh,
			int subMeshIndex = 0,
			const BoundingBox aabb = BoundingBox::Undefined
		);

		/**
		 * @brief Get Geometry indices and vertices
		 * @param indices
		 * @param vertices
		*/
		void GetGeometry(std::vector<uint32_t>* indices, std::vector<RHI_Vertex_PosTexNorTan>* vertices) const;

		/**
		 * @brief Get Mesh pointer
		 * @return
		*/
		Mesh* GetMesh()                           const { return m_mesh; }

		/**
		 * @brief Set subMesh index in mesh
		 * @param subMeshIndex
		*/
		void SetSubMeshIndex(int subMeshIndex) { m_subMeshIndex = subMeshIndex; }

		/**
		 * @brief Get subMesh index in mesh
		 * @return
		*/
		int GetSubMeshIndex() { return m_subMeshIndex; }

		/**
		 * @brief Get SubMesh
		 * @return
		*/
		SubMesh& GetSubMesh() { return m_subMesh; }

		/**
		 * @brief Get Index offset in mesh
		 * @return
		*/
		uint32_t GetIndexOffset()                 const { return m_subMesh.m_geometryIndexOffset; }

		/**
		 * @brief Get Vertex Count in mesh
		 * @return
		*/
		uint32_t GetIndexCount()                  const { return m_subMesh.m_geometryIndexCount; }

		/**
		 * @brief Get Vertex offset in mesh
		 * @return
		*/
		uint32_t GetVertexOffset()                const { return m_subMesh.m_geometryVertexOffset; }

		/**
		 * @brief
		 * @return
		*/
		uint32_t GetVertexCount()                 const { return m_subMesh.m_geometryVertexCount; }

		/**
		 * @brief GPU Instance Object
		 * @return
		*/
		bool HasInstancing() const { return false; }
		/**
		 * @brief Get Bounding Box of mesh
		 * @return
		*/
		const BoundingBox& GetBoundingBox() const { return m_boundingBox; }
		const BoundingBox& GetBoundingBox(const BoundingBoxType type, const uint32_t instance_group_index = 0);
		/**
		 * @brief Set Mesh path
		 * @param meshPath
		*/
		void SetMeshPath(std::string meshPath) { m_meshPath = meshPath; }

		/**
		 * @brief Get Mesh path
		 * @return 
		*/
		std::string GetMeshPath() { return m_meshPath; }

	private:

		/**
		 * @brief mesh pointer
		*/
		Mesh* m_mesh = nullptr;

		/**
		 * @brief SubMesh Index in Mesh
		*/
		int m_subMeshIndex;

		/**
		 * @brief SubMesh
		*/
		SubMesh m_subMesh;

		/**
		 * \brief Mesh resource path
		 */
		std::string m_meshPath;

		/**
		 * @brief mesh bounding box
		*/
		BoundingBox m_boundingBox;
		bool m_bounding_box_dirty = true;
		BoundingBox m_bounding_box_untransformed;
		BoundingBox m_bounding_box_instances;
		std::vector<BoundingBox> m_bounding_box_instance_group;

		// misc
		Matrix m_transform_previous = Matrix::Identity;

		RTTR_ENABLE(Component)
	};
}
