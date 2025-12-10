#pragma once
#include "Engine/AssetTypes.h"
#include "Engine/TextureAsset.h"

class SpriteAsset : public IAsset
{
public:
    struct Rect
    {
        int32 x, y, width, height;
    };

    static TAssetPtr<SpriteAsset> Create(TAssetPtr<TextureAsset> texture, const Rect& rect);
    static TAssetPtr<SpriteAsset> Create(TAssetPtr<TextureAsset> texture);

    SpriteAsset(const std::string& path) : m_Path(path), m_Loaded(false){}
    SpriteAsset(TAssetPtr<TextureAsset> texture, const Rect& rect);

    const std::string& GetPath() const override {return m_Path;}
    bool IsLoaded() const override {return m_Loaded && m_Texture.IsValid();}
    size_t GetMemorySize() const override {return sizeof(SpriteAsset);}

    TAssetPtr<TextureAsset> GetTexture() const {return m_Texture;}
    const Rect& GetRect() const {return m_Rect;}


private:
    friend class SpriteLoader;
    std::string m_Path;
    TAssetPtr<TextureAsset> m_Texture;
    Rect m_Rect;
    bool m_Loaded = false;
};