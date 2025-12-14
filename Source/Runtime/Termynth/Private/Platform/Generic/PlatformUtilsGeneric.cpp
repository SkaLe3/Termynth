#include "Platform/Generic/PlatformUtilsGeneric.h"
#include "Core/Logger.h"
#include "Core/Core.h"
#include <iostream>

PlatformUtilsGeneric* PlatformUtilsGeneric::s_Instance = nullptr;

PlatformUtilsGeneric::PlatformUtilsGeneric()
{
	std::ios::sync_with_stdio(false); // optional, decouple from C stdio
    std::cout << std::nounitbuf; // disables automatic flush after each output
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

std::string PlatformUtilsGeneric::MakeUtf8(const uint8* value)
{
    char16_t c = static_cast<char16_t>(value[0] | (value[1] << 8));

    std::string out;

    if (c <= 0x7F) {
        // 1-byte ASCII
        out += static_cast<char>(c);
    } 
    else if (c <= 0x7FF) {
        // 2-byte UTF-8
        out += static_cast<char>(0xC0 | ((c >> 6) & 0x1F));
        out += static_cast<char>(0x80 | (c & 0x3F));
    } 
    else {
        // 3-byte UTF-8
        out += static_cast<char>(0xE0 | ((c >> 12) & 0x0F));
        out += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (c & 0x3F));
    }

    return out;
}

std::string PlatformUtilsGeneric::Utf8FromChar16(char16_t c)
{
    std::string out;

    if (c <= 0x7F) {
        // 1-byte ASCII
        out += static_cast<char>(c);
    } else if (c <= 0x7FF) {
        // 2-byte UTF-8
        out += static_cast<char>(0xC0 | ((c >> 6) & 0x1F));
        out += static_cast<char>(0x80 | (c & 0x3F));
    } else {
        // 3-byte UTF-8
        out += static_cast<char>(0xE0 | ((c >> 12) & 0x0F));
        out += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (c & 0x3F));
    }

    return out;
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
    return TERMYNTH_ENGINE_DIR;
}
fs::path PlatformUtilsGeneric::ProjectDir()
{
    return TERMYNTH_PROJECT_DIR;
}