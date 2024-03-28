
#pragma once

//= INCLUDES ==================
#include <memory>
#include <atomic>
#include "Runtime/Core/Tools/FileSystem/FileSystem.h"
#include "Runtime/Core/Meta/Reflection/object.h"
#include "Runtime/Core/Log/debug.h"
//=============================

namespace LitchiRuntime
{
    enum class ResourceType
    {
        Texture,
        Texture2d,
        Texture2dArray,
        TextureCube,
        Audio,
        Material,
        Mesh,
        Cubemap,
        Animation,
        Font,
        Shader,
        Prefab,
        Unknown,
    };

    class LC_CLASS IResource : public Object
    {
    public:
        IResource(ResourceType type);
        virtual ~IResource() = default;

        void SetResourceFilePath(const std::string& obsoultePath)
        {
            const bool is_native_file = LitchiRuntime::FileSystem::IsEngineMaterialFile(obsoultePath) || LitchiRuntime::FileSystem::IsEngineModelFile(obsoultePath) || LitchiRuntime::FileSystem::IsEnginePrefabFile(obsoultePath);

            // If this is an native engine file, don't do a file check as no actual foreign material exists (it was created on the fly)
            if (!is_native_file)
            {
                if (!LitchiRuntime::FileSystem::IsFile(obsoultePath))
                {
                    DEBUG_LOG_ERROR("{} is not a valid file path", obsoultePath.c_str());
                    return;
                }
            }

            const std::string file_path_relative = LitchiRuntime::FileSystem::GetRelativePath(obsoultePath);

            //// Foreign file
            //if (!LitchiRuntime::FileSystem::IsEngineFile(path))
            //{
                /*m_resource_file_path_foreign    = file_path_relative;
                m_resource_file_path_native     = LitchiRuntime::FileSystem::NativizeFilePath(file_path_relative);*/
        		m_resource_file_path_foreign = obsoultePath;
                m_resource_file_path_native = obsoultePath;
                m_resource_file_path_asset = LitchiRuntime::FileSystem::GetRelativePathAssetFromNative(obsoultePath);

            //}
            //// Native file
            //else
            //{
            //    m_resource_file_path_foreign.clear();
            //    m_resource_file_path_native = file_path_relative;
            //}
                
            m_object_name        = LitchiRuntime::FileSystem::GetFileNameWithoutExtensionFromFilePath(obsoultePath);
            m_resource_directory = LitchiRuntime::FileSystem::GetDirectoryFromFilePath(obsoultePath);
        }
        
        ResourceType GetResourceType()                 const { return m_resource_type; }
        const char* GetResourceTypeCstr()              const { return typeid(*this).name(); }
        bool HasFilePathNative()                       const { return !m_resource_file_path_native.empty(); }
        const std::string& GetResourceFilePath()       const { return m_resource_file_path_foreign; }
        const std::string& GetResourceFilePathNative() const { return m_resource_file_path_native; }
        const std::string& GetResourceDirectory()      const { return m_resource_directory; }
        const std::string& GetResourceFilePathAsset()      const { return m_resource_file_path_asset; }

        // Flags
        void SetFlag(const uint32_t flag, bool enabled = true)
        {
            if (enabled)
            {
                m_flags |= flag;
            }
            else
            {
                m_flags &= ~flag;
            }
        }
        uint32_t GetFlags()           const { return m_flags; }
        void SetFlags(const uint32_t flags) { m_flags = flags; }

        // Misc
        bool IsReadyForUse() const { return m_is_ready_for_use; }

        // IO
        virtual bool SaveToFile(const std::string& file_path) { return true; }
        virtual bool LoadFromFile(const std::string& file_path) { return true; }

        // Type
        template <typename T>
        static constexpr ResourceType TypeToEnum();

    protected:
        ResourceType m_resource_type         = ResourceType::Unknown;
        std::atomic<bool> m_is_ready_for_use = false;
        uint32_t m_flags                     = 0;

    private:
        std::string m_resource_directory;
        std::string m_resource_file_path_native;
        std::string m_resource_file_path_foreign;
        std::string m_resource_file_path_asset;
    };
}
