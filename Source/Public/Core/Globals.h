#pragma once
#include "Core/Core.h"

enum class EColorMode
{
    Ansi16,
    TrueColor
};


namespace GlobalParameters
{
    /**
    * @brief Global color mode setting.
    *
    * Enables 24-bit color using ANSI escape sequences.
    * This setting is only available when WindowsApiOutput is off.
    *
    * @note Changing this value requires a full window redraw to take effect.
    *       Call IPlatformWindow::ForceRedraw() after modifying this variable.
    */
    extern EColorMode g_ColorMode;

    /**
    * @brief Global flag to enable Windows API output.
    *
    * When enabled, output uses the Windows API for smoother updates.
    *
    * @note This setting does NOT support 24-bit colors or text styles.
    */
    extern bool g_bUseWindowsApiOutput;

    /**
    * @brief Global viewport size settings.
    *
    * The viewport width and height are set at compile time but may later
    * be overridden with command line arguments. (@TODO: Implement command line override)
    *
    * @note Runtime resizing of the viewport is not yet supported.
    */
    extern uint32 g_ViewportWidth;
    extern uint32 g_ViewportHeight;

} // namespace GlobalParameters

extern bool g_IsRequestingExit;

extern void BeginExitIfRequested();

inline bool IsExitRequested()
{
    return g_IsRequestingExit;
}

void RequestEngineExit();

