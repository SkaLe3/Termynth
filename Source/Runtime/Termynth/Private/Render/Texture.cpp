#include "Render/Texture.h"
#include <cstring>

uint8* AllocateTexture(uint32 format, uint32 width, uint32 height, const uint8* data)
{
    uint8* allocatedMemory =(uint8*)std::malloc(height * width * format);
    std::memcpy(allocatedMemory, data, format * width * height);
    return allocatedMemory;
}


uint32 TextureFormatToByteSize(ETextureFormat format)
{
    switch (format)
    {
    case ETextureFormat::DefaultText:
        return 3;
    case ETextureFormat::AttributesText:
        return 4;
    case ETextureFormat::ColoredText:
        return 9;
    case ETextureFormat::AttributesColoredText:
        return 10;
    default:
        return 0;
    }
}

AsciiTexture::AsciiTexture(const TextureDesc& desc, const uint8* data)
{
    m_Desc = desc;
    m_BytesInCell = TextureFormatToByteSize(desc.Format);
    m_Data = AllocateTexture(m_BytesInCell, desc.Width, desc.Height, data);
    
}

AsciiTexture::~AsciiTexture()
{
    std::free((void*)m_Data);
}

uint32 AsciiTexture::GetSize() const
{
    return m_Desc.Width * m_Desc.Height * TextureFormatToByteSize(m_Desc.Format);
}