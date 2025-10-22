#include "Platform/Windows/WindowsPlatformUtils.h"
#include "Core/Logger.h"
#include <windows.h>
#include <conio.h>

void PlatformUtilsWindows::ClearScreen()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD topLeft = {0, 0};
    DWORD written;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD cells;

    GetConsoleScreenBufferInfo(hOut, &csbi);
    cells = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputCharacter(hOut, ' ', cells, topLeft, &written);
    FillConsoleOutputAttribute(hOut, csbi.wAttributes, cells, topLeft, &written);
    SetConsoleCursorPosition(hOut, topLeft);
}

void PlatformUtilsWindows::MoveCursorTopLeft()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD topLeft = {0, 0};
    SetConsoleCursorPosition(hOut, topLeft);
}
void PlatformUtilsWindows::HideCursor()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

void PlatformUtilsWindows::ShowCursor()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

void PlatformUtilsWindows::Init()
{
    if (s_Instance != nullptr)
    {
        LOG_WARNING("PlatformUtils: Init() called. s_Instance is already initialized");
    }
    s_Instance = new PlatformUtilsWindows();
}
int PlatformUtilsWindows::kbhitNonBlocking()
{
    return _kbhit();
}
char PlatformUtilsWindows::getchNonBlocking()
{
    return static_cast<char>(_getch());
}

void PlatformUtilsWindows::ReadExtendedKey(uint8 &c, int32 &ext)
{
    if (c == 0 || c == 224)
    {
        ext = getchNonBlocking();
    }
    else
    {
        ext = 0;
    }
}

bool PlatformUtilsWindows::GetAsyncKeyState(uint8 c)
{
    return (::GetAsyncKeyState(c) & 0x8000) != 0;
}