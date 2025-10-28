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

void PlatformUtilsWindows::InitInput()
{
    LOG_INFO("Set console output to unicode");
    SetConsoleOutputCP(65001);
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

int32 PlatformUtilsWindows::GetVirtualKeyCode(Key key)
{
    switch (key)
    {
        // Arrow keys
        case Key::Up:    return VK_UP;
        case Key::Down:  return VK_DOWN;
        case Key::Left:  return VK_LEFT;
        case Key::Right: return VK_RIGHT;

        // Function keys
        case Key::F1:  return VK_F1;
        case Key::F2:  return VK_F2;
        case Key::F3:  return VK_F3;
        case Key::F4:  return VK_F4;
        case Key::F5:  return VK_F5;
        case Key::F6:  return VK_F6;
        case Key::F7:  return VK_F7;
        case Key::F8:  return VK_F8;
        case Key::F9:  return VK_F9;
        case Key::F10: return VK_F10;
        case Key::F11: return VK_F11;
        case Key::F12: return VK_F12;

        // Special keys
        case Key::Insert:    return VK_INSERT;
        case Key::DeleteKey: return VK_DELETE;
        case Key::Home:      return VK_HOME;
        case Key::End:       return VK_END;
        case Key::PageUp:    return VK_PRIOR;
        case Key::PageDown:  return VK_NEXT;
        case Key::Escape:    return VK_ESCAPE;
        case Key::Enter:     return VK_RETURN;
        case Key::Space:     return VK_SPACE;
        case Key::Backspace: return VK_BACK;
        case Key::Tab:       return VK_TAB;

        // Letters
        case Key::A: return 'A';
        case Key::B: return 'B';
        case Key::C: return 'C';
        case Key::D: return 'D';
        case Key::E: return 'E';
        case Key::F: return 'F';
        case Key::G: return 'G';
        case Key::H: return 'H';
        case Key::I: return 'I';
        case Key::J: return 'J';
        case Key::K: return 'K';
        case Key::L: return 'L';
        case Key::M: return 'M';
        case Key::N: return 'N';
        case Key::O: return 'O';
        case Key::P: return 'P';
        case Key::Q: return 'Q';
        case Key::R: return 'R';
        case Key::S: return 'S';
        case Key::T: return 'T';
        case Key::U: return 'U';
        case Key::V: return 'V';
        case Key::W: return 'W';
        case Key::X: return 'X';
        case Key::Y: return 'Y';
        case Key::Z: return 'Z';

        // Numbers
        case Key::Num0: return '0';
        case Key::Num1: return '1';
        case Key::Num2: return '2';
        case Key::Num3: return '3';
        case Key::Num4: return '4';
        case Key::Num5: return '5';
        case Key::Num6: return '6';
        case Key::Num7: return '7';
        case Key::Num8: return '8';
        case Key::Num9: return '9';

        default: return 0; // Unknown key
    }
}