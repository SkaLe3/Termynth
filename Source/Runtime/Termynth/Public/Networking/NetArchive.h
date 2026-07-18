#pragma once
#include "Core/Core.h"
#include "Core/Logger.h"
#include <vector>
#include <string>
#include <cstring>
#include <type_traits>

// Minimal binary rea/write archive used for both replicated-property serialization and RPC parameter serialization.
class FArchive
{
public:
    explicit FArchive(bool bSaving) : m_bSaving(bSaving) {}

    bool IsSaving() const { return m_bSaving; }
    bool IsLoading() const {return !m_bSaving; }

    template<typename T>
    typename std::enable_if<std::is_trivially_copyable<T>::value, FArchive&>::type
    operator<<(T& Value)
    {
        if (m_bSaving)
        {
            const Byte* p = reinterpret_cast<const Byte*>(&Value);
            m_Buffer.insert(m_Buffer.end(), p, p + sizeof(T));
        }
        else
        {
            ASSERT(m_ReadOffset + sizeof(T) <= m_Buffer.size());
            std::memcpy(&Value, m_Buffer.data() + m_ReadOffset, sizeof(T));
            m_ReadOffset += sizeof(T);
        }
        return *this;
    }

    FArchive& operator<<(std::string& Value)
    {
        uint16 len = m_bSaving ? (uint16)Value.size() : 0;
        *this << len;
        if (m_bSaving)
        {
            m_Buffer.insert(m_Buffer.end(), Value.begin(), Value.end());
        }
        else
        {
            ASSERT(m_ReadOffset + len <= m_Buffer.size());
            Value.assign((const char*)m_Buffer.data() + m_ReadOffset, len);
            m_ReadOffset += len;
        }
        return *this;
    }

    const std::vector<Byte>& GetBuffer() const { return m_Buffer; }
    void SetBuffer(std::vector<Byte> data) { m_Buffer = std::move(data); m_ReadOffset = 0; }

private:
    bool m_bSaving;
    std::vector<Byte> m_Buffer;
    size_t m_ReadOffset = 0;
};