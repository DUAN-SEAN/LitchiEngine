
#pragma once

//= INCLUDES ======================
#include <vector>
#include "../RHI/RHI_Definitions.h"
#include "../RHI/RHI_Vertex.h"
//=================================

namespace LitchiRuntime::Geometry
{
    static void CreateCube(std::vector<RHI_Vertex_PosTexNorTan>* vertices, std::vector<uint32_t>* indices, float size = 1.0f)
    {
        using namespace Math;

        // front
        vertices->emplace_back(Vector3(-0.5f, -0.5f, -0.5f) * size, Vector2(0, 1), Vector3(0, 0, -1) * size, Vector3(0, 1, 0));
        vertices->emplace_back(Vector3(-0.5f, 0.5f, -0.5f) * size, Vector2(0, 0), Vector3(0, 0, -1) * size, Vector3(0, 1, 0));
        vertices->emplace_back(Vector3(0.5f, -0.5f, -0.5f) * size, Vector2(1, 1), Vector3(0, 0, -1) * size, Vector3(0, 1, 0));
        vertices->emplace_back(Vector3(0.5f, 0.5f, -0.5f) * size, Vector2(1, 0), Vector3(0, 0, -1) * size, Vector3(0, 1, 0));

        // bottom
        vertices->emplace_back(Vector3(-0.5f, -0.5f, 0.5f) * size, Vector2(0, 1), Vector3(0, -1, 0) * size, Vector3(1, 0, 0));
        vertices->emplace_back(Vector3(-0.5f, -0.5f, -0.5f) * size, Vector2(0, 0), Vector3(0, -1, 0) * size, Vector3(1, 0, 0));
        vertices->emplace_back(Vector3(0.5f, -0.5f, 0.5f) * size, Vector2(1, 1), Vector3(0, -1, 0) * size, Vector3(1, 0, 0));
        vertices->emplace_back(Vector3(0.5f, -0.5f, -0.5f) * size, Vector2(1, 0), Vector3(0, -1, 0) * size, Vector3(1, 0, 0));

        // back
        vertices->emplace_back(Vector3(-0.5f, -0.5f, 0.5f) * size, Vector2(1, 1), Vector3(0, 0, 1) * size, Vector3(0, 1, 0));
        vertices->emplace_back(Vector3(-0.5f, 0.5f, 0.5f) * size, Vector2(1, 0), Vector3(0, 0, 1) * size, Vector3(0, 1, 0));
        vertices->emplace_back(Vector3(0.5f, -0.5f, 0.5f) * size, Vector2(0, 1), Vector3(0, 0, 1) * size, Vector3(0, 1, 0));
        vertices->emplace_back(Vector3(0.5f, 0.5f, 0.5f) * size, Vector2(0, 0), Vector3(0, 0, 1) * size, Vector3(0, 1, 0));

        // top
        vertices->emplace_back(Vector3(-0.5f, 0.5f, 0.5f) * size, Vector2(0, 0), Vector3(0, 1, 0) * size, Vector3(1, 0, 0));
        vertices->emplace_back(Vector3(-0.5f, 0.5f, -0.5f) * size, Vector2(0, 1), Vector3(0, 1, 0) * size, Vector3(1, 0, 0));
        vertices->emplace_back(Vector3(0.5f, 0.5f, 0.5f) * size, Vector2(1, 0), Vector3(0, 1, 0) * size, Vector3(1, 0, 0));
        vertices->emplace_back(Vector3(0.5f, 0.5f, -0.5f) * size, Vector2(1, 1), Vector3(0, 1, 0) * size, Vector3(1, 0, 0));

        // left
        vertices->emplace_back(Vector3(-0.5f, -0.5f, 0.5f) * size, Vector2(0, 1), Vector3(-1, 0, 0) * size, Vector3(0, 1, 0));
        vertices->emplace_back(Vector3(-0.5f, 0.5f, 0.5f) * size, Vector2(0, 0), Vector3(-1, 0, 0) * size, Vector3(0, 1, 0));
        vertices->emplace_back(Vector3(-0.5f, -0.5f, -0.5f) * size, Vector2(1, 1), Vector3(-1, 0, 0) * size, Vector3(0, 1, 0));
        vertices->emplace_back(Vector3(-0.5f, 0.5f, -0.5f) * size, Vector2(1, 0), Vector3(-1, 0, 0) * size, Vector3(0, 1, 0));

        // right
        vertices->emplace_back(Vector3(0.5f, -0.5f, 0.5f) * size, Vector2(1, 1), Vector3(1, 0, 0) * size, Vector3(0, 1, 0));
        vertices->emplace_back(Vector3(0.5f, 0.5f, 0.5f) * size, Vector2(1, 0), Vector3(1, 0, 0) * size, Vector3(0, 1, 0));
        vertices->emplace_back(Vector3(0.5f, -0.5f, -0.5f) * size, Vector2(0, 1), Vector3(1, 0, 0) * size, Vector3(0, 1, 0));
        vertices->emplace_back(Vector3(0.5f, 0.5f, -0.5f) * size, Vector2(0, 0), Vector3(1, 0, 0) * size, Vector3(0, 1, 0));

        // front
        indices->emplace_back(0); indices->emplace_back(1); indices->emplace_back(2);
        indices->emplace_back(2); indices->emplace_back(1); indices->emplace_back(3);

        // bottom
        indices->emplace_back(4); indices->emplace_back(5); indices->emplace_back(6);
        indices->emplace_back(6); indices->emplace_back(5); indices->emplace_back(7);

        // back
        indices->emplace_back(10); indices->emplace_back(9); indices->emplace_back(8);
        indices->emplace_back(11); indices->emplace_back(9); indices->emplace_back(10);

        // top
        indices->emplace_back(14); indices->emplace_back(13); indices->emplace_back(12);
        indices->emplace_back(15); indices->emplace_back(13); indices->emplace_back(14);

        // left
        indices->emplace_back(16); indices->emplace_back(17); indices->emplace_back(18);
        indices->emplace_back(18); indices->emplace_back(17); indices->emplace_back(19);

        // right
        indices->emplace_back(22); indices->emplace_back(21); indices->emplace_back(20);
        indices->emplace_back(23); indices->emplace_back(21); indices->emplace_back(22);
    }

