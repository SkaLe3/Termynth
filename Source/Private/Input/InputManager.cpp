#include "Input/InputManager.h"
#include "Platform/PlatformUtils.h"
#include "Core/Logger.h"

#include <algorithm>


InputManager &InputManager::Get()
{
    static InputManager s_Instance;
    return s_Instance;
}

InputManager::InputManager()
{
    m_KeyStates.clear();

    ConfigureAxis("Horizontal", Key::Right, Key::Left);
    ConfigureAxis("Vertical", Key::Up, Key::Down);
    ConfigureAxis("HorizontalWASD", Key::D, Key::A);
    ConfigureAxis("VerticalWASD", Key::W, Key::S);

}


void InputManager::PollInput()
{
    // Update key states: Pressed -> Held, Released -> None
    for (auto& [key, state] : m_KeyStates)
    {
        if (state == KeyState::Pressed)
            state = KeyState::Held;
        else if (state == KeyState::Released)
            state = KeyState::None;
    }

    std::unordered_map<Key, bool> currentFrameKeys;

    while (PlatformUtils::Get().kbhitNonBlocking())
    {
        uint8_t c = PlatformUtils::Get().getchNonBlocking();
        int32_t ext = 0;

        PlatformUtils::Get().ReadExtendedKey(c, ext);

        Key key = TranslateCharToKey(c, ext);

        if (key != Key::Unknown)
        {
            currentFrameKeys[key] = true;
        }
    }

    // Now held
    for (const auto& [key, pressed] : currentFrameKeys)
    {
        KeyState currentState = m_KeyStates[key];
        if (currentState == KeyState::None || currentState == KeyState::Released)
        {
            m_KeyStates[key] = KeyState::Pressed;
            TriggerKeyPressedCallbacks(key);
        }
        else
        {
            m_KeyStates[key] = KeyState::Held;
        }
    }

    // Now not held
    for (auto& [key, state] : m_KeyStates)
    {
        if ((state == KeyState::Held || state == KeyState::Pressed) &&
            currentFrameKeys.find(key) == currentFrameKeys.end())
        {
            if (!PlatformUtils::Get().GetAsyncKeyState(TranslateKeyToVirtualKey(key)))
            {
                m_KeyStates[key] = KeyState::Released;
                TriggerKeyReleasedCallbacks(key);
            }
        }
    }

    // Update axis values with smoothing
    for (auto& [name, axis] : m_Axes)
    {
        float targetValue = 0.0f;
        if (GetKey(axis.Positive))
            targetValue += 1.0f;
        if (GetKey(axis.Negative))
            targetValue -= 1.0f;

        // Sooth interpolation
        if (axis.Smoothing > 0.0f)
        {
            float diff = targetValue - axis.CurrentValue;
            axis.CurrentValue += diff * axis.Smoothing;

            if (std::abs(diff) < 0.01f)
                axis.CurrentValue = targetValue;
        }
        else
        {
            axis.CurrentValue = targetValue;
        }
    }
}

bool InputManager::GetKey(Key key) const
{
    auto it = m_KeyStates.find(key);
    if (it == m_KeyStates.end())
        return false;
    
    return it->second == KeyState::Pressed || it->second == KeyState::Held;
}

bool InputManager::GetKeyPressed(Key key) const
{
    auto it = m_KeyStates.find(key);
    if (it == m_KeyStates.end())
        return false;
    
    return it->second == KeyState::Pressed;
}

bool InputManager::GetKeyReleased(Key key) const
{
    auto it = m_KeyStates.find(key);
    if (it == m_KeyStates.end())
        return false;
    
    return it->second == KeyState::Released;
}

bool InputManager::IsAnyKeyPressed() const
{
    for (const auto& [key, state] : m_KeyStates)
    {
        if (state == KeyState::Pressed || state == KeyState::Held)
            return true;
    }
    return false;
}

void InputManager::GetPressedKeys(std::vector<Key>& outKeys) const
{
    outKeys.clear();
    for (const auto& [key, state] : m_KeyStates)
    {
        if (state == KeyState::Pressed || state == KeyState::Held)
            outKeys.push_back(key);
    }
}

KeyState InputManager::GetKeyState(Key key) const
{
    auto it = m_KeyStates.find(key);
    if (it == m_KeyStates.end())
        return KeyState::None;
    return it->second;
}

