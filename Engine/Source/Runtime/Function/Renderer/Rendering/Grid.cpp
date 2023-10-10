/*
Copyright(c) 2016-2023 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//= INCLUDES =============================
#include "Runtime/Core/pch.h"
#include "Grid.h"
#include "../RHI/RHI_Vertex.h"
#include "../RHI/RHI_VertexBuffer.h"
#include "../World/Components/Transform.h"
//========================================

//= NAMESPACES ================
using namespace std;
using namespace LitchiRuntime::Math;
//=============================

namespace LitchiRuntime
{
    Grid::Grid()
    {
        // create vertices
        vector<RHI_Vertex_PosCol> vertices;
        BuildGrid(&vertices);
        m_vertex_count = static_cast<uint32_t>(vertices.size());

        // create vertex buffer
        m_vertex_buffer = make_shared<RHI_VertexBuffer>(false, "grid");
        m_vertex_buffer->Create(vertices);
    }

    const Matrix& Grid::ComputeWorldMatrix(shared_ptr<Transform> camera)
    {
        // To get the grid to feel infinite, it has to follow the camera,
        // but only by increments of the grid's spacing size. This gives the illusion 
        // that the grid never moves and if the grid is large enough, the user can't tell.
        const float grid_spacing  = 1.0f;
        const Vector3 translation = Vector3
        (
            static_cast<int>(camera->GetPosition().x / grid_spacing) * grid_spacing, 
            0.0f, 
            static_cast<int>(camera->GetPosition().z / grid_spacing) * grid_spacing
        );
    
        m_world = Matrix::CreateScale(grid_spacing) * Matrix::CreateTranslation(translation);

        return m_world;
    }

    void Grid::BuildGrid(vector<RHI_Vertex_PosCol>* vertices)
    {
        const int half_size_w = int(m_terrain_width * 0.5f);
        const int half_size_h = int(m_terrain_height * 0.5f);

        for (int j = -half_size_h; j < half_size_h; j++)
        {
            for (int i = -half_size_w; i < half_size_w; i++)
            {
                // Become more transparent, the further out we go
                const float alphaWidth  = 1.0f - static_cast<float>(Helper::Abs(j)) / static_cast<float>(half_size_h);
                const float alphaHeight = 1.0f - static_cast<float>(Helper::Abs(i)) / static_cast<float>(half_size_w);
                float alpha             = (alphaWidth + alphaHeight) * 0.5f;
                alpha                   = Helper::Clamp(Helper::Pow(alpha, 15.0f), 0.0f, 1.0f);

                // LINE 1
                // Upper left.
                float position_x = static_cast<float>(i);
                float position_z = static_cast<float>(j + 1);
                vertices->emplace_back(Vector3(position_x, 0.0f, position_z), Vector4(1.0f, 1.0f, 1.0f, alpha));

                // Upper right.
                position_x = static_cast<float>(i + 1);
                position_z = static_cast<float>(j + 1);
                vertices->emplace_back(Vector3(position_x, 0.0f, position_z), Vector4(1.0f, 1.0f, 1.0f, alpha));

                // LINE 2
                // Upper right.
                position_x = static_cast<float>(i + 1);
                position_z = static_cast<float>(j + 1);
                vertices->emplace_back(Vector3(position_x, 0.0f, position_z), Vector4(1.0f, 1.0f, 1.0f, alpha));

                // Bottom right.
                position_x = static_cast<float>(i + 1);
                position_z = static_cast<float>(j);
                vertices->emplace_back(Vector3(position_x, 0.0f, position_z), Vector4(1.0f, 1.0f, 1.0f, alpha));

                // LINE 3
                // Bottom right.
                position_x = static_cast<float>(i + 1);
                position_z = static_cast<float>(j);
                vertices->emplace_back(Vector3(position_x, 0.0f, position_z), Vector4(1.0f, 1.0f, 1.0f, alpha));

                // Bottom left.
                position_x = static_cast<float>(i);
                position_z = static_cast<float>(j);
                vertices->emplace_back(Vector3(position_x, 0.0f, position_z), Vector4(1.0f, 1.0f, 1.0f, alpha));

                // LINE 4
                // Bottom left.
                position_x = static_cast<float>(i);
                position_z = static_cast<float>(j);
                vertices->emplace_back(Vector3(position_x, 0.0f, position_z), Vector4(1.0f, 1.0f, 1.0f, alpha));

                // Upper left.
                position_x = static_cast<float>(i);
                position_z = static_cast<float>(j + 1);
                vertices->emplace_back(Vector3(position_x, 0.0f, position_z), Vector4(1.0f, 1.0f, 1.0f, alpha));
            }
        }
    }
}
