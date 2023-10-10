///*
//Copyright(c) 2016-2023 Panos Karabelas
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
//copies of the Software, and to permit persons to whom the Software is furnished
//to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
//FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
//COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
//IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//*/
//
//#pragma once
//
////= INCLUDES =====================
//#include "../Resource/IResource.h"
//#include "../Math/Matrix.h"
////================================
//
//namespace LitchiRuntime
//{
//    struct AnimationVertexWeight
//    {
//        uint32_t vertexID;
//        float weight;
//    };
//
//    struct AnimationBone
//    {
//        std::string name;
//        std::vector<AnimationVertexWeight> vertexWeights;
//        Matrix offset;
//    };
//
//    struct KeyVector
//    {
//        double time;
//        Vector3 value;
//    };
//
//    struct KeyQuaternion
//    {
//        double time;
//        Math::Quaternion value;
//    };
//
//    struct AnimationNode
//    {
//        std::string name;
//        std::vector<KeyVector> positionFrames;
//        std::vector<KeyQuaternion> rotationFrames;
//        std::vector<KeyVector> scaleFrames;
//    };
//
//    class SP_CLASS Animation : public IResource
//    {
//    public:
//        Animation();
//        ~Animation() = default;
//
//        //= IResource ==========================================
//        bool LoadFromFile(const std::string& filePath) override;
//        bool SaveToFile(const std::string& filePath) override;
//        //======================================================
//
//        void SetObjectName(const std::string& name)   { m_object_name = name; }
//        void SetDuration(double duration)       { m_duration = duration; }
//        void SetTicksPerSec(double ticksPerSec) { m_ticksPerSec = ticksPerSec; }
//
//    private:
//        std::string m_object_name;
//        double m_duration    = 0;
//        double m_ticksPerSec = 0;
//
//        // Each channel controls a single node
//        std::vector<AnimationNode> m_channels;
//    };
//}
