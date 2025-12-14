#pragma once
#include "Core/Core.h"

#include <memory>
#include <string>
#include <vector>

template<typename T>
class TAssetPtr
{
public:
    TAssetPtr() = default;
    TAssetPtr(std::shared_ptr<T> ptr) : m_Asset(ptr) {}

    T* Get() const {return m_Asset.get();}
    T* operator->() const {return m_Asset.get();}
    bool IsValid() const {return m_Asset != nullptr;}

private:
    std::shared_ptr<T> m_Asset;
};

class IAsset 
{
public:
    virtual ~IAsset() = default;
    virtual const std::string& GetPath() const = 0;
    virtual bool IsLoaded() const = 0;
    virtual size_t GetMemorySize() const = 0;
};

class IAssetLoader
{
public:
    virtual ~IAssetLoader() = default;
    virtual std::shared_ptr<IAsset> Load(const std::vector<uint8>& data, const std::string& path) = 0;
    virtual bool CanLoad(const std::string& extension) const = 0;
};