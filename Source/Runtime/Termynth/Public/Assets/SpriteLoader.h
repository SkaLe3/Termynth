#pragma once

#include "Assets/AssetTypes.h"

class SpriteLoader : public IAssetLoader
{
public:
    std::shared_ptr<IAsset> Load(const std::vector<uint8>& data, const std::string& path) override;
    bool CanLoad(const std::string& extension) const override;

};