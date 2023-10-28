
#pragma once

//= INCLUDES ===================
#include <string>
#include <memory>
#include "Runtime/Function/Renderer/RHI/RHI_Definitions.h"
//==============================

enum class IconType
{
    Component_Options,
    Component_AudioListener,
    Component_AudioSource,
    Component_ReflectionProbe,
    Component_Camera,
    Component_Light,
    Component_Material,
    Component_Material_RemoveTexture,
    Component_MeshCollider,
    Component_Renderable,
    Component_PhysicsBody,
    Component_SoftBody,
    Component_Terrain,
    Component_Environment,
    Component_Transform,
    Console_Info,
    Console_Warning,
    Console_Error,
    Button_Play,
    Button_Profiler,
    Button_ResourceCache,
    Button_RenderDoc,
    Directory_Folder,
    Directory_File_Audio,
    Directory_File_World,
    Directory_File_Model,
    Directory_File_Default,
    Directory_File_Material,
    Directory_File_Shader,
    Directory_File_Xml,
    Directory_File_Dll,
    Directory_File_Txt,
    Directory_File_Ini,
    Directory_File_Exe,
    Directory_File_Font,
    Directory_File_Texture,
    Screenshot,
    Undefined
};

class Icon
{
public:
    Icon() = default;
    Icon(IconType type, const std::string& file_path);

    LitchiRuntime::RHI_Texture* GetTexture() const;
    void SetTexture(std::shared_ptr<LitchiRuntime::RHI_Texture> texture);
    std::string GetFilePath() const;
    IconType GetType() const { return m_type; }

private:
    IconType m_type = IconType::Undefined;
    std::shared_ptr<LitchiRuntime::RHI_Texture> m_texture;
};

class IconLoader
{
public:
    static void Initialize();

    static LitchiRuntime::RHI_Texture* GetTextureByType(IconType type);
    static Icon* LoadFromFile(const std::string& filePath, IconType type = IconType::Undefined);
};
