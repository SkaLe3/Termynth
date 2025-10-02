#include "Platform/Linux/LinuxPlatformUtils.h"
#include "Core/Core.h"
#include "Core/Logger.h"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

static PlatformUtilsLinux g_PlatformUtilsInstance;

termios s_OrigTermios;

void PlatformUtilsLinux::InitInput()
{
    tcgetattr(STDIN_FILENO, &origTermios);
    termios newTermios = origTermios;
    newTermios.c_lflag &= ~(ICANON | ECHO); // disable line buffering and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);

    // make stdin non-blocking
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
}
void PlatformUtilsLinux::RestoreInput()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &origTermios);
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) & ~O_NONBLOCK);
}
bool PlatformUtilsLinux::kbhitNonBlocking()
{
    int ch = getchar();
    if (ch != EOF) {
        ungetc(ch, stdin); // put it back for reading
        return true;
    }
    return false;
}
char PlatformUtilsLinux::getchNonBlocking()
{
    return getchar();
}

void PlatformUtilsLinux::ReadExtendedKey(uint8& c, int32& ext)
{
    ext = 0;
    if (c == 27)
    {
        int32 next1 = getchNonBlocking();
        if (next1 == '[')
        {
            int32 next2 = getchNonBlocking();
            ext = next2;
            c = 0;
        }
        else
        {
            ungetc(next1, stdin);
        }
    }
}

void PlatformUtilsLinux::Init()
{
    if (s_Instance != nullptr)
    {
        LOG_WARNING("PlatformUtils: Init() called. s_Instance is already initialized");
    }
    s_Instance = new PlatformUtilsLinux();
}