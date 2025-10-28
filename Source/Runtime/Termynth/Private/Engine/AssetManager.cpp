#include "Engine/AssetManager.h"
#include "Core/Logger.h"

#include <algorithm>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;


AssetManager& AssetManager::Get()
{
    static AssetManager instance;
    return instance;
}

void AssetManager::Initialize(const fs::path& engineContentDir, const fs::path& projectContentDir)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_EngineContentDir = engineContentDir;
    m_ProjectContentDir = projectContentDir;
    m_Initialized = true;
}

void AssetManager::Shutdown() 
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_LoadedAssets.clear();
    m_PakFiles.clear();
    m_Loaders.clear();
    m_Initialized = false;
}

bool AssetManager::MountPak(const std::string& pakPath)
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    auto pak = std::make_unique<PakFile>();
    if (pak->Open(pakPath))
    {
        m_PakFiles.push_back(std::move(pak));
        return true;
    }
    return false;
}

void AssetManager::UnmountPak(const std::string& pakPath)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_PakFiles.erase(
        std::remove_if(m_PakFiles.begin(), m_PakFiles.end(),
        [&](const auto& pak) {return pak->GetPakPath() == pakPath;}),
        m_PakFiles.end()
    );

}

void AssetManager::RegisterLoader(std::shared_ptr<IAssetLoader> loader)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Loaders.push_back(loader);
}

std::shared_ptr<IAsset> AssetManager::LoadAssetInternal(const std::string& path)
{
    std::string normalizedPath = NormalizePath(path);

    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        auto it = m_LoadedAssets.find(normalizedPath);
        if (it != m_LoadedAssets.end())
        {
            if (auto asset = it->second.lock())
            {
                return asset;
            }
        }
    }

    std::vector<uint8> data = ReadAssetData(normalizedPath);
    if (data.empty())
    {
        return nullptr;
    }

    IAssetLoader* loader = FindLoader(normalizedPath);
    if (!loader)
    {
        LOG_WARNING("Could not find loader for the asset: " + normalizedPath);
        return nullptr;
    }
  

    auto asset = loader->Load(data, normalizedPath);
    if (!asset)
    {
        return nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_LoadedAssets[normalizedPath] = asset;
    }
    return asset;
}

std::vector<uint8> AssetManager::ReadAssetData(const std::string& path)
{
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        for (auto& pak : m_PakFiles)
        {
            if (pak->Contains(path))
            {
                return pak->ReadAsset(path);
            }
        }
    }

    fs::path filePath = path;
    fs::path remainingPath;
    std::string engineDirName = "Engine";
    std::string projectDirName = "Game";
    std::string contentSource = "";

    bool bFound = false;

    for (const auto& component : filePath)
    {
        if (component == engineDirName && !bFound)
        {
            contentSource = engineDirName;
            bFound = true;
        }
        else if (component == projectDirName && !bFound)
        {
            contentSource = projectDirName;
            bFound = true;
        }

        else if (bFound)
        {
            remainingPath /= component;
        }
    }
    fs::path fullPath;
    if (contentSource == "Engine")
    {
        fullPath = fs::absolute(m_EngineContentDir) / remainingPath;
    }
    else if (contentSource == "Game")
    {
        fullPath = fs::absolute(m_ProjectContentDir) / remainingPath;
    }
    else
    {
        LOG_ERROR("Could not read asset data. Invalid asset path: missing 'Engine' or 'Game' root");
        return{};
    }
    std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        LOG_ERROR("Could not open asset file: " + fullPath.string());
        return {};
    }

    size_t size = file.tellg();
    file.seekg(0);

    std::vector<uint8> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);

    return data;

}

IAssetLoader* AssetManager::FindLoader(const std::string& path)
{
    std::string ext = fs::path(path).extension().string();

    std::lock_guard<std::mutex> lock(m_Mutex);
    for (auto& loader : m_Loaders)
    {
        if (loader->CanLoad(ext))
        {
            return loader.get();
        }
    }
    return nullptr;
}

std::string AssetManager::NormalizePath(const std::string& path) 
{
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    return normalized;
}

void AssetManager::UnloadAsset(const std::string& path) 
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    std::string normalizedPath = NormalizePath(path);
    
    auto it = m_LoadedAssets.find(normalizedPath);
    if (it != m_LoadedAssets.end() && it->second.expired()) 
    {
        m_LoadedAssets.erase(it);
    }
}

void AssetManager::UnloadAllAssets() 
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_LoadedAssets.clear();
}

bool AssetManager::IsAssetLoaded(const std::string& path) const 
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    std::string normalizedPath = const_cast<AssetManager*>(this)->NormalizePath(path);
    
    auto it = m_LoadedAssets.find(normalizedPath);
    return it != m_LoadedAssets.end() && !it->second.expired();
}

size_t AssetManager::GetLoadedAssetCount() const 
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_LoadedAssets.size();
}

size_t AssetManager::GetTotalMemoryUsage() const 
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    size_t total = 0;
    
    for (const auto& pair : m_LoadedAssets) 
    {
        if (auto asset = pair.second.lock()) 
        {
            total += asset->GetMemorySize();
        }
    }
    
    return total;
}