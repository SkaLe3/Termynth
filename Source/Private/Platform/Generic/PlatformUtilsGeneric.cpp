#include "Platform/Generic/PlatformUtilsGeneric.h"
#include "Core/Logger.h"
#include <iostream>

PlatformUtilsGeneric* PlatformUtilsGeneric::s_Instance = nullptr;

PlatformUtilsGeneric::PlatformUtilsGeneric()
{

}

PlatformUtilsGeneric::~PlatformUtilsGeneric()
{
    delete s_Instance;
}
PlatformUtilsGeneric& PlatformUtilsGeneric::Get()
{
    if (s_Instance == nullptr)
    {
        LOG_FATAL("PlatformUtils::Get(): s_Instance is nullptr. Initialize() was never called");
    }
    return *s_Instance;
}

void PlatformUtilsGeneric::Init()
{
    if (s_Instance != nullptr)
    {
        LOG_WARNING("PlatformUtils: Init() called. s_Instance is already initialized");
    }
    s_Instance = new PlatformUtilsGeneric();
}

void PlatformUtilsGeneric::ClearScreen()
{
    std::cout << "\033[2J\033[1;1H";
}

void PlatformUtilsGeneric::ResetTerminal()
{
    std::cout << "\033c";
}

void PlatformUtilsGeneric::MoveCursor(int32_t x, int32_t y)
{
    std::cout << "\033[" << y << ";" << x << "H";
}

void PlatformUtilsGeneric::MoveCursorTopLeft()
{
    std::cout << "\033[H";
}

void PlatformUtilsGeneric::HideCursor()
{
    std::cout << "\033[?25l";
}
void PlatformUtilsGeneric::ShowCursor()
{
    std::cout << "\033[?25h";
}


fs::path PlatformUtilsGeneric::EngineDir()
{
    return "";
}
fs::path PlatformUtilsGeneric::ProjectDir()
{
    return "";
}