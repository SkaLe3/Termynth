#pragma once
#include <functional>
#include <vector>
#include <algorithm>
#include <cstdint>

template<typename... Args>
class MulticastDelegate
{
public:
    using Func = std::function<void(Args...)>;

    struct Handle {
        uint64_t id = 0;
        bool operator==(const Handle& other) const { return id == other.id; }
        explicit operator bool() const { return id != 0; }
    };

public:
    Handle Add(Func func)
    {
        Handle handle = { ++m_LastId };
        m_Bindings.push_back({ handle, std::move(func) });
        return handle;
    }

    template<typename T>
    Handle Add(T* instance, void (T::*method)(Args...))
    {
        return Add([=](Args... args) {
            (instance->*method)(args...);
        });
    }

    void Remove(Handle handle)
    {
        m_Bindings.erase(
            std::remove_if(m_Bindings.begin(), m_Bindings.end(),
                [&](const Binding& b) { return b.handle == handle; }),
            m_Bindings.end()
        );
    }

    void Clear()
    {
        m_Bindings.clear();
    }

    void Broadcast(Args... args)
    {
        auto tempList = m_Bindings;
        for (auto& b : tempList)
            b.func(args...);
    }

private:
    struct Binding {
        Handle handle;
        Func func;
    };

    std::vector<Binding> m_Bindings;
    uint64_t m_LastId = 0;
};
