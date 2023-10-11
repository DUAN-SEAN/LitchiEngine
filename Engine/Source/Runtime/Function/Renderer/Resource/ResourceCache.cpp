
//= INCLUDES =========================
#include "Runtime/Core/pch.h"
#include "ResourceCache.h"
#include "../World/World.h"
#include "../IO/FileStream.h"
#include "../RHI/RHI_Texture2D.h"
#include "../RHI/RHI_Texture2DArray.h"
#include "../RHI/RHI_TextureCube.h"
#include "../Audio/AudioClip.h"
#include "../Rendering/Mesh.h"
#include "Runtime/Core/Display/Event.h"
//====================================

//= NAMESPACES ================
using namespace std;
using namespace LitchiRuntime::Math;
//=============================

namespace LitchiRuntime
{
    namespace
    {
        static array<string, 6> m_standard_resource_directories;
        static string m_project_directory;
        static vector<shared_ptr<IResource>> m_resources;
        static mutex m_mutex;
    }

    void ResourceCache::Initialize()
    {
        // Create project directory
        SetProjectDirectory("project\\");

        // Add engine standard resource directories
        const string data_dir = "data\\";
        AddResourceDirectory(ResourceDirectory::Environment,    m_project_directory + "environment");
        AddResourceDirectory(ResourceDirectory::Fonts,          data_dir + "fonts");
        AddResourceDirectory(ResourceDirectory::Icons,          data_dir + "icons");
        AddResourceDirectory(ResourceDirectory::ShaderCompiler, data_dir + "shader_compiler");
        AddResourceDirectory(ResourceDirectory::Shaders,        data_dir + "shaders");
        AddResourceDirectory(ResourceDirectory::Textures,       data_dir + "textures");

        // Subscribe to events
        SP_SUBSCRIBE_TO_EVENT(EventType::WorldSaveStart, SP_EVENT_HANDLER_STATIC(SaveResourcesToFiles));
        SP_SUBSCRIBE_TO_EVENT(EventType::WorldLoadStart, SP_EVENT_HANDLER_STATIC(LoadResourcesFromFiles));
        SP_SUBSCRIBE_TO_EVENT(EventType::WorldClear,     SP_EVENT_HANDLER_STATIC(Shutdown));
    }

    bool ResourceCache::IsCached(const string& resource_name, const ResourceType resource_type)
    {
        SP_ASSERT(!resource_name.empty());

        lock_guard<mutex> guard(m_mutex);

        for (shared_ptr<IResource>& resource : m_resources)
        {
            if (resource->GetResourceType() != resource_type)
                continue;

            if (resource_name == resource->GetObjectName())
                return true;
        }

        return false;
    }

    bool ResourceCache::IsCached(const uint64_t resource_id)
    {
        lock_guard<mutex> guard(m_mutex);

        for (shared_ptr<IResource>& resource : m_resources)
        {
            if (resource_id == resource->GetObjectId())
                return true;
        }

        return false;
    }
    
	shared_ptr<IResource>& ResourceCache::GetByName(const string& name, const ResourceType type)
    {
        lock_guard<mutex> guard(m_mutex);

        for (shared_ptr<IResource>& resource : m_resources)
        {
            if (name == resource->GetObjectName())
                return resource;
        }

        static shared_ptr<IResource> empty;
        return empty;
    }

    vector<shared_ptr<IResource>> ResourceCache::GetByType(const ResourceType type /*= ResourceType::Unknown*/)
    {
        lock_guard<mutex> guard(m_mutex);

        vector<shared_ptr<IResource>> resources;
        for (shared_ptr<IResource>& resource : m_resources)
        {
            if (resource->GetResourceType() == type || type == ResourceType::Unknown)
            {
                resources.emplace_back(resource);
            }
        }

        return resources;
    }

    uint64_t ResourceCache::GetMemoryUsageCpu(ResourceType type /*= Resource_Unknown*/)
    {
        lock_guard<mutex> guard(m_mutex);

        uint64_t size = 0;
        for (shared_ptr<IResource>& resource : m_resources)
        {
            if (resource->GetResourceType() == type || type == ResourceType::Unknown)
            {
                if (SP_Object* object = dynamic_cast<SP_Object*>(resource.get()))
                {
                    size += object->GetObjectSizeCpu();
                }
            }
        }

        return size;
    }

