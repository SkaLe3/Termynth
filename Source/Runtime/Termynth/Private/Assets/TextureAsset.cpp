#include "Assets/TextureAsset.h"
#include "Render/Texture.h"
#include "Utils/ImageLoader.h"

TAssetPtr<TextureAsset> TextureAsset::Create(const std::string &textureData)
{
    TAssetPtr<TextureAsset> texture = TAssetPtr<TextureAsset>(std::shared_ptr<TextureAsset>(new TextureAsset));
    
    int32 w = 0,h = 0;
    int32 channels = 0;
    
    uint8* imageData = load_image_from_text(textureData, w, h, channels);

    TextureDesc desc;
    desc.Format = ETextureFormat::DefaultText;
    desc.Width = w;
    desc.Height = h;

    texture->m_Width = w;
    texture->m_Height = h;
    texture->m_Channels = channels;
    texture->m_Data = std::make_shared<AsciiTexture>(desc, imageData);
    texture->m_Path = "";
    texture->m_Loaded = true;

    free_image(imageData);

    return texture;
}

size_t TextureAsset::GetMemorySize() const
{
    return m_Data->GetSize();
}
TextureAsset::TextureAsset()
    : m_Loaded(true)   
{

}