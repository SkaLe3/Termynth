#include "Engine/SpriteLoader.h"
#include "Engine/SpriteAsset.h"
#include "Engine/AssetManager.h"

#include <sstream>

std::shared_ptr<IAsset> SpriteLoader::Load(const std::vector<uint8>& data, const std::string& path)
{
    auto sprite = std::make_shared<SpriteAsset>(path);

    std::string content(data.begin(), data.end());
    std::istringstream iss(content);

    std::string texturePath;
    std::getline(iss, texturePath);

    int32 x, y, w, h;
    iss >> x >> y >> w >> h;

    sprite->m_Texture = AssetManager::Get().LoadAsset<TextureAsset>(texturePath);
    sprite->m_Rect = {x, y, w, h};
    sprite->m_Loaded = true;

    return sprite;
}

bool SpriteLoader::CanLoad(const std::string& extension) const
{
    return extension == ".thsprite";
}