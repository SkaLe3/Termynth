#include "Core/Globals.h"

EColorMode GlobalParameters::g_ColorMode = EColorMode::Ansi16;
bool GlobalParameters::g_bUseWindowsApiOutput = true;

uint32 GlobalParameters::g_ViewportWidth = 80;
uint32 GlobalParameters::g_ViewportHeight = 25;

bool g_ShouldRequestExit = false;
bool g_IsRequestingExit = false;

void BeginExitIfRequested()
{
    if (g_ShouldRequestExit)
    {
        g_IsRequestingExit = true;
    }
}

void RequestEngineExit()
{
    g_ShouldRequestExit = true;
}