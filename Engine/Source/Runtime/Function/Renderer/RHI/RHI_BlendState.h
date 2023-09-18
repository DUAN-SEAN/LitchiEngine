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

#pragma once

//= INCLUDES =================
#include "RHI_Definitions.h"
#include "Runtime/Core/Meta/Reflection/object.h"
//============================

namespace Spartan
{
    class SP_CLASS RHI_BlendState : public LitchiRuntime::Object
    {
    public:
        RHI_BlendState(
            const bool blend_enabled                    = false,
            const RHI_Blend source_blend                = RHI_Blend::Src_Alpha,
            const RHI_Blend dest_blend                  = RHI_Blend::Inv_Src_Alpha,
            const RHI_Blend_Operation blend_op          = RHI_Blend_Operation::Add,
            const RHI_Blend source_blend_alpha          = RHI_Blend::One,
            const RHI_Blend dest_blend_alpha            = RHI_Blend::One,
            const RHI_Blend_Operation blend_op_alpha    = RHI_Blend_Operation::Add,
            const float blend_factor                    = 0.0f
        );
        ~RHI_BlendState();

        auto GetBlendEnabled()                        const { return m_blend_enabled; }
        auto GetSourceBlend()                         const { return m_source_blend; }
        auto GetDestBlend()                           const { return m_dest_blend; }
        auto GetBlendOp()                             const { return m_blend_op; }
        auto GetSourceBlendAlpha()                    const { return m_source_blend_alpha; }
        auto GetDestBlendAlpha()                      const { return m_dest_blend_alpha; }
        auto GetBlendOpAlpha()                        const { return m_blend_op_alpha; }
        void SetBlendFactor(const float blend_factor)       { m_blend_factor = blend_factor; }
        float GetBlendFactor()                        const { return m_blend_factor; }

        uint64_t GetHash() const { return m_hash; }

        bool operator==(const RHI_BlendState& rhs) const
        {
            return m_hash == rhs.m_hash;
        }

        void* GetRhiResource() const { return m_rhi_resource; }

    private:
        bool m_blend_enabled                 = false;
        RHI_Blend m_source_blend             = RHI_Blend::Src_Alpha;
        RHI_Blend m_dest_blend               = RHI_Blend::Inv_Src_Alpha;
        RHI_Blend_Operation m_blend_op       = RHI_Blend_Operation::Add;
        RHI_Blend m_source_blend_alpha       = RHI_Blend::One;
        RHI_Blend m_dest_blend_alpha         = RHI_Blend::One;
        RHI_Blend_Operation m_blend_op_alpha = RHI_Blend_Operation::Add;
        float m_blend_factor                 = 1.0f;

        uint64_t m_hash      = 0;
        void* m_rhi_resource = nullptr;
    };
}
