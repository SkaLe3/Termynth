#include "Render/Sprite.h"

std::shared_ptr<Sprite> Sprite::Create(TAssetPtr<SpriteAsset> asset, iVector2 origin)
{
    return std::shared_ptr<Sprite>(new Sprite(asset, origin));
}
Sprite::Sprite(TAssetPtr<SpriteAsset> asset, iVector2 origin)
    : m_Sprite(asset), m_Origin(origin)
{
}