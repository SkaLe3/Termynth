#pragma once
#include "Core/Core.h"

enum class EColorMode
{
    Ansi16,
    TrueColor
};


namespace GlobalParameters
{
    /* 
    * Allows 24-bit color with ANSI escape sequences
    * This setting is available with WindowsApiOutput off
    * Attention: Changing this value requires full window redraw to take effect (use IPlatformWindow::ForceRedraw() )
    */
    extern EColorMode g_ColorMode;

    /*
    * Allows WindowsApi use for output to achieve smoother update
    * TThis setting does NOT support 24-bit colors and text styles
    */
    extern bool g_bUseWindowsApiOutput;

    /*
    * Viewport size is set at compilation time but may be overriden with command line arguments (@TODO: Add command line arguments override)
    * Runtime resizing is not yet allowed
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

