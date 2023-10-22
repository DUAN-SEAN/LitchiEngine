
#pragma once

//= INCLUDES =====================
#include "../Resource/IResource.h"
#include "Runtime/Core/Math/Matrix.h"
//================================

namespace LitchiRuntime
{
    struct AnimationVertexWeight
    {
        uint32_t vertexID;
        float weight;
    };

    struct AnimationBone
    {
        std::string name;
        std::vector<AnimationVertexWeight> vertexWeights;
        Matrix offset;
    };

    struct KeyVector
    {
        double time;
        Vector3 value;
    };

    struct KeyQuaternion
    {
        double time;
        Quaternion value;
    };

    struct AnimationNode
    {
        std::string name;
        std::vector<KeyVector> positionFrames;
        std::vector<KeyQuaternion> rotationFrames;
        std::vector<KeyVector> scaleFrames;
    };

    class SP_CLASS Animation : public IResource
    {
    public:
        Animation();
        ~Animation() = default;

        //= IResource ==========================================
        bool LoadFromFile(const std::string& filePath) override;
        bool SaveToFile(const std::string& filePath) override;
        //======================================================

        void SetObjectName(const std::string& name)   { m_object_name = name; }
        void SetDuration(double duration)       { m_duration = duration; }
        void SetTicksPerSec(double ticksPerSec) { m_ticksPerSec = ticksPerSec; }

    private:
        std::string m_object_name;
        double m_duration    = 0;
        double m_ticksPerSec = 0;

        // Each channel controls a single node
        std::vector<AnimationNode> m_channels;
    };
}
