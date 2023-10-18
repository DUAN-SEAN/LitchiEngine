
#pragma once

//= INCLUDES ======================
#include <string>
#include "Runtime/Core/Definitions.h"
//=================================

struct aiNode;
struct aiMesh;

namespace LitchiRuntime
{
    class Entity;
    class Mesh;

    class SP_CLASS ModelImporter
    {
    public:
        static void Initialize();
        static bool Load(Mesh* mesh, const std::string& file_path);

    private:
        static void ParseNode(const aiNode* node, std::shared_ptr<Entity> parent_entity = nullptr);
        static void ParseNodeMeshes(const aiNode* node, Entity* new_entity);
        static void ParseNodeLight(const aiNode* node, Entity* new_entity);
        static void ParseAnimations();
        static void ParseMesh(aiMesh* mesh, Entity* entity_parent);
        static void ParseNodes(const aiMesh* mesh);
    };
}