    static void CreateQuad(std::vector<RHI_Vertex_PosTexNorTan>* vertices, std::vector<uint32_t>* indices)
    {
        using namespace Math;

        vertices->emplace_back(Vector3(-0.5f, 0.0f, 0.5f),  Vector2(0, 0), Vector3(0, 1, 0), Vector3(1, 0, 0)); // 0 top-left
        vertices->emplace_back(Vector3(0.5f,  0.0f, 0.5f),  Vector2(1, 0), Vector3(0, 1, 0), Vector3(1, 0, 0)); // 1 top-right
        vertices->emplace_back(Vector3(-0.5f, 0.0f, -0.5f), Vector2(0, 1), Vector3(0, 1, 0), Vector3(1, 0, 0)); // 2 bottom-left
        vertices->emplace_back(Vector3(0.5f,  0.0f, -0.5f), Vector2(1, 1), Vector3(0, 1, 0), Vector3(1, 0, 0)); // 3 bottom-right

        indices->emplace_back(3);
        indices->emplace_back(2);
        indices->emplace_back(0);
        indices->emplace_back(3);
        indices->emplace_back(0);
        indices->emplace_back(1);
    }

    static void CreateSphere(std::vector<RHI_Vertex_PosTexNorTan>* vertices, std::vector<uint32_t>* indices, float radius = 1.0f, int slices = 20, int stacks = 20)
    {
        using namespace Math;

        Vector3 normal = Vector3(0, 1, 0);
        Vector3 tangent = Vector3(1, 0, 0);
        vertices->emplace_back(Vector3(0, radius, 0), Vector2::Zero, normal, tangent);

        const float phiStep   = Helper::PI / stacks;
        const float thetaStep = 2.0f * Helper::PI / slices;

        for (int i = 1; i <= stacks - 1; i++)
        {
            const float phi = i * phiStep;
            for (int j = 0; j <= slices; j++)
            {
                const float theta = j * thetaStep;
                Vector3 p = Vector3(
                    (radius * sin(phi) * cos(theta)),
                    (radius * cos(phi)),
                    (radius * sin(phi) * sin(theta))
                );

                Vector3 t = Vector3(-radius * sin(phi) * sin(theta), 0, radius * sin(phi) * cos(theta)).Normalized();
                Vector3 n = p.Normalized();
                Vector2 uv = Vector2(theta / (Helper::PI * 2), phi / Helper::PI);
                vertices->emplace_back(p, uv, n, t);
            }
        }

        normal = Vector3(0, -1, 0);
        tangent = Vector3(1, 0, 0);
        vertices->emplace_back(Vector3(0, -radius, 0), Vector2(0, 1), normal, tangent);

        for (int i = 1; i <= slices; i++)
        {
            indices->emplace_back(0);
            indices->emplace_back(i + 1);
            indices->emplace_back(i);
        }
        int baseIndex = 1;
        const int ringVertexCount = slices + 1;
        for (int i = 0; i < stacks - 2; i++)
        {
            for (int j = 0; j < slices; j++)
            {
                indices->emplace_back(baseIndex + i * ringVertexCount + j);
                indices->emplace_back(baseIndex + i * ringVertexCount + j + 1);
                indices->emplace_back(baseIndex + (i + 1) * ringVertexCount + j);

                indices->emplace_back(baseIndex + (i + 1) * ringVertexCount + j);
                indices->emplace_back(baseIndex + i * ringVertexCount + j + 1);
                indices->emplace_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
            }
        }
        int southPoleIndex = (int)vertices->size() - 1;
        baseIndex = southPoleIndex - ringVertexCount;
        for (int i = 0; i < slices; i++)
        {
            indices->emplace_back(southPoleIndex);
            indices->emplace_back(baseIndex + i);
            indices->emplace_back(baseIndex + i + 1);
        }
    }

