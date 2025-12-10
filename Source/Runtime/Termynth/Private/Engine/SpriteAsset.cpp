#include "Engine/SpriteAsset.h"

TAssetPtr<SpriteAsset> SpriteAsset::Create(TAssetPtr<TextureAsset> texture, const Rect& rect)
{
    return TAssetPtr<SpriteAsset>(std::make_shared<SpriteAsset>(texture, rect));
}

TAssetPtr<SpriteAsset> SpriteAsset::Create(TAssetPtr<TextureAsset> texture)
{
    return TAssetPtr<SpriteAsset>(std::make_shared<SpriteAsset>(texture, Rect{0, 0, texture->GetWidth(), texture->GetHeight()}));
}
SpriteAsset::SpriteAsset(TAssetPtr<TextureAsset> texture, const Rect &rect)
    : m_Texture(texture), m_Rect(rect), m_Loaded(true)
{

}