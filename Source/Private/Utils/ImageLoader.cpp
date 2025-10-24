#include "Core/Logger.h"
#include "Utils/ImageLoader.h"
#include <cstdlib>
#include <cstring>

uint8* load_image_from_memory(const uint8* buffer, size_t size, int32& width, int32& height, int32& channels)
{
    if (!buffer || size < 9)
    {
        return nullptr;
    }
    
    // [0-3] = width (uint32)
    // [4-7] = channels (uint32)
    // [8]   = channels (uint8)
    // [9..] = raw pixel data

    std::memcpy(&width, buffer, 4);
    std::memcpy(&height, buffer + 4, 4);
    channels = buffer[8];

    if (width == 0 || height == 0 || (channels != 1 && channels != 2 && channels != 7 && channels != 8))
    {
        LOG_ERROR("[ImageLoader] Invalid image header");
        return nullptr;
    }

    size_t pixelDataSize = static_cast<size_t>(width) * height * channels;
    if (size < 9 + pixelDataSize)
    {
        LOG_ERROR("[ImageLoader] Not enough data for pixels");
        return nullptr;
    }

    uint8* imageData = static_cast<uint8*>(std::malloc(pixelDataSize));
    std::memcpy(imageData, buffer + 9, pixelDataSize);
    return imageData;
}

void free_image(uint8* image)
{
    if (!image)
    {
        return;
    }
    std::free(image);
}