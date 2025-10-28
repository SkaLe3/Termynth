#include "Platform/Windows/WindowsPlatformWindow.h"
#include "Core/Globals.h"
#include "Core/Logger.h"
#include "Platform/PlatformUtils.h"

#include <string>
#include <vector>

static inline WORD CellColorToWindowsAttribute(const Cell& cell)
{
    uint8 fgIndex = Color::RGBToAnsiCode(cell.FgR, cell.FgG, cell.FgB);
    uint8 bgIndex = Color::RGBToAnsiCode(cell.BgR, cell.BgG, cell.BgB);

    WORD fgAttr = 0;
    WORD bgAttr = 0;

    if (fgIndex & 0x1) fgAttr |= FOREGROUND_BLUE;
    if (fgIndex & 0x2) fgAttr |= FOREGROUND_GREEN;
    if (fgIndex & 0x4) fgAttr |= FOREGROUND_RED;
    if (fgIndex & 0x8) fgAttr |= FOREGROUND_INTENSITY;

    if (bgIndex & 0x1) bgAttr |= BACKGROUND_BLUE;
    if (bgIndex & 0x2) bgAttr |= BACKGROUND_GREEN;
    if (bgIndex & 0x4) bgAttr |= BACKGROUND_RED;
    if (bgIndex & 0x8) bgAttr |= BACKGROUND_INTENSITY;

    return fgAttr | bgAttr;
}



WindowsPlatformWindow::WindowsPlatformWindow(int32 width, int32 height)
    : PlatformWindowGeneric(width, height)
{
    InitConsoleBuffer();
}

WindowsPlatformWindow::~WindowsPlatformWindow()
{
    SetConsoleActiveScreenBuffer(m_OriginalBuffer);
    CloseHandle(m_OffscreenBuffer);
}

void WindowsPlatformWindow::SwapBuffers()
{
    if (GlobalParameters::g_bUseWindowsApiOutput == false)
    {
        PlatformWindowGeneric::SwapBuffers();
        return;
    }
    if (GlobalParameters::g_ColorMode == EColorMode::TrueColor)
    {
        static bool bLogged = false;
        if (!bLogged)
        {
            LOG_WARNING("g_ColorMode set to True color. True color is not compatible with WindowsAPi (g_bUseWindowsApiOutput is true)");
            bLogged = true;
        }
    }

    std::vector<CHAR_INFO> charBuffer(m_Framebuffer->Width * m_Framebuffer->Height);
    for (int32 y = 0; y < m_Framebuffer->Height; ++y)
    {
        for (int32 x = 0; x < m_Framebuffer->Width; ++x)
        {
            int32 idx = y * m_Framebuffer->Width + x;
            const Cell& cell = m_Framebuffer->CurrentBuffer->Handle[idx];

            CHAR_INFO& ci = charBuffer[idx];
            //ci.Char.AsciiChar = cell.Char; 
            WCHAR my_wchar = *(WCHAR*)cell.Char;
            ci.Char.UnicodeChar = my_wchar;
            ci.Attributes = CellColorToWindowsAttribute(cell);
        }
    }

    SMALL_RECT writeRegion;
    writeRegion.Left = 0;
    writeRegion.Top = 0;
    writeRegion.Right = m_Framebuffer->Width - 1;
    writeRegion.Bottom = m_Framebuffer->Height - 1;

    COORD bufferSize = {(SHORT)m_Framebuffer->Width, (SHORT)m_Framebuffer->Height};
    COORD bufferCoord = {0, 0};

    WriteConsoleOutputA(
        m_OffscreenBuffer,
        charBuffer.data(),
        bufferSize,
        bufferCoord,
        &writeRegion
    );
    SetConsoleActiveScreenBuffer(m_OffscreenBuffer);
}

void WindowsPlatformWindow::InitConsoleBuffer()
{
    m_OriginalBuffer = GetStdHandle(STD_OUTPUT_HANDLE);

    m_OffscreenBuffer = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL);

    COORD size = {(SHORT)m_Framebuffer->Width, (SHORT)m_Framebuffer->Height};
    SetConsoleScreenBufferSize(m_OffscreenBuffer, size);

    SMALL_RECT window = {0, 0, (SHORT)(m_Framebuffer->Width - 1), (SHORT)(m_Framebuffer->Height - 1)};
    SetConsoleWindowInfo(m_OffscreenBuffer, TRUE, &window);
}