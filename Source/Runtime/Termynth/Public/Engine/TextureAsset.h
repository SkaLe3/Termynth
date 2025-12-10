#pragma once
#include "Engine/AssetTypes.h"
#include <memory>

class Texture;

class TextureAsset : public IAsset
{
public:
    static TAssetPtr<TextureAsset> Create(const std::string& textureData);

    TextureAsset(const std::string& path) : m_Path(path) , m_Loaded(false) {}

    const std::string& GetPath() const override {return m_Path;}
    bool IsLoaded() const override {return m_Loaded;}
    size_t GetMemorySize() const override;

    int32 GetWidth() const {return m_Width;}
    int32 GetHeight() const {return m_Height;}
    int32 GetChannels() const {return m_Channels;}
    std::shared_ptr<Texture> GetData() const {return m_Data;}
private:
    TextureAsset();

private:
    friend class TextureLoader;
    std::string m_Path;
    std::shared_ptr<Texture> m_Data;
    int32 m_Width = 0;
    int32 m_Height = 0;
    int32 m_Channels = 0;
    bool m_Loaded = false;
};