    static void CreateCylinder(std::vector<RHI_Vertex_PosTexNorTan>* vertices, std::vector<uint32_t>* indices, float radiusTop = 1.0f, float radiusBottom = 1.0f, float height = 1.0f, int slices = 15, int stacks = 15)
    {
        using namespace Math;

        const float stackHeight = height / stacks;
        const float radiusStep = (radiusTop - radiusBottom) / stacks;
        const float ringCount = (float)(stacks + 1);

        for (int i = 0; i < ringCount; i++)
        {
            const float y = -0.5f * height + i * stackHeight;
            const float r = radiusBottom + i * radiusStep;
            const float dTheta = 2.0f * Helper::PI / slices;
            for (int j = 0; j <= slices; j++)
            {
                const float c = cos(j * dTheta);
                const float s = sin(j * dTheta);

                Vector3 v = Vector3(r*c, y, r*s);
                Vector2 uv = Vector2((float)j / slices, 1.0f - (float)i / stacks);
                Vector3 t = Vector3(-s, 0.0f, c);

                const float dr = radiusBottom - radiusTop;
                Vector3 bitangent = Vector3(dr*c, -height, dr*s);

                Vector3 n = Vector3::Cross(t, bitangent).Normalized();
                vertices->emplace_back(v, uv, n, t);

            }
        }

        const int ringVertexCount = slices + 1;
        for (int i = 0; i < stacks; i++)
        {
            for (int j = 0; j < slices; j++)
            {
                indices->push_back(i * ringVertexCount + j);
                indices->push_back((i + 1) * ringVertexCount + j);
                indices->push_back((i + 1) * ringVertexCount + j + 1);

                indices->push_back(i * ringVertexCount + j);
                indices->push_back((i + 1) * ringVertexCount + j + 1);
                indices->push_back(i * ringVertexCount + j + 1);
            }
        }

        // Build top cap
        int baseIndex = (int)vertices->size();
        float y = 0.5f * height;
        const float dTheta = 2.0f * Helper::PI / slices;

        Vector3 normal;
        Vector3 tangent;

        for (int i = 0; i <= slices; i++)
        {
            const float x = radiusTop * cos(i*dTheta);
            const float z = radiusTop * sin(i*dTheta);
            const float u = x / height + 0.5f;
            const float v = z / height + 0.5f;

            normal = Vector3(0, 1, 0);
            tangent = Vector3(1, 0, 0);
            vertices->emplace_back(Vector3(x, y, z), Vector2(u, v), normal, tangent);
        }

        normal = Vector3(0, 1, 0);
        tangent = Vector3(1, 0, 0);
        vertices->emplace_back(Vector3(0, y, 0), Vector2(0.5f, 0.5f), normal, tangent);

        int centerIndex = (int)vertices->size() - 1;
        for (int i = 0; i < slices; i++)
        {
            indices->push_back(centerIndex);
            indices->push_back(baseIndex + i + 1);
            indices->push_back(baseIndex + i);
        }

        // Build bottom cap
        baseIndex = (int)vertices->size();
        y = -0.5f * height;

        for (int i = 0; i <= slices; i++)
        {
            const float x = radiusBottom * cos(i * dTheta);
            const float z = radiusBottom * sin(i * dTheta);
            const float u = x / height + 0.5f;
            const float v = z / height + 0.5f;

            normal =     Vector3(0, -1, 0);
            tangent        = Vector3(1, 0, 0);
            vertices->emplace_back(Vector3(x, y, z), Vector2(u, v), normal, tangent);
        }

        normal        = Vector3(0, -1, 0);
        tangent        = Vector3(1, 0, 0);
        vertices->emplace_back(Vector3(0, y, 0), Vector2(0.5f, 0.5f), normal, tangent);

        centerIndex = (int)vertices->size() - 1;
        for (int i = 0; i < slices; i++)
        {
            indices->push_back(centerIndex);
            indices->push_back(baseIndex + i);
            indices->push_back(baseIndex + i + 1);
        }
    }

    static void CreateCone(std::vector<RHI_Vertex_PosTexNorTan>* vertices, std::vector<uint32_t>* indices, float radius = 1.0f, float height = 2.0f)
    {
        CreateCylinder(vertices, indices, 0.0f, radius, height);
    }
}
