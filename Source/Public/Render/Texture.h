#pragma once
#include "Core/Core.h"
#include <string>

enum class ETextureFormat : uint8
{
    DefaultText,
    AttributesText,
    ColoredText,
    AttributesColoredText
};


struct TextureDesc
{
    uint32 Width = 0;
    uint32 Height = 0;
    ETextureFormat Format = ETextureFormat::AttributesColoredText;
    std::string DebugName;
};


class Texture
{
public:
    virtual ~Texture() = default;

    virtual uint32 GetWidth() const = 0;
    virtual uint32 GetHeight() const = 0;
    virtual ETextureFormat GetFormat() const = 0;
    virtual uint32 GetSize() const = 0;

    const std::string& GetDebugName() const {return m_Desc.DebugName;}
protected:
    TextureDesc m_Desc;
};

class AsciiTexture final : public Texture
{
public:
    AsciiTexture(const TextureDesc& desc, const uint8* data = nullptr);
    AsciiTexture(const std::string& path);
    ~AsciiTexture() override;

    virtual uint32 GetWidth() const override { return m_Desc.Width;}
    virtual uint32 GetHeight() const override { return m_Desc.Height;}
    virtual ETextureFormat GetFormat() const override {return m_Desc.Format;}
    virtual uint32 GetSize() const override;

    const uint8* GetData() const {return m_Data;}

private:
    const uint8* m_Data;
};