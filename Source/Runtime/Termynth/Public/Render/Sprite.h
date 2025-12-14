#pragma once
#include "Core/Math/Math.h"
#include "Assets/SpriteAsset.h"

class Sprite
{
public:
    static std::shared_ptr<Sprite> Create(TAssetPtr<SpriteAsset> asset, iVector2 origin = {0, 0});
    const iVector2& GetOrigin() {return m_Origin;}
    iVector2 GetRectStart() {return iVector2( m_Sprite->GetRect().x,  m_Sprite->GetRect().y);}
    iVector2 GetRectSize() {return iVector2(m_Sprite->GetRect().width, m_Sprite->GetRect().height);} 
    Texture* GetTexture() {return m_Sprite->GetTexture()->GetData().get();}
private:
    Sprite(TAssetPtr<SpriteAsset> asset, iVector2 origin);
private:
    TAssetPtr<SpriteAsset> m_Sprite;
    iVector2 m_Origin;
};