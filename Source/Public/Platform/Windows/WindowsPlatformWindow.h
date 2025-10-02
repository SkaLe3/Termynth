#pragma once

#include "Core/Core.h"
#include "Platform/Generic/PlatformWindowGeneric.h"

#include <windows.h>

class WindowsPlatformWindow : public PlatformWindowGeneric
{
public:
    WindowsPlatformWindow(int32 width, int32 height);
    ~WindowsPlatformWindow();

    virtual void SwapBuffers() override;

private:
    void InitConsoleBuffer();

private:
    HANDLE m_OriginalBuffer;
    HANDLE m_OffscreenBuffer;
};