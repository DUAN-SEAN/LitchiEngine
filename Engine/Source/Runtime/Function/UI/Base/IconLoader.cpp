
//= INCLUDES ======================
#include "IconLoader.h"
#include "Runtime/Function/Renderer/Resource/ResourceCache.h"
#include "Runtime/Function/Renderer/RHI/RHI_Texture2D.h"
//#include "Runtime/Core/ThreadPool.h"
//#include "Event.h"
//=================================

//= NAMESPACES =========
using namespace std;
using namespace LitchiRuntime;
//======================

namespace
{
    // Using shared_ptr because the vector can re-allocate at is grows, and call the destructor of Icon.
    // If this happens when the Icon's texture is being loaded in the thread pool, we'll get a crash.
    static vector<shared_ptr<Icon>> icons;
    static Icon no_icon;
    static mutex icon_mutex;
}

static void destroy_rhi_resources()
{
    icons.clear();
}

static Icon* get_icon_by_type(IconType type)
{
    for (auto& icon : icons)
    {
        if (icon->GetType() == type)
            return icon.get();
    }

    return &no_icon;
}

Icon::Icon(IconType type, const string& file_path)
{
    m_type = type;

    // Create texture
    string name = FileSystem::GetFileNameFromFilePath(file_path);
    m_texture   = make_shared<RHI_Texture2D>(RHI_Texture_Srv, name.c_str());

    // Load texture
    m_texture->LoadFromFile(file_path);
}

RHI_Texture* Icon::GetTexture() const
{
    if (m_texture && m_texture->IsReadyForUse())
    {
        return m_texture.get();
    }

    return nullptr;
}

void Icon::SetTexture(shared_ptr<RHI_Texture> texture)
{
    m_texture = texture;
}

string Icon::GetFilePath() const
{
    return m_texture->GetResourceFilePath();
}

void IconLoader::Initialize()
{
    // SP_SUBSCRIBE_TO_EVENT(EventType::RendererOnShutdown, SP_EVENT_HANDLER_STATIC(destroy_rhi_resources));

    // ThreadPool::AddTask([&]()
    // {
        const string data_dir = ResourceCache::GetDataDirectory() + "\\";

        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\component_componentOptions.png", IconType::Component_Options);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\component_audioListener.png", IconType::Component_AudioListener);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\component_audioSource.png", IconType::Component_AudioSource);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\component_reflectionProbe.png", IconType::Component_ReflectionProbe);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\component_camera.png", IconType::Component_Camera);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\component_light.png", IconType::Component_Light);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\component_material.png", IconType::Component_Material);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\component_material_removeTexture.png", IconType::Component_Material_RemoveTexture);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\component_meshCollider.png", IconType::Component_MeshCollider);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\component_renderable.png", IconType::Component_Renderable);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\component_rigidBody.png", IconType::Component_PhysicsBody);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\component_softBody.png", IconType::Component_SoftBody);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\component_transform.png", IconType::Component_Transform);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\component_terrain.png", IconType::Component_Terrain);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\component_environment.png", IconType::Component_Environment);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\console_info.png", IconType::Console_Info);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\console_warning.png", IconType::Console_Warning);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\console_error.png", IconType::Console_Error);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\button_play.png", IconType::Button_Play);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\profiler.png", IconType::Button_Profiler);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\resource_cache.png", IconType::Button_ResourceCache);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\renderdoc.png", IconType::Button_RenderDoc);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\file.png", IconType::Directory_File_Default);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\folder.png", IconType::Directory_Folder);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\audio.png", IconType::Directory_File_Audio);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\model.png", IconType::Directory_File_Model);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\world.png", IconType::Directory_File_World);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\material.png", IconType::Directory_File_Material);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\shader.png", IconType::Directory_File_Shader);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\xml.png", IconType::Directory_File_Xml);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\dll.png", IconType::Directory_File_Dll);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\txt.png", IconType::Directory_File_Txt);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\ini.png", IconType::Directory_File_Ini);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\exe.png", IconType::Directory_File_Exe);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\font.png", IconType::Directory_File_Font);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\texture.png", IconType::Directory_File_Texture);
        IconLoader::LoadFromFile(data_dir + "Engine\\Icons\\screenshot.png", IconType::Screenshot);
    // });
}

RHI_Texture* IconLoader::GetTextureByType(IconType type)
{
    return LoadFromFile("", type)->GetTexture();
}

Icon* IconLoader::LoadFromFile(const string& file_path, IconType type /*Undefined*/)
{
    // Check if the texture is already loaded, and return that
    bool search_by_type = type != IconType::Undefined;
    for (auto& icon : icons)
    {
        if (search_by_type)
        {
            if (icon->GetType() == type)
                return icon.get();
        }
        else if (icon->GetFilePath() == file_path)
        {
            return icon.get();
        }
    }

    // The texture is new so load it
    if (FileSystem::IsSupportedImageFile(file_path) || FileSystem::IsEngineTextureFile(file_path))
    {
        lock_guard<mutex> guard(icon_mutex);

        // Add a new icon
        icons.push_back(make_shared<Icon>(type, file_path));

        // Return it
        return icons.back().get();
    }

    return get_icon_by_type(IconType::Directory_File_Default);
}
