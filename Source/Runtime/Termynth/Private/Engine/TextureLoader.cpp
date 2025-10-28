#include "Engine/TextureLoader.h"
#include "Engine/TextureAsset.h"
#include "Utils/ImageLoader.h"
#include "Core/Logger.h"

#include "Render/Texture.h"

std::shared_ptr<IAsset> TextureLoader::Load(const std::vector<uint8>& data, const std::string& path)
{
    auto texture = std::make_shared<TextureAsset>(path);

    int32 w = 0,h = 0;
    int32 channels = 0;
    
    uint8* imageData = load_image_from_memory(data.data(), data.size(), w, h, channels);
    if (!imageData)
    {
        LOG_ERROR("[TextureLoader] Could not load texture with path: '" + path + "'");
        return nullptr;
    }
    TextureDesc desc;

    switch (channels)
    {
    case 3:
        desc.Format = ETextureFormat::DefaultText;
        break;
    case 4:
        desc.Format = ETextureFormat::AttributesText;
        break;
    case 9: 
        desc.Format = ETextureFormat::ColoredText;
        break;
    case 10:
        desc.Format = ETextureFormat::AttributesColoredText;
        break;
    default:
        LOG_ERROR("[TextureLoader] Wrong texture format. Could not load texture with path: '" + path + "'");
        return nullptr;
    }

    desc.Width = w;
    desc.Height = h;
    desc.DebugName = path;

    texture->m_Width = w;
    texture->m_Height = h;
    texture->m_Channels = channels;
    texture->m_Data = std::make_shared<AsciiTexture>(desc, imageData);
    texture->m_Path = path;
    texture->m_Loaded = true;

    free_image(imageData);

    return texture;
}

bool TextureLoader::CanLoad(const std::string& extension) const
{
    return extension == ".thtx";
}