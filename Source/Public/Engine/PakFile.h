#pragma once
#include "Core/Core.h"

#include <string>
#include <unordered_map>
#include <mutex>
#include <vector>

class PakFile
{
public:
    struct Entry
    {
        std::string Path;
        size_t Offset;
        size_t Size;
        size_t UncompressedSize;
        bool Compressed;
    };

    PakFile() = default;
    ~PakFile() {Close();}

    bool Open(const std::string& pakPath);
    void Close();
    bool Contains(const std::string& assetPath) const;
    std::vector<uint8> ReadAsset(const std::string& assetPath);
    std::string GetPakPath() const {return m_PakPath;}
private:
    std::string m_PakPath;
    std::unordered_map<std::string, Entry> m_Entries;
    mutable std::mutex m_Mutex;
    bool m_bIsOpen = false;
};