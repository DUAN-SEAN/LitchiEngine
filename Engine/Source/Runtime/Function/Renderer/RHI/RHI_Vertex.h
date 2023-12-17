
#pragma once

//= INCLUDES ===============
#include "Runtime/Core/pch.h"
//==========================

namespace LitchiRuntime
{
    struct RHI_Vertex_Pos
    {
        RHI_Vertex_Pos(const Vector3& position)
        {
            this->pos[0] = position.x;
            this->pos[1] = position.y;
            this->pos[2] = position.z;
        }

        float pos[3] = { 0, 0, 0 };
    };

    struct RHI_Vertex_PosTex
    {
        RHI_Vertex_PosTex(const float pos_x, const float pos_y, const float pos_z, const float tex_x, const float tex_y)
        {
            pos[0] = pos_x;
            pos[1] = pos_y;
            pos[2] = pos_z;

            tex[0] = tex_x;
            tex[1] = tex_y;
        }

        RHI_Vertex_PosTex(const Vector3& pos, const Vector2& tex)
        {
            this->pos[0] = pos.x;
            this->pos[1] = pos.y;
            this->pos[2] = pos.z;

            this->tex[0] = tex.x;
            this->tex[1] = tex.y;
        }

        float pos[3] = { 0, 0, 0 };
        float tex[2] = { 0, 0 };
    };

    struct RHI_Vertex_PosCol
    {
        RHI_Vertex_PosCol() = default;

        RHI_Vertex_PosCol(const Vector3& pos, const Vector4& col)
        {
            this->pos[0] = pos.x;
            this->pos[1] = pos.y;
            this->pos[2] = pos.z;

            this->col[0] = col.x;
            this->col[1] = col.y;
            this->col[2] = col.z;
            this->col[3] = col.w;
        }

        float pos[3] = { 0, 0, 0 };
        float col[4] = { 0, 0, 0, 0};
    };

    struct RHI_Vertex_Pos2dTexCol8
    {
        RHI_Vertex_Pos2dTexCol8() = default;

        float pos[2] = { 0, 0 };
        float tex[2] = { 0, 0 };
        uint32_t col = 0;
    };

    struct RHI_Vertex_PosTexNorTan
    {
        RHI_Vertex_PosTexNorTan() = default;
        RHI_Vertex_PosTexNorTan(
            const Vector3& pos,
            const Vector2& tex,
            const Vector3& nor = Vector3::Zero,
            const Vector3& tan = Vector3::Zero)
        {
            this->pos[0] = pos.x;
            this->pos[1] = pos.y;
            this->pos[2] = pos.z;

            this->tex[0] = tex.x;
            this->tex[1] = tex.y;

            this->nor[0] = nor.x;
            this->nor[1] = nor.y;
            this->nor[2] = nor.z;

            this->tan[0] = tan.x;
            this->tan[1] = tan.y;
            this->tan[2] = tan.z;
        }

        float pos[3] = { 0, 0, 0 };
        float tex[2] = { 0, 0 };
        float nor[3] = { 0, 0, 0 };
        float tan[3] = { 0, 0, 0 };
    };

    struct RHI_Vertex_PosTexNorTanBone
    {
        RHI_Vertex_PosTexNorTanBone() = default;
        RHI_Vertex_PosTexNorTanBone(
            const Vector3& pos,
            const Vector2& tex,
            const Vector3& nor = Vector3::Zero,
            const Vector3& tan = Vector3::Zero,
            const Vector4& boneIndex = Vector4::Zero,
            const Vector3& boneWeight = Vector3::Zero)
        {
            this->pos[0] = pos.x;
            this->pos[1] = pos.y;
            this->pos[2] = pos.z;

            this->tex[0] = tex.x;
            this->tex[1] = tex.y;

            this->nor[0] = nor.x;
            this->nor[1] = nor.y;
            this->nor[2] = nor.z;

            this->tan[0] = tan.x;
            this->tan[1] = tan.y;
            this->tan[2] = tan.z;


            this->boneIndices[0] = boneIndex.x;
            this->boneIndices[1] = boneIndex.y;
            this->boneIndices[2] = boneIndex.z;
            this->boneIndices[3] = boneIndex.w;


            this->boneWeights[0] = boneWeight.x;
            this->boneWeights[1] = boneWeight.y;
            this->boneWeights[2] = boneWeight.z;
        }

        float pos[3] = { 0, 0, 0 };
        float tex[2] = { 0, 0 };
        float nor[3] = { 0, 0, 0 };
        float tan[3] = { 0, 0, 0 };
        
        float boneIndices[4] = { 0, 0, 0 ,0};
        float boneWeights[3] = { 0, 0, 0 };
    };

    SP_ASSERT_STATIC_IS_TRIVIALLY_COPYABLE(RHI_Vertex_Pos);
    SP_ASSERT_STATIC_IS_TRIVIALLY_COPYABLE(RHI_Vertex_PosTex);
    SP_ASSERT_STATIC_IS_TRIVIALLY_COPYABLE(RHI_Vertex_PosCol);
    SP_ASSERT_STATIC_IS_TRIVIALLY_COPYABLE(RHI_Vertex_Pos2dTexCol8);
    SP_ASSERT_STATIC_IS_TRIVIALLY_COPYABLE(RHI_Vertex_PosTexNorTan);
    SP_ASSERT_STATIC_IS_TRIVIALLY_COPYABLE(RHI_Vertex_PosTexNorTanBone);
}
