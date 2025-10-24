#include "Engine/AudioLoader.h"

#include "Engine/AudioClipAsset.h"

std::shared_ptr<IAsset> AudioLoader::Load(const std::vector<uint8>& data, const std::string& path)
{
    auto audio = std::make_shared<AudioClipAsset>(path);

    // TODO: Implement audio loading with dr_wav or stb_vorbis

    audio->m_SampleRate = 44100;
    audio->m_Channels = 2;
    audio->m_BitsPerSample = 16;
    audio->m_Data = data;
    audio->m_Loaded = true;

    return audio;
}
bool AudioLoader::CanLoad(const std::string& extension) const
{
    return extension == ".wav" || extension == ".ogg" || extension == ".mp3";
}