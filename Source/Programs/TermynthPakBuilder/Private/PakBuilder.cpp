#include "PakBuilder.h"
#include "Engine/PakFile.h"
#include "Core/Logger.h"

#include <iostream>
#include <algorithm>

PakBuilder::PakBuilder()
{
    // Don't compress already compressed formats
    m_CompressionRules[".png"] = false;
    m_CompressionRules[".jpg"] = false;
    m_CompressionRules[".jpeg"] = false;
    m_CompressionRules[".ogg"] = false;
    m_CompressionRules[".mp3"] = false;

    // Compress text-based formats
    m_CompressionRules[".thsp"] = true;
    m_CompressionRules[".thtx"] = true;
    m_CompressionRules[".txt"] = true;
    m_CompressionRules[".xml"] = true;
    m_CompressionRules[".json"] = true;

    // Compress uncompressed audio
    m_CompressionRules[".wav"] = true;
}

bool PakBuilder::AddFile(const std::string &assetPath, const std::string &diskPath, bool bCompress)
{
    if (!fs::exists(diskPath))
    {
        LOG_ERROR("File not found: " + diskPath);
        return false;
    }

    FileEntry entry;
    entry.AssetPath = NormalizePath(assetPath);
    entry.DiskPath = diskPath;
    entry.bCompress = bCompress;

    if (!ReadFile(diskPath, entry.Data))
    {
        LOG_ERROR("Failed to read file: " + diskPath);
        return false;
    }

    if (!bCompress)
    {
        std::string ext = fs::path(diskPath).extension().string();
        entry.bCompress = ShouldCompress(ext);
    }

    m_Entries.push_back(entry);
    return true;
}

bool PakBuilder::AddDirectory(const std::string &contentDir, const std::string &baseAssetPath)
{
    if (!fs::exists(contentDir) || !fs::is_directory(contentDir))
    {
        LOG_ERROR("Directory not found: " + contentDir);
        return false;
    }

    size_t addedCount = 0;

    for (const auto &entry : fs::recursive_directory_iterator(contentDir))
    {
        if (entry.is_regular_file())
        {
            // Relative path from content directory
            fs::path relativePath = fs::relative(entry.path(), contentDir);
            std::string assetPath = baseAssetPath.empty()
                                        ? relativePath.string()
                                        : baseAssetPath + "/" + relativePath.string();
            if (AddFile(assetPath, entry.path().string()))
            {
                addedCount++;
            }
        }
    }

    if (addedCount > 0)
    {
        LOG_INFO("Added " + std::to_string(addedCount) + " files from directory: " + contentDir);
    }
    else
    {
        LOG_INFO("Directory: '" + contentDir + "' does not contain any files");
    }
    return addedCount > 0;
}

void PakBuilder::SetCompressionForExtension(const std::string &ext, bool compress)
{
    std::string normalized = ext;
    if (normalized[0] != '.')
    {
        normalized = "." + normalized;
    }
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    m_CompressionRules[normalized] = compress;
}