    uint64_t ResourceCache::GetMemoryUsageGpu(ResourceType type /*= Resource_Unknown*/)
    {
        lock_guard<mutex> guard(m_mutex);

        uint64_t size = 0;
        for (shared_ptr<IResource>& resource : m_resources)
        {
            if (resource->GetResourceType() == type || type == ResourceType::Unknown)
            {
                if (SP_Object* object = dynamic_cast<SP_Object*>(resource.get()))
                {
                    size += object->GetObjectSizeGpu();
                }
            }
        }

        return size;
    }

    void ResourceCache::SaveResourcesToFiles()
    {
        // Create resource list file
        string file_path = GetProjectDirectoryAbsolute() + World::GetName() + "_resources.dat";
        auto file = make_unique<FileStream>(file_path, FileStream_Write);
        if (!file->IsOpen())
        {
            SP_LOG_ERROR("Failed to open file.");
            return;
        }

        const uint32_t resource_count = GetResourceCount();

        // Start progress report
        ProgressTracker::GetProgress(ProgressType::Resource).Start(resource_count, "Loading resources...");

        // Save resource count
        file->Write(resource_count);

        // Save all the currently used resources to disk
        for (shared_ptr<IResource>& resource : m_resources)
        {
            if (!resource->HasFilePathNative())
            {
                // Save file path
                file->Write(resource->GetResourceFilePathNative());
                // Save type
                file->Write(static_cast<uint32_t>(resource->GetResourceType()));
                // Save resource (to a dedicated file)
                resource->SaveToFile(resource->GetResourceFilePathNative());
            }

            // Update progress
            ProgressTracker::GetProgress(ProgressType::Resource).JobDone();
        }
    }

    void ResourceCache::LoadResourcesFromFiles()
    {
        // Open resource list file
        string file_path = GetProjectDirectoryAbsolute() + World::GetName() + "_resources.dat";
        unique_ptr<FileStream> file = make_unique<FileStream>(file_path, FileStream_Read);
        if (!file->IsOpen())
            return;

        // Load resource count
        const uint32_t resource_count = file->ReadAs<uint32_t>();

        for (uint32_t i = 0; i < resource_count; i++)
        {
            // Load resource file path
            string file_path = file->ReadAs<string>();

            // Load resource type
            const ResourceType type = static_cast<ResourceType>(file->ReadAs<uint32_t>());

            switch (type)
            {
            case ResourceType::Mesh:
                Load<Mesh>(file_path);
                break;
            case ResourceType::Material:
                Load<Material>(file_path);
                break;
            case ResourceType::Texture:
                Load<RHI_Texture>(file_path);
                break;
            case ResourceType::Texture2d:
                Load<RHI_Texture2D>(file_path);
                break;
            case ResourceType::Texture2dArray:
                Load<RHI_Texture2DArray>(file_path);
                break;
            case ResourceType::TextureCube:
                Load<RHI_TextureCube>(file_path);
                break;
            case ResourceType::Audio:
                Load<AudioClip>(file_path);
                break;
            }
        }

    }

    void ResourceCache::Shutdown()
    {
        uint32_t resource_count = static_cast<uint32_t>(m_resources.size());
        m_resources.clear();
        DEBUG_LOG_INFO("%d resources have been cleared", resource_count);
    }

    uint32_t ResourceCache::GetResourceCount(const ResourceType type)
    {
        return static_cast<uint32_t>(GetByType(type).size());
    }

    void ResourceCache::AddResourceDirectory(const ResourceDirectory type, const string& directory)
    {
        m_standard_resource_directories[static_cast<uint32_t>(type)] = directory;
    }

    string ResourceCache::GetResourceDirectory(const ResourceDirectory type)
    {
        return m_standard_resource_directories[static_cast<uint32_t>(type)];
    }

    void ResourceCache::SetProjectDirectory(const string& directory)
    {
        if (!FileSystem::Exists(directory))
        {
            FileSystem::CreateDirectory(directory);
        }

        m_project_directory = directory;
    }

    string ResourceCache::GetProjectDirectoryAbsolute()
    {
        return FileSystem::GetWorkingDirectory() + "/" + m_project_directory;
    }

    const string& ResourceCache::GetProjectDirectory()
    {
        return m_project_directory;
    }

    string ResourceCache::GetDataDirectory()
    {
        return "Data";
    }

    vector<shared_ptr<IResource>>& ResourceCache::GetResources()
    {
        return m_resources;
    }

    mutex& ResourceCache::GetMutex()
    {
        return m_mutex;
    }
}
