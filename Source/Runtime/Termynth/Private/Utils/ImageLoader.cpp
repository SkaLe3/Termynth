#include "Core/Logger.h"
#include "Utils/ImageLoader.h"
#include <cstdlib>
#include <cstring>

uint8 *load_image_from_memory(const uint8 *buffer, size_t size, int32 &width, int32 &height, int32 &channels)
{
    if (!buffer || size < 9)
    {
        return nullptr;
    }

    // [0-3] = magic - "THTX" (uint32)
    // [4-7] = width (uint32)
    // [8-11] = height (uint32)
    // [12]   = channels (uint8)
    // [13..] = raw pixel data
    int32 magic;

    std::memcpy(&magic, buffer, 4);
    std::memcpy(&width, buffer + 4, 4);
    std::memcpy(&height, buffer + 8, 4);
    std::memcpy(&channels, buffer + 12, 1);

    if (width == 0 || height == 0 || (channels != 3 && channels != 4 && channels != 9 && channels != 10))
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

    uint8 *imageData = static_cast<uint8 *>(std::malloc(pixelDataSize));
    std::memcpy(imageData, buffer + 13, pixelDataSize);
    return imageData;
}

uint8 *load_image_from_text(const std::string &buffer, int32 &width, int32 &height, int32 &channels)
{
    const size_t CELL_SIZE = 3;
    channels = CELL_SIZE;
    std::vector<std::vector<uint8_t>> rows;
    std::vector<uint8_t> currentRow;

    size_t maxWidth = 0;

    for (char c : buffer)
    {
        if (c == '\n')
        {
            maxWidth = std::max(maxWidth, currentRow.size() / CELL_SIZE);
            rows.push_back(std::move(currentRow));
            currentRow.clear();
        }
        else
        {
            uint8_t cell[CELL_SIZE] = {}; // zero initialize everything
            cell[0] = static_cast<uint8_t>(c); // Char[0]
            cell[1] = 0;                        // Char[1]
            cell[2] = (c == '\t') ? 1 : 0;      // bTransparent
            currentRow.insert(currentRow.end(), cell, cell + CELL_SIZE);
        }
    }

     // Push last row if not empty
    if (!currentRow.empty())
    {
        maxWidth = std::max(maxWidth, currentRow.size() / CELL_SIZE);
        rows.push_back(std::move(currentRow));
    }

    height = rows.size();
    width = maxWidth;

    // Flatten rows and pad each row to maxWidth
    std::vector<uint8_t> data;
    data.reserve(height * maxWidth * CELL_SIZE);

    uint8_t emptyCell[CELL_SIZE] = {'\t', 0, 1}; // for padding

    for (auto& row : rows)
    {
        size_t cellsInRow = row.size() / CELL_SIZE;
        data.insert(data.end(), row.begin(), row.end());

        // Pad row to maxWidth
        for (size_t i = cellsInRow; i < maxWidth; ++i)
        {
            data.insert(data.end(), emptyCell, emptyCell + CELL_SIZE);
        }
    }
    uint8* rawData = static_cast<uint8*>(std::malloc(data.size() * sizeof(uint8_t)));
    std::memcpy(rawData, data.data(), data.size() * sizeof(uint8_t));
    return rawData;
}
void free_image(uint8 *image)
{
    if (!image)
    {
        return;
    }
    std::free(image);
}