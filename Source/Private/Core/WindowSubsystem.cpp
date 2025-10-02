#include "Core/WindowSubsystem.h"
#include "Platform/Generic/PlatformWindowGeneric.h"

#if defined(PLATFORM_WINDOWS)
#include "Platform/Windows/WindowsPlatformWindow.h"
#elif defined(PLATFORM_LINUX)
#include "Platform/Linux/LinuxPlatformWindow.h"
#include "WindowSubsystem.h"
#endif

void WindowSubsystem::Init(int32 width, int32 height)
{
#if defined(PLATFORM_WINDOWS)
    m_ActiveWindow = std::make_unique<WindowsPlatformWindow>(width, height);
#elif defined(PLATFORM_LINUX)
    m_ActiveWindow = std::make_unique<LinuxPlatformWindow>(width, height);
#else
    m_ActiveWindow = std::make_unique<GenericPlatformWindow>(width, height);
#endif
}

WindowSubsystem &WindowSubsystem::Get()
{
    static WindowSubsystem s_Instance; 
    return s_Instance;
}