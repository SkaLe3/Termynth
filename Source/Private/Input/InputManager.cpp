#include "Input/InputManager.h"
#include "Platform/PlatformUtils.h"

#include <fstream>


InputManager &InputManager::Get()
{
    static InputManager s_Instance;
    return s_Instance;
}

void InputManager::PollInput()
{
    m_PreviousState = m_CurrentState;

    for (auto& keystate : m_CurrentState)
    {
        keystate.second = false;
    }

    while (PlatformUtils::Get().kbhitNonBlocking())
    {
        uint8 c = PlatformUtils::Get().getchNonBlocking();
        int32 ext = 0;

        PlatformUtils::Get().ReadExtendedKey(c, ext);

        Key key = TranslateCharToKey(c, ext);
        if (key != Key::Unknown)
        {
            m_CurrentState[key] = true;
        }
    }

    for (auto& [key, wasDown] : m_PreviousState)
    {
        if (wasDown && !m_CurrentState[key])
        {
            // Generate release event
        }
        if (!wasDown && m_CurrentState[key])
        {
            // Generate press event
        }
        if (wasDown && m_CurrentState[key])
        {
            // Generate held event
        }
    }
}
bool InputManager::IsKeyPressed(Key key) const
{
    return m_CurrentState.at(key);
}

Key InputManager::TranslateCharToKey(char c, int32 extended)
{
    if (extended != 0)
    {
        switch (extended)
        {
            case 72: return Key::Up;
            case 80: return Key::Down;
            case 75: return Key::Left;
            case 77: return Key::Right;

            case 59: return Key::F1;
            case 60: return Key::F2;
            case 61: return Key::F3;
            case 62: return Key::F4;
            case 63: return Key::F5;
            case 64: return Key::F6;
            case 65: return Key::F7;
            case 66: return Key::F8;
            case 67: return Key::F9;
            case 68: return Key::F10;

            case 133: return Key::F11;
            case 134: return Key::F12;

            case 82: return Key::Insert;
            case 83: return Key::DeleteKey;
            case 71: return Key::Home;
            case 79: return Key::End;
            case 73: return Key::PageUp;
            case 81: return Key::PageDown;

            default: return Key::Unknown;
        }
    }

    // Map normal ASCII keys
    switch (c)
    {
        case 27: return Key::Escape;
        case 13: return Key::Enter;
        case 32: return Key::Space;
        case 8:  return Key::Backspace;
        case 9:  return Key::Tab;

        case '0': return Key::Num0;
        case '1': return Key::Num1;
        case '2': return Key::Num2;
        case '3': return Key::Num3;
        case '4': return Key::Num4;
        case '5': return Key::Num5;
        case '6': return Key::Num6;
        case '7': return Key::Num7;
        case '8': return Key::Num8;
        case '9': return Key::Num9;

        case 'a': case 'A': return Key::A;
        case 'b': case 'B': return Key::B;
        case 'c': case 'C': return Key::C;
        case 'd': case 'D': return Key::D;
        case 'e': case 'E': return Key::E;
        case 'f': case 'F': return Key::F;
        case 'g': case 'G': return Key::G;
        case 'h': case 'H': return Key::H;
        case 'i': case 'I': return Key::I;
        case 'j': case 'J': return Key::J;
        case 'k': case 'K': return Key::K;
        case 'l': case 'L': return Key::L;
        case 'm': case 'M': return Key::M;
        case 'n': case 'N': return Key::N;
        case 'o': case 'O': return Key::O;
        case 'p': case 'P': return Key::P;
        case 'q': case 'Q': return Key::Q;
        case 'r': case 'R': return Key::R;
        case 's': case 'S': return Key::S;
        case 't': case 'T': return Key::T;
        case 'u': case 'U': return Key::U;
        case 'v': case 'V': return Key::V;
        case 'w': case 'W': return Key::W;
        case 'x': case 'X': return Key::X;
        case 'y': case 'Y': return Key::Y;
        case 'z': case 'Z': return Key::Z;

        default: return Key::Unknown;
    }
}

InputManager::InputManager()
{
    for (auto& keystate : m_CurrentState)
    {
        keystate.second = false;
    }
    m_PreviousState = m_CurrentState;
}