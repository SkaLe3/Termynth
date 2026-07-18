#pragma once
#include "NetArchive.h"

class IRepProp
{
    public:
        virtual ~IRepProp() = default;
        virtual bool ConsumeDirty() = 0;
        virtual void Serialize(FArchive& ar) = 0;
};

template<typename T>
class TRepProp : public IRepProp
{
public:
    TRepProp() = default;
    TRepProp(const T& initial) : m_Value(initial) {}

    TRepProp& operator=(const T& value)
    {
        if (!(m_Value == value))
        {
            m_Value = value;
            m_bDirty = true;
        }
        return *this;
    }

    operator const T&() const { return m_Value; }
    const T& Get() const { return m_Value; }

    bool ConsumeDirty() override
    {
        bool wasDirty = m_bDirty;
        m_bDirty = false;
        return wasDirty;
    }

    void Serialize(FArchive& ar) override
    {
        if (ar.IsSaving())
        {
            T copy = m_Value;
            ar << copy;
        }
        else
        {
            T incoming{};
            ar << incoming;
            m_Value = incoming; // bypass operator= so receiving doesn't re-dirty
        }
    }

private:
    T m_Value{};
    bool m_bDirty = true;
};
