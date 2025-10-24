#pragma once
#include "Engine/AssetTypes.h"


class AudioClipAsset : public IAsset
{
public:
    AudioClipAsset(const std::string& path) : m_Path(path), m_Loaded(false){}

    const std::string& GetPath() const override {return m_Path;}
    bool IsLoaded() const override {return m_Loaded;}
    size_t GetMemorySize() const override {return m_Data.size();}

    int32 GetSampleRate() const {return m_SampleRate;}
    int32 GetChannels() const {return m_Channels;}
    int32 GetBitsPerSample() const {return m_BitsPerSample;}
    const uint8* GetData() const {return m_Data.data();}
    size_t GetDataSize() const {return m_Data.size();}

private:
    friend class AudioLoader;
    std::string m_Path;
    std::vector<uint8> m_Data;
    int32 m_SampleRate = 0;
    int32 m_Channels = 0;
    int32 m_BitsPerSample = 0;
    bool m_Loaded = false;
};
