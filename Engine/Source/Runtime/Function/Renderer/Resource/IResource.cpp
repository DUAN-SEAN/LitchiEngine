
//= INCLUDES =========================
#include "Runtime/Core/pch.h"
#include "IResource.h"
#include "../RHI/RHI_Texture2D.h"
#include "../RHI/RHI_Texture2DArray.h"
#include "../RHI/RHI_TextureCube.h"
//====================================

//= NAMESPACES ==========
using namespace std;
using namespace LitchiRuntime;
//=======================

IResource::IResource(const ResourceType type)
{
    m_resource_type = type;
}

template <typename T>
inline constexpr ResourceType IResource::TypeToEnum() { return ResourceType::Unknown; }

template<typename T>
inline constexpr void validate_resource_type() { static_assert(std::is_base_of<IResource, T>::value, "Provided type does not implement IResource"); }

// Explicit template instantiation
#define INSTANTIATE_TO_RESOURCE_TYPE(T, enumT) template<> SP_CLASS ResourceType IResource::TypeToEnum<T>() { validate_resource_type<T>(); return enumT; }

// To add a new resource to the engine, simply register it here
INSTANTIATE_TO_RESOURCE_TYPE(RHI_Texture,           ResourceType::Texture)
INSTANTIATE_TO_RESOURCE_TYPE(RHI_Texture2D,         ResourceType::Texture2d)
INSTANTIATE_TO_RESOURCE_TYPE(RHI_Texture2DArray,    ResourceType::Texture2dArray)
INSTANTIATE_TO_RESOURCE_TYPE(RHI_TextureCube,       ResourceType::TextureCube)