bool PakBuilder::Build(const std::string &outputPakPath)
{
    if (m_Entries.empty())
    {
        LOG_ERROR("No files to pack!");
        return false;
    }
    std::ofstream file(outputPakPath, std::ios::binary);
    if (!file.is_open())
    {
        LOG_ERROR("Failed to create pak file: " + outputPakPath);
        return false;
    }

    LOG_INFO("Building PAK file: " + outputPakPath);
    LOG_INFO("Files to pack: " + std::to_string(m_Entries.size()));

    // Write magic header
    const char magic[4] = {'T', 'P', 'A', 'K'};
    file.write(magic, 4);

    // Write entry count
    uint32_t entryCount = static_cast<uint32_t>(m_Entries.size());
    file.write(reinterpret_cast<const char *>(&entryCount), sizeof(entryCount));

    // Calculate header size
    size_t headerSize = 8; // magic + entry count
    for (const auto &entry : m_Entries)
    {
        headerSize += sizeof(uint32_t);         // path length
        headerSize += entry.AssetPath.length(); // path
        headerSize += sizeof(uint64_t) * 3;     // offset, size, uncopressed size
        headerSize += sizeof(bool);             // compressed flag
    }

    // Prepare entries with data offsets
    struct PackedEntry
    {
        std::string Path;
        size_t Offset;
        std::vector<uint8_t> Data;
        size_t UncompressedSize;
        bool bCompressed;
    };

    std::vector<PackedEntry> PackedEntries;
    size_t currentOffset = headerSize;

    for (auto &entry : m_Entries)
    {
        PackedEntry packed;
        packed.Path = entry.AssetPath;
        packed.Offset = currentOffset;
        packed.UncompressedSize = entry.Data.size();
        packed.bCompressed = entry.bCompress;

        if (entry.bCompress)
        {
            if (PakFile::Compress(entry.Data, packed.Data))
            {
                LOG_INFO("  Compressed: " + entry.AssetPath 
                        + " (" + std::to_string(entry.Data.size()) + " -> " + std::to_string(packed.Data.size())
                        + " bytes, " + std::to_string(100 * packed.Data.size() / entry.Data.size()) + "%)");
            }
            else
            {
                // Compression failed, store uncompressed
                packed.Data = entry.Data;
                packed.bCompressed = false;
                LOG_INFO("  Stored: " + entry.AssetPath + " (compression failed)");
            }
        }
        else
        {
            packed.Data = entry.Data;
            LOG_INFO("  Stored: " + entry.AssetPath + " (" + std::to_string(packed.Data.size()) + " bytes)");
        }

        currentOffset += packed.Data.size();
        PackedEntries.push_back(std::move(packed));
    }

    // Write entries header
    for (const auto &entry : PackedEntries)
    {
        uint32_t pathLen = static_cast<uint32_t>(entry.Path.length());
        file.write(reinterpret_cast<const char *>(&pathLen), sizeof(pathLen));
        file.write(entry.Path.c_str(), pathLen);

        file.write(reinterpret_cast<const char *>(&entry.Offset), sizeof(entry.Offset));

        size_t dataSize = entry.Data.size();
        file.write(reinterpret_cast<const char *>(&dataSize), sizeof(dataSize));
        file.write(reinterpret_cast<const char *>(&entry.UncompressedSize), sizeof(entry.UncompressedSize));
        file.write(reinterpret_cast<const char *>(&entry.bCompressed), sizeof(entry.bCompressed));
    }

    for (const auto &entry : PackedEntries)
    {
        file.write(reinterpret_cast<const char *>(entry.Data.data()), entry.Data.size());
    }

    file.close();

    LOG_INFO("\nPAK file created successfully!");
    LOG_INFO("Total files: " + std::to_string(PackedEntries.size()));
    LOG_INFO("Uncompressed size: " + std::to_string(GetTotalSize()) + " bytes");
    LOG_INFO("Packed size: " + std::to_string(fs::file_size(outputPakPath)) + " bytes");

    return true;
}

size_t PakBuilder::GetTotalSize() const
{
    size_t total = 0;
    for (const auto &entry : m_Entries)
    {
        total += entry.Data.size();
    }
    return total;
}

size_t PakBuilder::GetComressedSize() const
{
    // Calculate after compression
    return 0;
}

void PakBuilder::Clear()
{
    m_Entries.clear();
}

bool PakBuilder::ReadFile(const std::string &path, std::vector<uint8_t> &outData)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        return false;
    }

    size_t size = file.tellg();
    file.seekg(0);

    outData.resize(size);
    file.read(reinterpret_cast<char *>(outData.data()), size);
    return true;
}

std::string PakBuilder::NormalizePath(const std::string &path)
{
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');

    while (!normalized.empty() && normalized[0] == '/')
    {
        normalized.erase(0, 1);
    }
    return normalized;
}

bool PakBuilder::ShouldCompress(const std::string &extension)
{
    std::string ext = extension;
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    auto it = m_CompressionRules.find(ext);
    if (it != m_CompressionRules.end())
    {
        return it->second;
    }
    return true;
}
