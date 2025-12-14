#include "Assets/PakFile.h"
#include "Core/Logger.h"

#include <fstream>
#include <algorithm>
#include <filesystem>

#include <zlib.h>

namespace fs = std::filesystem;

bool PakFile::Open(const std::string& pakPath)
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    std::ifstream file(pakPath, std::ios::binary);
    if (!file.is_open()) return false;

    char magic[4];
    file.read(magic, 4);
    if (std::string(magic, 4) != "PAK1")
    {
        return false;
    }

    uint32 entryCount;
    file.read(reinterpret_cast<char*>(&entryCount), sizeof(entryCount));

    for (uint32 i = 0; i < entryCount; ++i)
    {
        Entry entry;

        // Read asset path
        uint32_t pathLen;
        file.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));
        entry.Path.resize(pathLen);
        file.read(&entry.Path[0], pathLen);

        // Read asset entry data
        file.read(reinterpret_cast<char*>(&entry.Offset), sizeof(entry.Offset));
        file.read(reinterpret_cast<char*>(&entry.Size), sizeof(entry.Size));
        file.read(reinterpret_cast<char*>(&entry.UncompressedSize), sizeof(entry.UncompressedSize));
        file.read(reinterpret_cast<char*>(&entry.Compressed), sizeof(entry.Compressed));

        m_Entries[entry.Path] = entry;
    }

    m_PakPath = pakPath;
    m_bIsOpen = true;
    return true;
}

void PakFile::Close()
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Entries.clear();
    m_bIsOpen = false;
}

bool PakFile::Contains(const std::string& assetPath) const 
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_Entries.find(assetPath) != m_Entries.end();
}

std::vector<uint8> PakFile::ReadAsset(const std::string& assetPath)
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    auto it = m_Entries.find(assetPath);
    if (it == m_Entries.end()) return {};

    const Entry& entry = it->second;

    std::ifstream file(m_PakPath, std::ios::binary);
    if (!file.is_open()) return {};

    file.seekg(entry.Offset);
    std::vector<uint8> data(entry.Size);
    file.read(reinterpret_cast<char*>(data.data()), entry.Size);

    if (entry.Compressed)
    {
        std::vector<uint8> unpackedData;
        if (Decompress(data, unpackedData, entry.UncompressedSize))
        {
            data = unpackedData;
        }
        else
        {
            LOG_ERROR("Failed to decompress: " + entry.Path);
        }
    }

    return data;
}
bool PakFile::Compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output)
{
    if (input.empty())
        return false;

    output.clear();
    output.resize(compressBound(input.size())); // allocate max possible size

    uLongf compressedSize = output.size();
    int32_t result = compress2(
        output.data(),
        &compressedSize,
        input.data(),
        input.size(),
        Z_BEST_COMPRESSION);

    if (result != Z_OK)
    {
        LOG_ERROR("  zlib compression failed");
        return false;
    }
    output.resize(compressedSize);

    return compressedSize < input.size();
}

bool PakFile::Decompress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output, size_t expectedSize)
{
    if (input.empty() || expectedSize == 0)
        return false;

    output.resize(expectedSize);
    uLongf destLen = expectedSize;
    int32 result = uncompress(
        output.data(),
        &destLen,
        input.data(),
        input.size()
    );

    if (result != Z_OK)
    {
        LOG_ERROR("  zlib decompression failed");
        return false;
    }
    return true;
}
