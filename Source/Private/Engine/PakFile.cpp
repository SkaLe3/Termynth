#include "Engine/PakFile.h"

#include <fstream>
#include <algorithm>
#include <filesystem>

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
        // TODO: Implement decompression
    }

    return data;
}