void InputManager::RegisterKeyPressedCalllback(Key key, KeyCallback callback)
{
    m_KeyPressedCallbacks[key].push_back(callback);
}

void InputManager::RegisterKeyReleasedCallback(Key key, KeyCallback callback)
{
    m_KeyReleasedCallbacks[key].push_back(callback);
}

void InputManager::UnregisterKeyPressedCallback(Key key)
{
    m_KeyPressedCallbacks.erase(key);
}

void InputManager::UnregisterKeyReleasedCallback(Key key)
{
    m_KeyReleasedCallbacks.erase(key);
}

void InputManager::TriggerKeyPressedCallbacks(Key key)
{
    auto it = m_KeyPressedCallbacks.find(key);
    if (it != m_KeyPressedCallbacks.end())
    {
        for (auto& callback : it->second)
        {
            callback(key);
        }
    }
}

void InputManager::TriggerKeyReleasedCallbacks(Key key)
{
    auto it = m_KeyReleasedCallbacks.find(key);
    if (it != m_KeyReleasedCallbacks.end())
    {
        for (auto& callback : it->second)
        {
            callback(key);
        }
    }
}

float InputManager::GetAxis(const char* axisName) const
{
    auto it = m_Axes.find(axisName);
    if (it == m_Axes.end())
        return 0.0f;
    return it->second.CurrentValue;
}

float InputManager::GetAxisRaw(const char* axisName) const
{
    auto it = m_Axes.find(axisName);
    if (it == m_Axes.end())
        return 0.0f;
    
    float value = 0.0f;
    if (GetKey(it->second.Positive))
        value += 1.0f;
    if (GetKey(it->second.Negative))
        value -= 1.0f;
    
    return value;
}

void InputManager::ConfigureAxis(const char* axisName, Key positive, Key negative)
{
    AxisConfig config;
    config.Positive = positive;
    config.Negative = negative;
    config.Smoothing = 0.1f;
    config.CurrentValue = 0.0f;
    
    m_Axes[axisName] = config;
}

