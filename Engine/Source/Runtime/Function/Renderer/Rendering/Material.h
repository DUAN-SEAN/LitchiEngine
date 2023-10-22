
#pragma once

//= INCLUDES ======================
#include <memory>
#include <array>
#include "../RHI/RHI_Definitions.h"
#include "../Resource/IResource.h"
//=================================

namespace LitchiRuntime
{
    enum class MaterialTexture
    {
        Color,
        Roughness,  // Specifies microfacet roughness of the surface for diffuse and specular reflection
        Metalness, // Blends between a non-metallic and metallic material model
        Normal,
        Occlusion,  // A texture that will be mixed with ssao.
        Emission,   // A texture that will cause a surface to be lit, works nice with bloom.
        Height,     // Perceived depth for parallax mapping.
        AlphaMask,  // A texture which will use pixel shader discards for transparent pixels.
        Undefined
    };

    enum class MaterialProperty
    {
        Clearcoat,           // Extra white specular layer on top of others
        Clearcoat_Roughness, // Roughness of clearcoat specular
        Anisotropic,         // Amount of anisotropy for specular reflection
        AnisotropicRotation, // Rotates the direction of anisotropy, with 1.0 going full circle
        Sheen ,              // Amount of soft velvet like reflection near edges
        SheenTint,           // Mix between white and using base color for sheen reflection
        ColorTint,           // Diffuse or metal surface color
        ColorR,
        ColorG,
        ColorB,
        ColorA,
        RoughnessMultiplier,
        MetalnessMultiplier,
        NormalMultiplier,
        HeightMultiplier,
        UvTilingX,
        UvTilingY,
        UvOffsetX,
        UvOffsetY,
        SingleTextureRoughnessMetalness,
        CanBeEdited,
        Undefined
    };

    class SP_CLASS Material : public IResource
    {
    public:
        Material();
        ~Material() = default;

        //= IResource ===========================================
        bool LoadFromFile(const std::string& file_path) override;
        bool SaveToFile(const std::string& file_path) override;
        //=======================================================

        //= TEXTURES  ================================================================================
        void SetTexture(const MaterialTexture texture_type, RHI_Texture* texture);
        void SetTexture(const MaterialTexture texture_type, std::shared_ptr<RHI_Texture> texture);
        void SetTexture(const MaterialTexture texture_type, std::shared_ptr<RHI_Texture2D> texture);
        void SetTexture(const MaterialTexture texture_type, std::shared_ptr<RHI_TextureCube> texture);
        bool HasTexture(const std::string& path) const;
        bool HasTexture(const MaterialTexture texture_type) const;
        std::string GetTexturePathByType(const MaterialTexture texture_type);
        std::vector<std::string> GetTexturePaths();
        RHI_Texture* GetTexture(const MaterialTexture texture_type);
        std::shared_ptr<RHI_Texture>& GetTexture_PtrShared(const MaterialTexture texturtexture_type);
        //============================================================================================
        
        float GetProperty(const MaterialProperty property_type) const { return m_properties[static_cast<uint32_t>(property_type)]; }
        void SetProperty(const MaterialProperty property_type, const float value);

        void SetColor(const Color& color);
 
    private:
        std::array<std::shared_ptr<RHI_Texture>, 9> m_textures;
        std::array<float, 22> m_properties;
    };
}
