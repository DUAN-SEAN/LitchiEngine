
#pragma once

//= INCLUDES ======================
#include <string>
#include "Runtime/Core/Definitions.h"
//=================================

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

    private:
        static void ParseNode(const aiNode* node, GameObject* parent_entity = nullptr);
        static void ParseNodeMeshes(const aiNode* node, GameObject* new_entity);
        static void ParseNodeLight(const aiNode* node, GameObject* new_entity);
        static void ParseAnimations();
        static void ParseMesh(aiMesh* mesh, GameObject* entity_parent);
        static void ParseNodes(const aiMesh* mesh);
    };
}