void InputManager::ClearAllInput()
{
    m_KeyStates.clear();
    for (auto& [name, axis] : m_Axes)
    {
        axis.CurrentValue = 0.0f;
    }
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

int32 InputManager::TranslateKeyToVirtualKey(Key key)
{
    switch (key)
    {
        // Arrow keys
        case Key::Up:    return PlatformUtils::Get().GetVirtualKeyCode(Key::Up);
        case Key::Down:  return PlatformUtils::Get().GetVirtualKeyCode(Key::Down);
        case Key::Left:  return PlatformUtils::Get().GetVirtualKeyCode(Key::Left);
        case Key::Right: return PlatformUtils::Get().GetVirtualKeyCode(Key::Right);

        // Function keys
        case Key::F1:  return PlatformUtils::Get().GetVirtualKeyCode(Key::F1);
        case Key::F2:  return PlatformUtils::Get().GetVirtualKeyCode(Key::F2);
        case Key::F3:  return PlatformUtils::Get().GetVirtualKeyCode(Key::F3);
        case Key::F4:  return PlatformUtils::Get().GetVirtualKeyCode(Key::F4);
        case Key::F5:  return PlatformUtils::Get().GetVirtualKeyCode(Key::F5);
        case Key::F6:  return PlatformUtils::Get().GetVirtualKeyCode(Key::F6);
        case Key::F7:  return PlatformUtils::Get().GetVirtualKeyCode(Key::F7);
        case Key::F8:  return PlatformUtils::Get().GetVirtualKeyCode(Key::F8);
        case Key::F9:  return PlatformUtils::Get().GetVirtualKeyCode(Key::F9);
        case Key::F10: return PlatformUtils::Get().GetVirtualKeyCode(Key::F10);
        case Key::F11: return PlatformUtils::Get().GetVirtualKeyCode(Key::F11);
        case Key::F12: return PlatformUtils::Get().GetVirtualKeyCode(Key::F12);

        // Special keys
        case Key::Insert:    return PlatformUtils::Get().GetVirtualKeyCode(Key::Insert);
        case Key::DeleteKey: return PlatformUtils::Get().GetVirtualKeyCode(Key::DeleteKey);
        case Key::Home:      return PlatformUtils::Get().GetVirtualKeyCode(Key::Home);
        case Key::End:       return PlatformUtils::Get().GetVirtualKeyCode(Key::End);
        case Key::PageUp:    return PlatformUtils::Get().GetVirtualKeyCode(Key::PageUp);
        case Key::PageDown:  return PlatformUtils::Get().GetVirtualKeyCode(Key::PageDown);
        case Key::Escape:    return PlatformUtils::Get().GetVirtualKeyCode(Key::Escape);
        case Key::Enter:     return PlatformUtils::Get().GetVirtualKeyCode(Key::Enter);
        case Key::Space:     return PlatformUtils::Get().GetVirtualKeyCode(Key::Space);
        case Key::Backspace: return PlatformUtils::Get().GetVirtualKeyCode(Key::Backspace);
        case Key::Tab:       return PlatformUtils::Get().GetVirtualKeyCode(Key::Tab);

        // Letters
        case Key::A: return PlatformUtils::Get().GetVirtualKeyCode(Key::A);
        case Key::B: return PlatformUtils::Get().GetVirtualKeyCode(Key::B);
        case Key::C: return PlatformUtils::Get().GetVirtualKeyCode(Key::C);
        case Key::D: return PlatformUtils::Get().GetVirtualKeyCode(Key::D);
        case Key::E: return PlatformUtils::Get().GetVirtualKeyCode(Key::E);
        case Key::F: return PlatformUtils::Get().GetVirtualKeyCode(Key::F);
        case Key::G: return PlatformUtils::Get().GetVirtualKeyCode(Key::G);
        case Key::H: return PlatformUtils::Get().GetVirtualKeyCode(Key::H);
        case Key::I: return PlatformUtils::Get().GetVirtualKeyCode(Key::I);
        case Key::J: return PlatformUtils::Get().GetVirtualKeyCode(Key::J);
        case Key::K: return PlatformUtils::Get().GetVirtualKeyCode(Key::K);
        case Key::L: return PlatformUtils::Get().GetVirtualKeyCode(Key::L);
        case Key::M: return PlatformUtils::Get().GetVirtualKeyCode(Key::M);
        case Key::N: return PlatformUtils::Get().GetVirtualKeyCode(Key::N);
        case Key::O: return PlatformUtils::Get().GetVirtualKeyCode(Key::O);
        case Key::P: return PlatformUtils::Get().GetVirtualKeyCode(Key::P);
        case Key::Q: return PlatformUtils::Get().GetVirtualKeyCode(Key::Q);
        case Key::R: return PlatformUtils::Get().GetVirtualKeyCode(Key::R);
        case Key::S: return PlatformUtils::Get().GetVirtualKeyCode(Key::S);
        case Key::T: return PlatformUtils::Get().GetVirtualKeyCode(Key::T);
        case Key::U: return PlatformUtils::Get().GetVirtualKeyCode(Key::U);
        case Key::V: return PlatformUtils::Get().GetVirtualKeyCode(Key::V);
        case Key::W: return PlatformUtils::Get().GetVirtualKeyCode(Key::W);
        case Key::X: return PlatformUtils::Get().GetVirtualKeyCode(Key::X);
        case Key::Y: return PlatformUtils::Get().GetVirtualKeyCode(Key::Y);
        case Key::Z: return PlatformUtils::Get().GetVirtualKeyCode(Key::Z);

        // Numbers
        case Key::Num0: return PlatformUtils::Get().GetVirtualKeyCode(Key::Num1);
        case Key::Num1: return PlatformUtils::Get().GetVirtualKeyCode(Key::Num2);
        case Key::Num2: return PlatformUtils::Get().GetVirtualKeyCode(Key::Num3);
        case Key::Num3: return PlatformUtils::Get().GetVirtualKeyCode(Key::Num4);
        case Key::Num4: return PlatformUtils::Get().GetVirtualKeyCode(Key::Num5);
        case Key::Num5: return PlatformUtils::Get().GetVirtualKeyCode(Key::Num6);
        case Key::Num6: return PlatformUtils::Get().GetVirtualKeyCode(Key::Num7);
        case Key::Num7: return PlatformUtils::Get().GetVirtualKeyCode(Key::Num8);
        case Key::Num8: return PlatformUtils::Get().GetVirtualKeyCode(Key::Num9);
        case Key::Num9: return PlatformUtils::Get().GetVirtualKeyCode(Key::Num0);

        default: return 0; // Unknown key
    }
}
