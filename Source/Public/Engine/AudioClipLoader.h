#pragma once
#include "Engine/AssetTypes.h"

class AudioLoader : public IAssetLoader
{
public:
    std::shared_ptr<IAsset> Load(const std::vector<uint8>& data, const std::string& path);
    bool CanLoad(const std::string& extension) const override;
};