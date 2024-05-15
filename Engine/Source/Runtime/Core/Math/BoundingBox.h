
#pragma once

//= INCLUDES ==========
#include "MathHelper.h"
#include "Vector3.h"
//=====================

namespace LitchiRuntime
{
	struct RHI_Vertex_PosTexNorTan;
	struct RHI_Vertex_PosTexNorTanBone;

	class Matrix;

	/**
	 * @brief Bounding Struct
	*/
	class LC_CLASS BoundingBox
	{
	public:

		/**
		 * @brief Construct with zero size.
		*/
		BoundingBox();

		/**
		 * @brief Construct from minimum and maximum vectors.
		 * @param min 
		 * @param max 
		*/
		BoundingBox(const Vector3& min, const Vector3& max);

		/**
		 * @brief Construct from points
		 * @param vertices 
		 * @param point_count 
		*/
		BoundingBox(const Vector3* vertices, const uint32_t point_count);

		/**
		 * @brief Construct from vertices
		 * @param vertices RHI_Vertex_PosTexNorTan without bone
		 * @param vertex_count 
		 * @param vertex_index 
		*/
		BoundingBox(const RHI_Vertex_PosTexNorTan* vertices, const uint32_t vertex_count,const uint32_t vertex_index = 0);

		/**
		 * @brief Construct from vertices with bone
		 * @param vertices RHI_Vertex_PosTexNorTanBone
		 * @param vertex_count 
		 * @param vertex_index 
		*/
		BoundingBox(const RHI_Vertex_PosTexNorTanBone* vertices, const uint32_t vertex_count, const uint32_t vertex_index = 0);

		/**
		 * @brief cctor
		*/
		~BoundingBox() = default;

		/**
		 * @brief Assign from bounding box
		 * @param rhs 
		 * @return 
		*/
		BoundingBox& operator =(const BoundingBox& rhs) = default;

		/**
		 * @brief Returns the center
		 * @return 
		*/
		Vector3 GetCenter() const { return (m_max + m_min) * 0.5f; }

		/**
		 * @brief Returns the size
		 * @return 
		*/
		Vector3 GetSize() const { return m_max - m_min; }

		/**
		 * @brief Returns extents
		 * @return 
		*/
		Vector3 GetExtents() const { return (m_max - m_min) * 0.5f; }

		/**
		 * @brief Test if a point is inside
		 * @param point 
		 * @return 
		*/
		Intersection IsInside(const Vector3& point) const;

		/**
		 * @brief Test if a bounding box is inside
		 * @param box 
		 * @return 
		*/
		Intersection IsInside(const BoundingBox& box) const;

		/**
		 * @brief Returns a transformed bounding box
		 * @param transform 
		 * @return 
		*/
		BoundingBox Transform(const Matrix& transform) const;

		/**
		 * @brief Merge with another bounding box
		 * @param box 
		*/
		void Merge(const BoundingBox& box);

		/**
		 * @brief Compare Other Bonding Box
		 * @param other compared target
		 * @return 
		*/
		bool operator==(const BoundingBox& other) const
		{
			return GetMin() == other.GetMin() && GetMax() == other.GetMax();
		}

		/**
		 * @brief Get BoundingBox min location
		 * @return 
		*/
		const Vector3& GetMin() const { return m_min; }

		/**
		 * @brief Get BoundingBox max location
		 * @return 
		*/
		const Vector3& GetMax() const { return m_max; }

		/**
		 * @brief 
		*/
		static const BoundingBox Undefined;

	private:
		Vector3 m_min;
		Vector3 m_max;
	};

}
