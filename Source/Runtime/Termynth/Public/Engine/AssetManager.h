#pragma once
#include "Core/Core.h"
#include "Engine/AssetTypes.h"
#include "Engine/PakFile.h"

#include <string>
#include <memory>

class AssetManager 
{
public:
    static AssetManager& Get();

    void Initialize(const fs::path& engineContentDir, const fs::path& projectContentDir);
    void Shutdown();

    bool MountPak(const std::string& pakPath);
    void UnmountPak(const std::string& pakPath);

    void RegisterLoader(std::shared_ptr<IAssetLoader> loader);

    template<typename T>
    TAssetPtr<T> LoadAsset(const std::string& path);

    void UnloadAsset(const std::string& path);

    void UnloadAllAssets();
    bool IsAssetLoaded(const std::string& path) const;
    size_t GetLoadedAssetCount() const;
    size_t GetTotalMemoryUsage() const;

private:
    AssetManager() = default;
    ~AssetManager() {Shutdown();}

    std::shared_ptr<IAsset> LoadAssetInternal(const std::string& path);
    std::vector<uint8> ReadAssetData(const std::string& path);
    IAssetLoader* FindLoader(const std::string& path);
    std::string NormalizePath(const std::string& path);

private:
     fs::path m_EngineContentDir;
     fs::path m_ProjectContentDir;
    std::vector<std::unique_ptr<PakFile>> m_PakFiles;
    std::vector<std::shared_ptr<IAssetLoader>> m_Loaders;
    std::unordered_map<std::string, std::weak_ptr<IAsset>> m_LoadedAssets;
    mutable std::mutex m_Mutex;
    bool m_Initialized = false;
};

template<typename T>
TAssetPtr<T> AssetManager::LoadAsset(const std::string& path)
{
    auto asset = LoadAssetInternal(path);
    return TAssetPtr<T>(std::static_pointer_cast<T>(asset));
}



