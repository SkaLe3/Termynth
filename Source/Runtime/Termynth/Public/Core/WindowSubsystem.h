#pragma once
#include "Core/Core.h"
#include <memory>

class IPlatformWindow;

class WindowSubsystem
{
public:
    static WindowSubsystem& Get();

    void Init(int32 width, int32 height);

    IPlatformWindow* GetWindow() const {return m_ActiveWindow.get();}

private:
    std::unique_ptr<IPlatformWindow> m_ActiveWindow;

private:
    WindowSubsystem() = default;
    ~WindowSubsystem() = default;
    WindowSubsystem(const WindowSubsystem&) = delete;
    WindowSubsystem(WindowSubsystem&&) = delete;

};