#include "Platform/Linux/LinuxPlatformUtils.h"
#include "Core/Core.h"
#include "Core/Logger.h"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>

static int g_Keyboard_fd = -1;

static PlatformUtilsLinux g_PlatformUtilsInstance;

termios g_OrigTermios;

void PlatformUtilsLinux::InitInput()
{
    tcgetattr(STDIN_FILENO, &g_OrigTermios);
    termios newTermios = g_OrigTermios;
    newTermios.c_lflag &= ~(ICANON | ECHO); // disable line buffering and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);

    // make stdin non-blocking
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);

    std::string devicePath = FindKeyboardDevicePath();
    if (devicePath.empty())
    {
        LOG_ERROR("Could not locate a suitable keyboard event device.");
        return;
    }

    LOG_INFO(std::format("Keyboard found at path: {}", devicePath));

    g_Keyboard_fd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK);
    if (g_Keyboard_fd < 0)
    {
        LOG_ERROR("Could not open input device. Check permissions (Group 'input' required).");
    }
    else
    {
        LOG_INFO(std::format("Devide opened with FD: {}", g_Keyboard_fd));
    }
}
void PlatformUtilsLinux::RestoreInput()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &g_OrigTermios);
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) & ~O_NONBLOCK);

    if (g_Keyboard_fd >= 0)
    {
        close(g_Keyboard_fd);
    }
}
int32 PlatformUtilsLinux::kbhitNonBlocking()
{
    int ch = getchar();
    if (ch != EOF) {
        ungetc(ch, stdin); // put it back for reading
        return 1;
    }
    return 0;
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

bool PlatformUtilsLinux::GetAsyncKeyState(uint8 c)
{
    if (g_Keyboard_fd < 0) return false;
    
    unsigned char key_state_map[KEY_MAX / 8 + 1] = {0};
    if (ioctl(g_Keyboard_fd, EVIOCGKEY(sizeof(key_state_map)), key_state_map) < 0 )
    {
        LOG_ERROR("ioctl failed");
        return false;
    }

    return (key_state_map[c / 8] & (1 << (c  % 8))) != 0;
}

int32 PlatformUtilsLinux::GetVirtualKeyCode(Key key)
{
    switch (key)
    {
        // Arrow keys
        case Key::Up:    return KEY_UP;
        case Key::Down:  return KEY_DOWN;
        case Key::Left:  return KEY_LEFT;
        case Key::Right: return KEY_RIGHT;

        // Function keys
        case Key::F1:  return KEY_F1;
        case Key::F2:  return KEY_F2;
        case Key::F3:  return KEY_F3;
        case Key::F4:  return KEY_F4;
        case Key::F5:  return KEY_F5;
        case Key::F6:  return KEY_F6;
        case Key::F7:  return KEY_F7;
        case Key::F8:  return KEY_F8;
        case Key::F9:  return KEY_F9;
        case Key::F10: return KEY_F10;
        case Key::F11: return KEY_F11;
        case Key::F12: return KEY_F12;

        // Special keys
        case Key::Insert:    return KEY_INSERT;
        case Key::DeleteKey: return KEY_DELETE;
        case Key::Home:      return KEY_HOME;
        case Key::End:       return KEY_END;
        case Key::PageUp:    return KEY_PAGEUP;
        case Key::PageDown:  return KEY_PAGEDOWN;
        case Key::Escape:    return KEY_ESC; // Note: KEY_ESC for Escape
        case Key::Enter:     return KEY_ENTER;
        case Key::Space:     return KEY_SPACE;
        case Key::Backspace: return KEY_BACKSPACE;
        case Key::Tab:       return KEY_TAB;

        // Letters
        // For letters and numbers, you must use the KEY_* macros, NOT the ASCII character.
        // The Linux kernel treats these as physical key locations.
        case Key::A: return KEY_A;
        case Key::B: return KEY_B;
        case Key::C: return KEY_C;
        case Key::D: return KEY_D;
        case Key::E: return KEY_E;
        case Key::F: return KEY_F;
        case Key::G: return KEY_G;
        case Key::H: return KEY_H;
        case Key::I: return KEY_I;
        case Key::J: return KEY_J;
        case Key::K: return KEY_K;
        case Key::L: return KEY_L;
        case Key::M: return KEY_M;
        case Key::N: return KEY_N;
        case Key::O: return KEY_O;
        case Key::P: return KEY_P;
        case Key::Q: return KEY_Q;
        case Key::R: return KEY_R;
        case Key::S: return KEY_S;
        case Key::T: return KEY_T;
        case Key::U: return KEY_U;
        case Key::V: return KEY_V;
        case Key::W: return KEY_W;
        case Key::X: return KEY_X;
        case Key::Y: return KEY_Y;
        case Key::Z: return KEY_Z;

        // Numbers (Main Row)
        case Key::Num0: return KEY_0;
        case Key::Num1: return KEY_1;
        case Key::Num2: return KEY_2;
        case Key::Num3: return KEY_3;
        case Key::Num4: return KEY_4;
        case Key::Num5: return KEY_5;
        case Key::Num6: return KEY_6;
        case Key::Num7: return KEY_7;
        case Key::Num8: return KEY_8;
        case Key::Num9: return KEY_9;

        default: return 0; // Unknown key
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

std::string PlatformUtilsLinux::FindKeyboardDevicePath()
{
    std::ifstream inputDevices("/proc/bus/input/devices");
    if (!inputDevices.is_open())
    {
        LOG_ERROR("Could not open /proc/bus/input/devices for reading.");
        return "";
    }
    std::string line;
    std::string eventPath = "";
    bool bHasKbdHandler = false;
    bool bHasKeyboardName = false;

    while (std::getline(inputDevices, line))
    {   
        if (line.rfind("N: Name=", 0) == 0)
        {
            if (line.find("keyboard") != std::string::npos ||
                line.find("Keypad") != std::string::npos)
                {
                    bHasKeyboardName = true;
                }
        }


        if (line.rfind("H: Handlers=", 0) == 0)
        {
            if (line.find("kbd") != std::string::npos && line.find("event") != std::string::npos)
            {
                bHasKbdHandler = true;

                std::stringstream ss(line);
                std::string segment;
                std::string token = "event";
                while (ss >> segment)
                {
                    if (segment.rfind(token, 0) == 0)
                    {
                        eventPath = "/dev/input/" + segment;
                        break;
                    }
                }
            }
        }
        if (line.empty())
        {
            if (bHasKbdHandler && bHasKeyboardName && !eventPath.empty())
            {
                return eventPath;
            }
            eventPath = "";
            bHasKbdHandler = false;
            bHasKeyboardName = false;
        }
    }
    if (bHasKbdHandler && bHasKeyboardName && !eventPath.empty())
    {
        return eventPath;
    }

    return "";

}
