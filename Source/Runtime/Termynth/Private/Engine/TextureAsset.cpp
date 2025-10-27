#include "Engine/TextureAsset.h"
#include "Render/Texture.h"

size_t TextureAsset::GetMemorySize() const
{
    return m_Data->GetSize();
}