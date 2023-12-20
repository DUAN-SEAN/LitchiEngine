
#pragma once

//= INCLUDES ======================
#include <string>
#include "Runtime/Core/Definitions.h"
//=================================

#define NUM_BONES_PER_VERTEX 4

struct aiNode;
struct aiMesh;

namespace LitchiRuntime
{
    class GameObject;
    class Mesh;

    class SP_CLASS ModelImporter
    {
    public:
        static void Initialize();
        static bool Load(Mesh* mesh, const std::string& file_path);

        struct BoneData {
            uint32_t boneIndex[NUM_BONES_PER_VERTEX];
            float weights[NUM_BONES_PER_VERTEX];
            void Add(uint32_t boneID, float weight) {
                for (size_t i = 0; i < NUM_BONES_PER_VERTEX; i++) {
                    if (weights[i] == 0.0f) {
                        boneIndex[i] = boneID;
                        weights[i] = weight;
                        return;
                    }
                }
                //insert error program
                // DEBUG_LOG_ERROR("bone index out of size");
            }
        };

    private:
        static void ReadNodeHierarchy(const aiNode* node, uint32_t parentIndex);
        static void ParseNode(const aiNode* node, GameObject* parent_entity = nullptr);
        static void ParseNodeMeshes(const aiNode* node, GameObject* new_entity);
        static void ParseNodeLight(const aiNode* node, GameObject* new_entity);
        static void ParseMesh(aiMesh* assimp_mesh, GameObject* entity_parent);
        static void ParseMeshWithBone(aiMesh* assimp_mesh, GameObject* entity_parent);
        static void ParseMeshWithoutBone(aiMesh* assimp_mesh, GameObject* entity_parent);
        static void ParseBones(const aiMesh* assimp_mesh, std::vector<BoneData>& boneData);
        static void ParseAnimations();
    public:
    };
}
