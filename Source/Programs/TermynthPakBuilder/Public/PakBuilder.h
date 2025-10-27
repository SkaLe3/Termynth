#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

class PakBuilder
{
public:
    struct FileEntry
    {
        std::string AssetPath;
        std::string DiskPath;
        std::vector<uint8_t> Data;
        bool bCompress;
    };

    PakBuilder();

    bool AddFile(const std::string& assetPath, const std::string& diskPath, bool bCompress = false);
    bool AddDirectory(const std::string& contentDir, const std::string& baseAssetPath = "");
    void SetCompressionForExtension(const std::string& ext, bool compress);
    bool Build(const std::string& outputPakPath);

    size_t GetFileCount() const {return m_Entries.size(); }
    size_t GetTotalSize() const;
    size_t GetComressedSize() const;

    void Clear();

private:
    bool ReadFile(const std::string& path, std::vector<uint8_t>& outData);
    std::string NormalizePath(const std::string& path);
    bool ShouldCompress(const std::string& extension);


private:
    std::vector<FileEntry> m_Entries;
    std::unordered_map<std::string, bool> m_CompressionRules;

};