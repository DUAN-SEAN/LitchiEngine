//
//#pragma once
//
//#include <cstdint>
//
//#include "Renderer.h"
//
//
//namespace LitchiRuntime
//{
//	namespace Resource
//	{
//		class Shader;
//	}
//
//	class Mesh;
//}
//
//namespace LitchiRuntime
//{
//	/**
//	* The ShapeDrawer handles the drawing of basic shapes
//	*/
//	class ShapeDrawer
//	{
//	public:
//		/**
//		* Creates the ShapeDrawer instance. It needs a renderer
//		* @param p_renderer
//		*/
//		ShapeDrawer(LitchiRuntime::Renderer& p_renderer);
//
//		/**
//		* Destructor
//		*/
//		~ShapeDrawer();
//
//		/**
//		* Defines the view projection to use when drawing
//		* @param p_viewProjection
//		*/
//		void SetViewProjection(const Matrix& p_viewProjection);
//
//		/**
//		* Draw a line in world space
//		* @param p_start
//		* @param p_end
//		* @param p_color
//		* @param p_lineWidth
//		*/
//		void DrawLine(const Vector3& p_start, const Vector3& p_end, const Vector3& p_color, float p_lineWidth = 1.0f);
//
//		/**
//		* Draw a grid in world space
//		* @param p_viewPos
//		* @param p_color
//		* @param p_gridSize
//		* @param p_linear
//		* @param p_quadratic
//		* @param p_fadeThreshold
//		* @param p_lineWidth
//		*/
//		void DrawGrid(const Vector3& p_viewPos, const Vector3& p_color, int32_t p_gridSize = 50, float p_linear = 0.0f, float p_quadratic = 0.0f, float p_fadeThreshold = 0.0f, float p_lineWidth = 1.0f);
//
//	private:
//		LitchiRuntime::Shader* m_lineShader = nullptr;
//		LitchiRuntime::Shader* m_gridShader = nullptr;
//		LitchiRuntime::Mesh* m_lineMesh = nullptr;
//		LitchiRuntime::Renderer& m_renderer;
//	};
//}
